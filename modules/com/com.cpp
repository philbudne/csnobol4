/* $Id$ */

/*
 * COM (OLE Automation) interface for CSNOBOL4
 * -plb 6/19/2004
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define DEBUGF(X) printf X
#else
#define DEBUGF(X)
#endif

// compiles w/ VC 2019, cygwin64 3.1.7 9/2020
#include <windows.h>
#include <objbase.h>
#include <oleauto.h>
#include <wchar.h>

#define LA_DISP(N) (LPDISPATCH)lookup_handle(&com_handles, LA_HANDLE(N))

extern "C"
{
#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */
#include "handle.h"

MODULE(com);

static handle_handle_t com_handles;

// return a wide (OLE) string for an external function argument
// must call freeolestring to release after use
static LPOLESTR
getolestring(void *vp)
{
    char *narrow = mgetstring(vp);	// handles null vp
    int len = MultiByteToWideChar(CP_ACP, 0, narrow, -1, NULL, 0);
    LPWSTR p = new WCHAR[len];
    MultiByteToWideChar(CP_ACP, 0, narrow, -1, p, len);
    free(narrow);
    return p;
}

static void
freeolestring(LPOLESTR ptr)
{
    delete [] ptr;
}

static void
free_obj(void *x) {
    LPDISPATCH pdisp = (LPDISPATCH) x;
    if (pdisp)
	pdisp->Release();
}

static handle_t *
new_obj(LPDISPATCH pdisp) {
    return new_handle2(&com_handles, pdisp, "com_handles", free_obj, &module);
}

//
// LOAD("COM_LOAD(STRING)INTEGER", COM_DL)
//
// returns integer handle to object, or fails
int
COM_LOAD( LA_ALIST ) LA_DCL
{
    CLSID clsid;
    LPUNKNOWN punk = NULL;
    LPDISPATCH pdisp = NULL;
    LPOLESTR progid;
    HRESULT hr;
    static bool first = true;			// XXX bad for making DLL!!
    snohandle_t h;

    if (first) {
	if (FAILED(CoInitialize(NULL)))
	    RETFAIL;
	first = false;
    }

    progid = getolestring(LA_PTR(0));
    if (wcschr(progid, ':')) {		// moniker display name?
	hr = CoGetObject(progid,	// display name
			NULL,		// BIND_OPTS*
			IID_IUnknown, (void **)&punk);
	DEBUGF(("CoGetObject hr %x\n", hr));
	freeolestring(progid);
	if (FAILED(hr))
	    RETFAIL;

	// See if it has a class factory
	IClassFactory *pfact;
	hr = punk->QueryInterface(IID_IClassFactory, (void **)&pfact);
	DEBUGF(("QueryInterface hr %x\n", hr));
	if (SUCCEEDED(hr)) {
	    punk->Release();

	    // try to create an instance using class factory
	    hr = pfact->CreateInstance(NULL,	// agregate object
					IID_IUnknown,
					(void **)&punk);
	    DEBUGF(("CreateInstance hr %x\n", hr));
	    pfact->Release();
	    if (FAILED(hr))
		RETFAIL;
	} // have class factory
    } // moniker display name
    else {
	// lookup classid from program ID string
	hr = CLSIDFromProgID(progid, &clsid);
	DEBUGF(("CLSIDFromProgID hr %x\n", hr));
	freeolestring(progid);
	if (FAILED(hr))
	    RETFAIL;

	// create an instance of the IUnknown object
	// CoCreateInstanceEx takes COSERVERINFO (remote host)
	hr = CoCreateInstance(clsid,	// object class id
			      NULL,	// agregate object
			      CLSCTX_SERVER, // context
			      IID_IUnknown, // interface identifier
			      (void **)&punk); // out: interface pointer
	DEBUGF(("CoCreateInstance hr %x\n", hr));
	if (FAILED(hr))
	    RETFAIL;
    }

    // get IDispatch (automation) interface pointer from IUnknown object
    hr = punk->QueryInterface(IID_IDispatch, (void **)&pdisp);
    DEBUGF(("QueryInterface hr %x\n", hr));

    // undo AddRef performed by CoCreateInstance
    punk->Release();

    if (FAILED(hr))
	RETFAIL;

    h = new_obj(pdisp);
    if (!OK_HANDLE(h)) {
	pdisp->Release();
	RETFAIL;
    }
    RETHANDLE(h);
} // COM_LOAD


// convert a SNOBOL4 value descriptor to a Windows VARIANTARG
static bool
descr_to_variant(struct descr *dp, VARIANTARG *vp)
{
    VariantInit(vp);
    switch (D_V(dp)) {
    case I:
	if (sizeof(int_t) == 8) {
	    V_VT(vp) = VT_I8;
	    V_I8(vp) = D_A(dp);
	    return true;
	}
	else
	{
	    V_VT(vp) = VT_I4;
	    V_I4(vp) = D_A(dp);
	    return true;
	}
	break;
    case R:
	if (sizeof(real_t) == 4) {
	    V_VT(vp) = VT_R4;
	    V_R4(vp) = D_RV(dp);
	    return true;
	}
	else if (sizeof(real_t) == 8) {
	    V_VT(vp) = VT_R8;
	    V_R8(vp) = D_RV(dp);
	    return true;
	}
	break;
    case S:
	// XXX return "empty" for null string?
	V_VT(vp) = VT_BSTR;
	V_BSTR(vp) = getolestring((void*)D_A(dp));
	return true;
    default:
	break;
    }
    V_VT(vp) = VT_EMPTY;
    return false;
} // descr_to_variant

// used to clean up args created by descr_to_variant
static void
freevariant(VARIANTARG *vp)
{
    if (!vp)
	return;

    switch (V_VT(vp)) {
    case VT_BSTR:
	freeolestring(V_BSTR(vp));
	break;
    }
} // freevariant

// convert a Windows wide string to a SNOBOL4 return value
// handle allocation failure?
static int
retbstring(struct descr *retval, LPOLESTR olestr)
{
    int len;
    len = WideCharToMultiByte(CP_ACP, 0, olestr, -1, NULL, 0, NULL, NULL);
    char *narrow = new char[len];
    WideCharToMultiByte(CP_ACP, 0, olestr, -1, narrow, len, NULL, NULL);

    // retstring usually hidden by a macro, but want to release
    // storage before return, since retstring copies the data
    // (otherwise would need to replicate retstring innards)
    retstring(retval, narrow, len);
    delete [] narrow;
    return TRUE;
} // retbstring

// convert Windows VARIANTARG to a SNOBOL4 external function return value
static int
retvariant(struct descr *retval, VARIANTARG *vp)
{
    int ret;

    switch (V_VT(vp) & VT_TYPEMASK) {
    case VT_EMPTY:
    case VT_NULL:
	RETNULL;
    case VT_I1:
	RETTYPE = I;
	RETINT(V_I1(vp));
    case VT_I2:
	RETTYPE = I;
	RETINT(V_I2(vp));
    case VT_I4:
	RETTYPE = I;
	RETINT(V_I4(vp));
#ifdef HAVE_I8
    case VT_I8:
	RETTYPE = I;
	RETINT(V_I8(vp));
#endif
    case VT_UI1:
	RETTYPE = I;
	RETINT(V_UI1(vp));
    case VT_UI2:
	RETTYPE = I;
	RETINT(V_UI2(vp));
    case VT_UI4:
	RETTYPE = I;
	RETINT(V_UI4(vp));
    case VT_R4:
	RETTYPE = R;
	RETREAL(V_R4(vp));
    case VT_R8:
	RETTYPE = R;
	RETREAL(V_R8(vp));
    case VT_BSTR:
	// convert (and free) string
	ret = retbstring(retval, V_BSTR(vp));
	VariantClear(vp);
	return ret;
    case VT_DISPATCH:			// pointer to IDispatch object
	{
	LPDISPATCH pdisp = V_DISPATCH(vp);
	snohandle_t h = new_obj(pdisp);
	if (!OK_HANDLE(h))
	    RETFAIL;
	pdisp->AddRef();
	// RETTYPE = I;			// removed 2020-10-17 leave as EXTERN
	RETHANDLE(h);
	}
    }
    // XXX COMPLAIN so new entries can be added?
    DEBUGF(("unknown type %d\n", V_VT(vp) & VT_TYPEMASK));
    RETNULL;				/* ?? */
} // retvariant

//
// Invoke as method
// Polymorphic!! Takes arbitrary list of args!
// LOAD("COM_INVOKE(INTEGER,STRING)", COM_DL)
//
// does not handle in-out parameters
// could have a version which takes an array?
//	(but would need to be able to intern new strings)
int
COM_INVOKE( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    if (!LA_PTR(1))
	RETFAIL;		// must have name string
    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid = -1;

    hr = pdisp->GetIDsOfNames(IID_NULL,		// REFIID
			      &name,		// OLECHAR**
			      1,		// name count
			      LOCALE_SYSTEM_DEFAULT, // locale id
			      &dispid);		// DISPID*
    DEBUGF(("INVOKE GetIDsOfNames hr %x dispid %x\n", hr, dispid));

    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;

    DISPPARAMS dispparams;
    memset(&dispparams, 0, sizeof dispparams);

    dispparams.cArgs = nargs - 2; // method arg count
    dispparams.rgvarg = NULL;
    dispparams.cNamedArgs = 0;
    dispparams.rgdispidNamedArgs = NULL; // Dispatch IDs of named arguments

    if (dispparams.cArgs) {
	dispparams.rgvarg = new VARIANTARG[dispparams.cArgs];
	if (!dispparams.rgvarg)
		RETFAIL;

	// copy in reverse order
	VARIANTARG* vp = dispparams.rgvarg;
	for (int i = nargs-1; i > nargs-2; i--) {
	    descr_to_variant(LA_DESCR(i), vp); // XXX check return??
	    vp++;
	}
    }

    VARIANTARG result;
    VariantInit(&result);
    hr = pdisp->Invoke(dispid,		// dispatch id member
		       IID_NULL,	// ref iid
		       LOCALE_SYSTEM_DEFAULT, // locale id
		       DISPATCH_METHOD|DISPATCH_PROPERTYGET,
		       &dispparams,
		       &result,		// VARIANT*
		       NULL,		// EXCEPINFO*
		       0);		// arg error pointer
    DEBUGF(("INVOKE Invoke hr %x\n", hr));

    // clean up argument array
    if (dispparams.cArgs) {
	VARIANTARG* vp = dispparams.rgvarg;
	for (int i = 0; i < dispparams.cArgs; i++) {
	    freevariant(vp);
	    vp++;
        }
	// free argument array
	delete [] dispparams.rgvarg;
    }
    if (FAILED(hr))
	RETFAIL;

    return retvariant(retval, &result);
} // COM_INVOKE

// LOAD("COM_GETPROP(INTEGER,STRING,)", COM_DL)
// Polymorphic!! Takes arbitrary list of args!
int
COM_GETPROP( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid= -1;
    hr = pdisp->GetIDsOfNames(IID_NULL, &name, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
    DEBUGF(("GETPROP GetIDsOfNames hr %x dispid %x\n", hr, dispid));
    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;

    DISPPARAMS dispparams;
    memset(&dispparams, 0, sizeof dispparams);

    dispparams.cArgs = nargs - 2; // method arg count
    dispparams.rgvarg = NULL;
    dispparams.cNamedArgs = 0;
    dispparams.rgdispidNamedArgs = NULL; // Dispatch IDs of named arguments

    if (dispparams.cArgs) {
	dispparams.rgvarg = new VARIANTARG[dispparams.cArgs];
	if (!dispparams.rgvarg)
		RETFAIL;

	// copy in reverse order
	VARIANTARG* vp = dispparams.rgvarg;
	for (int i = nargs-1; i > nargs-2; i--) {
	    descr_to_variant(LA_DESCR(i), vp); // XXX check return??
	    vp++;
	}
    }

    VARIANTARG result;
    VariantInit(&result);
    hr = pdisp->Invoke(dispid,		// dispatch id member
			IID_NULL,	// ref iid
			LOCALE_SYSTEM_DEFAULT, // locale id
			DISPATCH_PROPERTYGET, // flags
			&dispparams,
			&result,	// result
			NULL,		// EXCEPINFO
			0);		// arg error pointer

    // clean up argument array
    if (dispparams.cArgs) {
	VARIANTARG* vp = dispparams.rgvarg;
	for (int i = 0; i < dispparams.cArgs; i++) {
	    freevariant(vp);
	    vp++;
        }
	// free argument array
	delete [] dispparams.rgvarg;
    }

    DEBUGF(("GETPROP Invoke hr %x\n", hr));
    if (FAILED(hr))
	RETFAIL;

    return retvariant(retval, &result);
} // COM_GETPROP

// LOAD("COM_PUTPROP(INTEGER,STRING,)STRING", COM_DL)
// Polymorphic!! Takes arbitrary list of args!
int
COM_PUTPROP( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid = -1;
    hr = pdisp->GetIDsOfNames(IID_NULL, &name, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;


    VARIANTARG value;
    descr_to_variant(LA_DESCR(2), &value);

    DISPPARAMS dispparams;
    memset(&dispparams, 0, sizeof dispparams);

    DISPID mydispid = DISPID_PROPERTYPUT;

    dispparams.rgdispidNamedArgs = &mydispid;
    dispparams.rgvarg = &value;
    dispparams.cArgs = 1;
    dispparams.cNamedArgs = 1;

    hr = pdisp->Invoke(dispid,		// dispatch id member
			IID_NULL,	// ref iid
			LOCALE_SYSTEM_DEFAULT, // locale id
			DISPATCH_PROPERTYPUT, // flags
			&dispparams,
			NULL,		// result
			NULL,		// EXCEPINFO
			0);		// arg error pointer
    if (FAILED(hr))
	RETFAIL;

    RETNULL;
} // COM_PUTPROP

// LOAD("COM_RELEASE(INTEGER)STRING", COM_DL)
int
COM_RELEASE( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    remove_handle(&com_handles, LA_HANDLE(0));
    pdisp->Release();
    RETNULL;
} // COM_RELEASE

} // extern "C"
