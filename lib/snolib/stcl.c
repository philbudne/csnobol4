/* $Id$ */

/*
 * Tcl/Tk loadable module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 6/23/2004
 *
 * Inspired by Arjen Markus' "ftcl" FORTRAN/Tcl interface
 *	http://www.digitalsmarties.com/tcl/ftcl.zip
 * As mentioned in Clif Flynt's Usenix ";login:" newsletter column June 2004
 *	http://www.usenix.org/publications/login/2004-06/pdfs/flynt.pdf
 *
 * ISSUES:
 * Remove IncrRefCounts?  Add explicit calls?
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* for free() */
#endif

#include <tcl.h>
#ifdef STCL_USE_TK
#include <tk.h>
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"

static handle_handle_t tcl_interps;
static handle_handle_t tcl_objs;	/* Objects NOT per-interp!! */

/*
 * LOAD("STCL_CREATEINTERP()INTEGER", STCL_DL)
 * Create and initialize a TCL interpreter
 *
 * return handle, or failure
 */
int
STCL_CREATEINTERP( LA_ALIST ) LA_DCL
{
    snohandle_t h;
    Tcl_Interp *interp = Tcl_CreateInterp();

    if (!interp)
	RETFAIL;

    if (Tcl_Init(interp) == TCL_ERROR) {
	Tcl_DeleteInterp(interp);
	RETFAIL;
    }

#ifdef STCL_USE_TK
    /* init can fail if $DISPLAY not set -- ignore */
    Tk_Init(interp);			/* XXX check return? */
#endif

    h = new_handle(&tcl_interps, interp);
    if (h == BAD_HANDLE) {
	Tcl_DeleteInterp(interp);
	/* XXX Release? */
	/* XXX remove_handle? */
	RETFAIL;
    }
    /* XXX Release? */
    RETINT(h);				/* XXX make string tcl%d? */
}

/*
 * LOAD("STCL_EVALFILE(INTEGER,STRING)STRING", STCL_DL)
 *
 * return result string, or failure
 */
int
STCL_EVALFILE( LA_ALIST ) LA_DCL
{
    char *file;
    int ret;

    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    if (!interp)
	RETFAIL;

    file = mgetstring(LA_PTR(0));
    ret = Tcl_EvalFile(interp, file);
    free(file);
    if (ret != TCL_OK)
	RETFAIL;

    RETSTR(Tcl_GetStringResult(interp));
}

/*
 * LOAD("STCL_GETVAR(INTEGER,STRING)STRING", STCL_DL)
 * return value of a Tcl variable (all Tcl variables are strings)
 */
int
STCL_GETVAR( LA_ALIST ) LA_DCL
{
    char *name;
    const char *val;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    if (!interp)
	RETFAIL;
    name = mgetstring(LA_PTR(1));
    val = Tcl_GetVar(interp, name, 0);
    RETSTR(val);
}

/*
 * LOAD("STCL_SETVAR(INTEGER,STRING,STRING)STRING", STCL_DL)
 * Set value of a Tcl variable
 *
 * returns null string or failure 
*/
int
STCL_SETVAR( LA_ALIST ) LA_DCL
{
    char *name;
    char *value;
    int ret;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    if (!interp)
	RETFAIL;
    name = mgetstring(LA_PTR(1));
    value = mgetstring(LA_PTR(2));
    ret = Tcl_SetVar(interp, name, value, 0);
    free(name);
    free(value);
    if (!ret)
	RETFAIL;
    RETNULL;
}

/*
 * LOAD("STCL_EVAL(INTEGER,STRING)STRING", STCL_DL)
 * Eval a tcl command
 *
 * returns result string or failure
 */
int
STCL_EVAL( LA_ALIST ) LA_DCL
{
    char *cmd;
    int ret;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    if (!interp)
	RETFAIL;
    cmd = mgetstring(LA_PTR(1));
    ret = Tcl_Eval(interp, cmd);
    free(cmd);
    if (ret != TCL_OK)
	RETFAIL;

    RETSTR(Tcl_GetStringResult(interp));
}

/*
 * LOAD("STCL_DELETEINTERP(INTEGER)STRING", STCL_DL)
 * Delete TCL interpreter
 *
 * return null string, or failure
 */
int
STCL_DELETEINTERP( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    if (!interp)
	RETFAIL;
    Tcl_DeleteInterp(interp);
    remove_handle(&tcl_interps, LA_INT(0)); /* gone to SNOBOL world... */
    RETNULL;
}

/*****************************************************************
 * new functions for Tcl Object interface
 * 9/1/2004
 */

/*
 * LOAD("STCL_NEWSTRINGOBJ(STRING)INTEGER", STCL_DL)
 * Create new string object, returns handle
 */
int
STCL_NEWSTRINGOBJ( LA_ALIST ) LA_DCL
{
    Tcl_Obj *obj;
    int h;

    obj = Tcl_NewStringObj(LA_STR_PTR(0), LA_STR_LEN(0));

    if (!obj)
	RETFAIL;

    h = new_handle(&tcl_objs, obj);
    if (h == BAD_HANDLE)
	RETFAIL;

    Tcl_IncrRefCount(obj);		/* XXX? */
    RETINT(h);
}

/*
 * LOAD("STCL_GETSTRINGFROMOBJ(INTEGER)STRING", STCL_DL)
 * Get string from an Object (given object handle)
 */
int
STCL_GETSTRINGFROMOBJ( LA_ALIST ) LA_DCL
{
    int length;
    Tcl_Obj *obj;
    char *val;

    obj = lookup_handle(&tcl_objs, LA_INT(0));
    if (!obj)
	RETFAIL;

    val = Tcl_GetStringFromObj(obj, &length);
    if (!val)
	RETFAIL;
    RETSTR2(val, length);
}

/*
 * LOAD("STCL_APPENDTOOBJ(INTEGER,STRING)STRING", STCL_DL)
 * Append string to an Object.
 * returns null string, or failure
 */
int
STCL_APPENDTOOBJ( LA_ALIST ) LA_DCL
{
    Tcl_Obj *obj;

    obj = lookup_handle(&tcl_objs, LA_INT(0));
    if (!obj)
	RETFAIL;

    Tcl_AppendToObj(obj, LA_STR_PTR(1), LA_STR_LEN(1));
    RETNULL;
}

/*
 * LOAD("STCL_EVALOBJEX(INTEGER,INTEGER,INTEGER)STRING", STCL_DL)
 * Evaluate (execute) an object -- saves compiled byte code
 */
int
STCL_EVALOBJEX( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp = lookup_handle(&tcl_objs, LA_INT(0));
    Tcl_Obj *obj = lookup_handle(&tcl_objs, LA_INT(1));
    int ret;

    if (!interp || !obj)
	RETFAIL;

    ret = Tcl_EvalObjEx(interp, obj, LA_INT(2));
    RETINT(ret);
}

/*
 * LOAD("STCL_GETOBJRESULT(INTEGER)", STCL_DL)
 * return a result object from an interpreter (after Tcl_EvalObjEx)
 */
int
STCL_GETOBJRESULT(LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp = lookup_handle(&tcl_objs, LA_INT(0));
    Tcl_Obj *obj = Tcl_GetObjResult(interp);
    int h;

    if (!interp || !obj)
	RETFAIL;

    h = new_handle(&tcl_objs, obj);
    if (h == BAD_HANDLE)
	RETFAIL;

    Tcl_IncrRefCount(obj);
    RETINT(h);
}

/*
 * LOAD("STCL_OBJSETVAR2(INTEGER,INTEGER,INTEGER,INTEGER,INTEGER)STRING", STCL_DL)
 */
int
STCL_OBJSETVAR2( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    Tcl_Obj *part1 = lookup_handle(&tcl_objs, LA_INT(1));
    Tcl_Obj *part2 = lookup_handle(&tcl_objs, LA_INT(2)); /* index */
    Tcl_Obj *val = lookup_handle(&tcl_objs, LA_INT(3));	/* new value */
    Tcl_Obj *res;
    int h;

    if (!interp)
	RETFAIL;

    res = Tcl_ObjSetVar2(interp, part1, part2, val, LA_INT(4));
    if (!res)
	RETFAIL;

    h = new_handle(&tcl_objs, res);
    if (h == BAD_HANDLE)
	RETFAIL;

    Tcl_IncrRefCount(res);		/* XXX needed? */
    RETINT(h);
}

/*
 * LOAD("STCL_OBJGETVAR2(INTEGER,STRING,STRING,INTEGER)STRING", STCL_DL)
 */
int
STCL_OBJGETVAR2( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_INT(0));
    Tcl_Obj *part1 = lookup_handle(&tcl_objs, LA_INT(1));
    Tcl_Obj *part2 = lookup_handle(&tcl_objs, LA_INT(2));
    Tcl_Obj *res;
    int h;

    if (!interp)
	RETFAIL;

    res = Tcl_ObjGetVar2(interp, part1, part2, LA_INT(3));
    if (!res)
	RETFAIL;

    h = new_handle(&tcl_objs, res);
    if (h == BAD_HANDLE)
	RETFAIL;

    Tcl_IncrRefCount(res);
    RETINT(h);
}

/*
 * LOAD("STCL_RELEASEOBJ(INTEGER)STRING", STCL_DL)
 * release a Tcl Object
 */
int
STCL_RELEASEOBJ( LA_ALIST ) LA_DCL
{
    Tcl_Obj *obj = lookup_handle(&tcl_objs, LA_INT(0));
    if (!obj)
	RETFAIL;
    Tcl_DecrRefCount(obj);
    /* XXX check IsShared? */
    remove_handle(&tcl_objs, LA_INT(0)); /* gone to SNOBOL world... */
    RETNULL;
}
