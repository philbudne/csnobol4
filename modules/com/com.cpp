/* $Id$ */

/*
 * COM (OLE Automation) interface for CSNOBOL4
 * -plb 6/19/2004
 */

// #define HAVE_I8			// Not w/ MINGW 2.95.3-6

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <ole2.h>			// sufficient w/ MINGW 2.95.3-6
/*
#include <objbase.h>
#include <olestd.h>
*/

#ifndef V_I1
#define V_I1(X) V_UNION(X,cVal)
#endif
#ifndef V_I8
#define V_I8(X) V_UNION(X,llVal)
#endif

#ifndef V_UI1
#define V_UI1(X) V_UNION(X,bVal)
#endif
#ifndef V_UI2
#define V_UI2(X) V_UNION(X,uiVal)
#endif
#ifndef V_UI4
#define V_UI4(X) V_UNION(X,ulVal)
#endif

extern "C"
{
#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

// return a wide (OLE) string for an external function argument
// must call freeolestring to release after use
static LPOLESTR
getolestring(void *vp)
{
    char narrow[1024];			// UGH!!!
    getstring(vp, narrow, sizeof(narrow)); 

    int len = MultiByteToWideChar(CP_ACP, 0, narrow, -1, NULL, 0);
    LPWSTR p = new WCHAR[len];
    MultiByteToWideChar(CP_ACP, 0, narrow, -1, p, len);
    return p;
}

static void
freeolestring(LPOLESTR ptr)
{
    delete [] ptr;
}

int
COM_LOAD( LA_ALIST ) LA_DCL
{
    CLSID clsid;
    LPUNKNOWN punk = NULL;
    LPDISPATCH pdisp = NULL;
    LPOLESTR progid;
    HRESULT hr;
    static bool first = true;

    if (first) {
	if (FAILED(CoInitialize(NULL)))
	    RETFAIL;
	first = false;
    }

    progid = getolestring(LA_PTR(0));
    if (wcschr(progid, ':')) {			// moniker?
	hr = CoGetObject(name, NULL, IID_IUnknown, (void **)&punk)
	if (FAILED(hr))
	    RETFAIL;

	// See if it has a class factory
	IClassFactory *pfact;
	hr = punk->QueryInterface(IID_IClassFactory, (void **)&pfact);
	if (SUCCEEDED(hr)) {
	    punk->Release();

	    // try to create an instance using class factory
	    hr = pfact->CreateInstance(NULL, IID_IUnknown, (void **)&punk);
	    if (FAILED(hr))
		RETFAIL;
	    pfact->Release();
	} // have class factory
    } // moniker
    else {
	// lookup classid from program ID string
	hr = CLSIDFromProgID(progid, &clsid);
	freeolestring(progid);
	if (FAILED(hr))
	    RETFAIL;

	// create an instance of the IUnknown object
	hr = CoCreateInstance(clsid,	// object class id
			      NULL,		// agregate object
			      CLSCTX_SERVER, // context
			      IID_IUnknown,	// interface identifier
			      (void **)&punk); // out: interface pointer
	if (FAILED(hr))
	    RETFAIL;
    }

    // get IDispatch (automation) interface pointer
    // (a noop? just request IDispatch above?)
    hr = punk->QueryInterface(IID_IDispatch, (void **)&pdisp);

    // undo AddRef performed by CoCreateInstance
    punk->Release();

    if (FAILED(hr))
	RETFAIL;

    RETINT((int_t)pdisp);	// XXX UGH! encode as string? return small int?
} // COM_LOAD

static bool
descr_to_variant(struct descr *dp, VARIANTARG *vp)
{
    switch (D_V(dp)) {
    case I:
	if (sizeof(int_t) == 4) {
	    V_VT(vp) = VT_I4;
	    V_I4(vp) = D_A(dp);
	    return true;
	}
#ifdef HAVE_I8
	else if (sizeof(int_t) == 8) {
	    V_VT(vp) = VT_I8;
	    V_I8(vp) = D_A(dp);
	    return true;
	}
#endif
	break;
    case R:
	if (sizeof(real_t) == 4) {
	    V_VT(vp) = VT_R4;
	    V_R4(vp) = D_A(dp);
	    return true;
	}
	else if (sizeof(int_t) == 8) {
	    V_VT(vp) = VT_R8;
	    V_R8(vp) = D_A(dp);
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

static void
freevariant(VARIANTARG *vp)
{
    if (!vp)
	return;

    switch (V_VT(vp) & VT_TYPEMASK) {
    case VT_BSTR:
	freeolestring(V_BSTR(vp));
	break;
    }
} // freevariant

static int
retbstring(struct descr *retval, LPOLESTR olestr)
{
    int len;
    len = WideCharToMultiByte(CP_ACP, 0, olestr, -1, NULL, 0, NULL, NULL);
    char *narrow = new char[len];
    WideCharToMultiByte(CP_ACP, 0, olestr, -1, narrow, len, NULL, NULL);
    SysFreeString(olestr);

    retstring(retval, narrow, len);
    delete [] narrow;

    return TRUE;
} // retbstring

static int
retvariant(struct descr *retval, VARIANTARG *vp)
{
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
	return retbstring(retval, V_BSTR(vp));
    }
    RETNULL;				/* ?? */
} // retvariant

// XXX decode string?? lookup small integer? pointer to self-ref block???
#define LA_DISP(X) ((LPDISPATCH)LA_INT(X))

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
	RETFAIL;
    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid;

    hr = pdisp->GetIDsOfNames(IID_NULL,		// REFIID
			      &name,		// OLECHAR**
			      1,		// name count
			      LOCALE_SYSTEM_DEFAULT, // locale id
			      &dispid);		// DISPID*
    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;

    DISPPARAMS dispparams;
    int dargs = nargs-2;
    VARIANTARG* vp = NULL;
    if (dargs) {
	vp = (VARIANTARG*) CoTaskMemAlloc(dargs*sizeof(VARIANTARG));
	if (!vp)
	    RETFAIL;
    }

    dispparams.cArgs = dargs;
    dispparams.rgvarg = vp;

    dispparams.cNamedArgs = 0;
    dispparams.rgdispidNamedArgs = NULL; // Dispatch IDs of named arguments

    // copy in reverse order
    if (dargs) {
	for (int i = nargs-1; i > nargs-2; i--) {
	    descr_to_variant(LA_DESCR(i), vp); // XXX check return??
	    vp++;
	}
    }

    VARIANTARG result;
    hr = pdisp->Invoke(dispid,		// dispatch id member
		       IID_NULL,	// ref iid
		       LOCALE_SYSTEM_DEFAULT, // locale id
		       DISPATCH_METHOD|DISPATCH_PROPERTYGET,
		       &dispparams,
		       &result,		// VARIANT*
		       NULL,		// EXCEPINFO*
		       0);		// arg error pointer

    // clean up argument array
    if (dargs) {
	vp = dispparams.rgvarg;
	for (int i = nargs; i > nargs-2; i--) {
	    freevariant(vp);
	    vp++;
        }
	CoTaskMemFree(dispparams.rgvarg);
    }

    return retvariant(retval, &result);
} // COM_INVOKE

int
COM_GETPROP( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid;
    hr = pdisp->GetIDsOfNames(IID_NULL, &name, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;

    DISPPARAMS dispparams;

    dispparams.cArgs = 0;
    dispparams.cNamedArgs = 0;

    VARIANTARG result;
    hr = pdisp->Invoke(dispid,		// dispatch id member
			IID_NULL,	// ref iid
			LOCALE_SYSTEM_DEFAULT, // locale id
			DISPATCH_PROPERTYGET, // flags
			&dispparams,
			&result,	// result
			NULL,		// EXCEPINFO
			0);		// arg error pointer

    if (FAILED(hr))
	RETFAIL;

    return retvariant(retval, &result);
} // COM_GETPROP

int
COM_PUTPROP( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    LPOLESTR name = getolestring(LA_PTR(1));
    HRESULT hr;
    DISPID dispid;
    hr = pdisp->GetIDsOfNames(IID_NULL, &name, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
    freeolestring(name);
    if (FAILED(hr))
	RETFAIL;

    DISPPARAMS dispparams;

    VARIANTARG value;
    descr_to_variant(LA_DESCR(2), &value);
    dispparams.cArgs = 1;
    dispparams.rgvarg = &value;

    DISPID mydispid = DISPID_PROPERTYPUT;
    dispparams.cNamedArgs = 1;
    dispparams.rgdispidNamedArgs = &mydispid;

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


int
COM_UNLOAD( LA_ALIST ) LA_DCL
{
    LPDISPATCH pdisp = LA_DISP(0);
    if (!pdisp)
	RETFAIL;

    pdisp->Release();
    RETNULL;
} // COM_UNLOAD

} // extern "C"
