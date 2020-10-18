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

/*
**=pea
**=sect NAME
**snobol4tcl \- SNOBOL4 Tcl/Tk interface
**
**=sect SYNOPSIS
**=code
**\~\~\~-INCLUDE 'stcl.sno'
**=ecode
**
**=sect DESCRIPTION
**Tcl is an embedable scripting language developed by John Osterhout,
**while at the University of California, Berkeley.  Tk is a graphical
**user interface toolkit developed for Tcl.
**
**This page describes a facility for invoking Tcl and Tk from SNOBOL4
**programs, inspired by Arjen Markus' "ftcl" FORTRAN/Tcl interface
**=cut
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for free() */

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

static void
free_obj(void *x) {
    Tcl_DecrRefCount(x);
}

static snohandle_t
new_obj(Tcl_Obj *obj) {
    return new_handle2(&tcl_objs, obj, "tcl_objs", free_obj, &module);
}

/*
**=pea
**=item I<tclhandle> = B<STCL_CREATEINTERP()>
**creates a Tcl interpreter and returns a handle which can be passed to
**the remaining functions.
**=cut
*/

static void
free_interp(void *x) {
    Tcl_DeleteInterp(x);
}

/*
 * LOAD("STCL_CREATEINTERP()INTEGER", STCL_DL)
 * Create and initialize a TCL interpreter
 *
 * return handle, or failure
 */
lret_t
STCL_CREATEINTERP( LA_ALIST ) {
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

    h = new_handle2(&tcl_interps, interp, "tcl_interps", free_interp, &module);
    if (!OK_HANDLE(h)) {
	Tcl_DeleteInterp(interp);
	/* XXX Release? */
	/* XXX remove_handle? */
	RETFAIL;
    }
    /* XXX Release? */
    RETHANDLE(h);				/* XXX make string tcl%d? */
}

/*
**=pea
**=item B<STCL_EVALFILE>(I<tclhandle>,I<filename>)
**reads a Tcl script file into the referenced Tcl interpreter.
**
**=cut
*/
/*
 * LOAD("STCL_EVALFILE(EXTERNAL,STRING)STRING", STCL_DL)
 *
 * return result string, or failure
 */
lret_t
STCL_EVALFILE( LA_ALIST ) {
    char *file;
    int ret;

    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
    if (!interp)
	RETFAIL;

    file = mgetstring(LA_PTR(1));
    ret = Tcl_EvalFile(interp, file);
    free(file);
    if (ret != TCL_OK)
	RETFAIL;

    RETSTR(Tcl_GetStringResult(interp));
}

/*
**=pea
**=item B<STCL_GETVAR>(I<tclhandle>,I<varname>)
**retrieves the string value of named variable from a Tcl interpreter.
**=cut
*/
/*
 * LOAD("STCL_GETVAR(EXTERNAL,STRING)STRING", STCL_DL)
 * return value of a Tcl variable (all Tcl variables are strings)
 */
lret_t
STCL_GETVAR( LA_ALIST ) {
    char *name;
    const char *val;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
    if (!interp)
	RETFAIL;
    name = mgetstring(LA_PTR(1));
    val = Tcl_GetVar(interp, name, 0);
    free(name);
    RETSTR(val);
}

/*
**=pea
**=item B<STCL_SETVAR>(I<tclhandle>,I<varname>,I<value>)
**sets the string value of named variable in a Tcl interpreter.
**=cut
*/
/*
 * LOAD("STCL_SETVAR(EXTERNAL,STRING,STRING)STRING", STCL_DL)
 * Set value of a Tcl variable
 *
 * returns null string or failure 
*/
lret_t
STCL_SETVAR( LA_ALIST ) {
    char *name;
    char *value;
    const char *ret;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
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
**=pea
**=item B<STCL_EVAL>(I<tclhandle>,I<tclstmt>)
**evaluates a string containing Tcl code in a Tcl interpreter.
**
**=cut
*/
/*
 * LOAD("STCL_EVAL(EXTERNAL,STRING)STRING", STCL_DL)
 * Eval a tcl command
 *
 * returns result string or failure
 */
lret_t
STCL_EVAL( LA_ALIST ) {
    char *cmd;
    int ret;
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
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
**=pea
**=item B<STCL_DELETEINTERP>(I<tclhandle>)
**destroys a Tcl interpreter.
**
**=cut
*/
/*
 * LOAD("STCL_DELETEINTERP(EXTERNAL)STRING", STCL_DL)
 * Delete TCL interpreter
 *
 * return null string, or failure
 */
lret_t
STCL_DELETEINTERP( LA_ALIST ) {
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
    if (!interp)
	RETFAIL;
    Tcl_DeleteInterp(interp);
    remove_handle(&tcl_interps, LA_HANDLE(0)); /* gone to SNOBOL world... */
    RETNULL;
}

/*****************************************************************
 * new functions for Tcl Object interface
 * 9/1/2004
 */

/*
**=pea
**=item B<STCL_NEWSTRINGOBJ>(I<string>)
**Creates a tcl string object, and returns a handle for it.
**=cut
*/
/*
 * LOAD("STCL_NEWSTRINGOBJ(STRING)EXTERNAL", STCL_DL)
 * Create new string object, returns handle
 */
lret_t
STCL_NEWSTRINGOBJ( LA_ALIST ) {
    Tcl_Obj *obj;
    snohandle_t h;

    obj = Tcl_NewStringObj(LA_STR_PTR(0), LA_STR_LEN(0));

    if (!obj)
	RETFAIL;

    h = new_obj(obj);
    if (!OK_HANDLE(h))
	RETFAIL;

    Tcl_IncrRefCount(obj);		/* XXX? */
    RETHANDLE(h);
}

/*
**=pea
**=item B<STCL_GETSTRINGFROMOBJ>(I<objhandle>)
**Get string from an Object (given object handle).
**=cut
*/
/*
 * LOAD("STCL_GETSTRINGFROMOBJ(EXTERNAL)STRING", STCL_DL)
 */
lret_t
STCL_GETSTRINGFROMOBJ( LA_ALIST ) {
    int length;
    Tcl_Obj *obj;
    char *val;

    obj = lookup_handle(&tcl_objs, LA_HANDLE(0));
    if (!obj)
	RETFAIL;

    val = Tcl_GetStringFromObj(obj, &length);
    if (!val)
	RETFAIL;
    RETSTR2(val, length);
}

/*
**=pea
**=item B<STCL_APPENDTOOBJ>(I<objhandle>, I<string>)
**Append string to an Object returns null string, or failure
**=cut
*/
/*
 * LOAD("STCL_APPENDTOOBJ(EXTERNAL,STRING)STRING", STCL_DL)
 */
lret_t
STCL_APPENDTOOBJ( LA_ALIST ) {
    Tcl_Obj *obj;

    obj = lookup_handle(&tcl_objs, LA_HANDLE(0));
    if (!obj)
	RETFAIL;

    Tcl_AppendToObj(obj, LA_STR_PTR(1), LA_STR_LEN(1));
    RETNULL;
}

/*
**=pea
**=item B<STCL_EVALOBJEX>(I<tclhandle>, I<objhandle>, I<flags>)
**Evaluate (execute) an object -- saves compiled byte code.
**Returns integer.
**=cut
*/
/*
 * LOAD("STCL_EVALOBJEX(EXTERNAL,EXTERNAL,INTEGER)STRING", STCL_DL)
 */
lret_t
STCL_EVALOBJEX( LA_ALIST ) {
    Tcl_Interp *interp = lookup_handle(&tcl_objs, LA_HANDLE(0));
    Tcl_Obj *obj = lookup_handle(&tcl_objs, LA_HANDLE(1));
    int ret;

    if (!interp || !obj)
	RETFAIL;

    ret = Tcl_EvalObjEx(interp, obj, LA_INT(2));
    RETINT(ret);
}

/*
**=pea
**=item I<objhandle> = B<STCL_GETOBJRESULT>(I<tclhandle>)
**return a result object from an interpreter (after B<TCL_EVALOBJEX>)
**=cut
*/
/*
 * LOAD("STCL_GETOBJRESULT(EXTERNAL)", STCL_DL)
 */
lret_t
STCL_GETOBJRESULT(LA_ALIST ) {
    Tcl_Interp *interp = lookup_handle(&tcl_objs, LA_HANDLE(0));
    Tcl_Obj *obj = Tcl_GetObjResult(interp);
    snohandle_t h;

    if (!interp || !obj)
	RETFAIL;

    h = new_obj(obj);
    if (!OK_HANDLE(h))
	RETFAIL;

    Tcl_IncrRefCount(obj);
    RETHANDLE(h);
}

/*
**=pea
**=item B<STCL_OBJSETVAR2>(I<tclhandle>, I<oh_name1>, I<oh_name2>, I<oh_value>, I<flags>)
**=cut
*/
/*
 * LOAD("STCL_OBJSETVAR2(HANDLE,HANDLE,HANDLE,HANDLE,INTEGER)STRING", STCL_DL)
 */
lret_t
STCL_OBJSETVAR2( LA_ALIST ) {
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
    Tcl_Obj *part1 = lookup_handle(&tcl_objs, LA_HANDLE(1));
    Tcl_Obj *part2 = lookup_handle(&tcl_objs, LA_HANDLE(2)); /* index */
    Tcl_Obj *val = lookup_handle(&tcl_objs, LA_HANDLE(3));	/* new value */
    Tcl_Obj *res;
    snohandle_t h;

    if (!interp)
	RETFAIL;

    res = Tcl_ObjSetVar2(interp, part1, part2, val, LA_INT(4));
    if (!res)
	RETFAIL;

    h = new_obj(res);
    if (!OK_HANDLE(h))
	RETFAIL;

    Tcl_IncrRefCount(res);		/* XXX needed? */
    RETHANDLE(h);
}

/*
**=pea
**=item B<STCL_OBJGETVAR2>(I<tclhandle>, I<oh_name1>, I<oh_name2>, I<flags>)
**=cut
*/
/*
 * LOAD("STCL_OBJGETVAR2(HANDLE,HANDLE,HANDLE,INTEGER)STRING", STCL_DL)
 */
lret_t
STCL_OBJGETVAR2( LA_ALIST ) {
    Tcl_Interp *interp = lookup_handle(&tcl_interps, LA_HANDLE(0));
    Tcl_Obj *part1 = lookup_handle(&tcl_objs, LA_HANDLE(1));
    Tcl_Obj *part2 = lookup_handle(&tcl_objs, LA_HANDLE(2));
    Tcl_Obj *res;
    snohandle_t h;

    if (!interp)
	RETFAIL;

    res = Tcl_ObjGetVar2(interp, part1, part2, LA_INT(3));
    if (!res)
	RETFAIL;

    h = new_obj(res);
    if (!OK_HANDLE(h))
	RETFAIL;

    Tcl_IncrRefCount(res);
    RETHANDLE(h);
}

/*
**=pea
**=item B<STCL_RELEASEOBJ>(I<objhandle>)
**release a Tcl Object
**=cut
*/
/*
 * LOAD("STCL_RELEASEOBJ(HANDLE)STRING", STCL_DL)
 */
lret_t
STCL_RELEASEOBJ( LA_ALIST ) {
    Tcl_Obj *obj = lookup_handle(&tcl_objs, LA_HANDLE(0));
    if (!obj)
	RETFAIL;
    Tcl_DecrRefCount(obj);
    /* XXX check IsShared? */
    remove_handle(&tcl_objs, LA_HANDLE(0)); /* gone to SNOBOL world... */
    RETNULL;
}

/*
**=pea
**=sect EXAMPLE
**=code
**-INCLUDE 'stcl.sno'
**        INTERP = STCL_CREATEINTERP()
**        TCL_VERSION = STCL_GETVAR(INTERP, "tcl_version")
**        OUTPUT = IDENT(TCL_VERSION) "Could not get tcl_version" :S(END)
**        OUTPUT = "Tcl Version: " TCL_VERSION
** 
**# check Tcl version
**        NUM = SPAN('0123456789')
**        VPAT = NUM '.' NUM
**        TCL_VERSION VPAT . VER                          :S(CHECKV)
**        OUTPUT = "could not parse tcl_version"          :(END)
** 
**CHECKV  LT(VER, 8.4)                                    :S(CHECKTK)
** 
**# Tcl 8.4 and later can dynamicly load Tk!
**        STCL_EVAL(INTERP, "package require Tk")         :F(END)
** 
**# Check for Tk
**CHECKTK TK_VERSION = STCL_GETVAR(INTERP, "tk_version")  :F(NO_TK)
**        DIFFER(TK_VERSION)                              :S(HAVETK)
**NO_TK   OUTPUT = "Could not find tk_version"            :(END)
**
**HAVETK  OUTPUT = "Tk version: " TK_VERSION
**
**LOOP    OUTPUT = STCL_EVAL(INTERP,
**+                   'tk_messageBox -message "Alert!"'
**+                   ' -type ok -icon info')
**        VAL = STCL_EVAL(INTERP,
**+                   'tk_messageBox -message "Really quit?"'
**+                   ' -type yesno -icon question')
**        OUTPUT = VAL
**        DIFFER(VAL, "yes")                              :S(LOOP)
**END
**=ecode
**
**=sect SEE ALSO
**B<tclsh>(1), B<wish>(1).
**=break
**=link http://ftcl.sourceforge.net/
**
**=sect AUTHOR
**Philip L. Budne
**=cut
*/
