/* $Id$ */

/*
 * Tcl/Tk loadable module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 6/23/2004
 *
 * Inspired by Arjen Markus' "ftcl" FORTRAN/Tcl interface
 * As mentioned in Clif Flynt's Usenix ";login:" newsletter column June 2004
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <tcl.h>
#ifndef STCL_NO_TK
#include <tk.h>
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"

static struct handle_list tcl_handles;

/*
 * LOAD("STCL_CREATE()INTEGER")
 * Create and initialize TCL interpreter
 *
 * return handle, or failure
 */
int
STCL_CREATE( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp;
    snohandle_t h;

    interp = Tcl_CreateInterp();
    if (!interp)
	RETFAIL;

    if (Tcl_Init(interp) == TCL_ERROR) {
	Tcl_DeleteInterp(interp);
	RETFAIL;
    }

#ifndef STCL_NO_TK
    /* bring in Tk; define tk functions */
    if (Tk_Init(interp) == TCL_ERROR) {
	Tcl_DeleteInterp(interp);
	RETFAIL;
    }
#endif

    h = new_handle(&tcl_handles, interp);
    if (h == BAD_HANDLE) {
	Tcl_DeleteInterp(interp);
	RETFAIL;
    }
    RETINT(h);				/* XXX make string tcl%d? */
}

/*
 * LOAD("STCL_EVALFILE(INTEGER,STRING)STRING")
 * Create and initialize TCL interpreter
 *
 * return null string, or failure
 */
int
STCL_EVALFILE( LA_ALIST ) LA_DCL
{
    char file[1024];			/* XXX */
    Tcl_Interp *interp;

    interp = lookup_handle(&tcl_handles, LA_INT(0));
    if (!interp)
	RETFAIL;

    getstring(LA_PTR(0), file, sizeof(file));
    if (Tcl_EvalFile(interp, file) != TCL_OK)
	RETFAIL;

    RETNULL;
}

/*
 * LOAD("STCL_GETVAR(INTEGER,STRING)STRING")
 * return value of a Tcl variable (all Tcl variables are strings)
 */
int
STCL_GETVAR( LA_ALIST ) LA_DCL
{
    char name[1024];			/* XXX */
    char *val;
    Tcl_Interp *interp;

    interp = lookup_handle(&tcl_handles, LA_INT(0));
    if (!interp)
	RETFAIL;

    getstring(LA_PTR(1), name, sizeof(name));
    val = Tcl_GetVar(interp, name, 0);
    RETSTR(val);
}

/*
 * LOAD("STCL_SETVAR(INTEGER,STRING,STRING)STRING")
 * Set value of a Tcl variable
 *
 * returns null string or failure 
*/
int
STCL_SETVAR( LA_ALIST ) LA_DCL
{
    char name[1024];			/* XXX */
    char value[1024];			/* XXX */
    Tcl_Interp *interp;

    interp = lookup_handle(&tcl_handles, LA_INT(0));
    if (!interp)
	RETFAIL;

    getstring(LA_PTR(1), name, sizeof(name));
    getstring(LA_PTR(2), value, sizeof(value));
    if (!Tcl_SetVar(interp, name, value, 0))
	RETFAIL;
    RETNULL;
}

/*
 * LOAD("STCL_EVAL(INTEGER,STRING)STRING")
 * Eval a tcl command
 *
 * returns null string or failure
 */
int
STCL_EVAL( LA_ALIST ) LA_DCL
{
    char cmd[1024];			/* XXX */
    Tcl_Interp *interp;

    interp = lookup_handle(&tcl_handles, LA_INT(0));
    if (!interp)
	RETFAIL;

    getstring(LA_PTR(1), cmd, sizeof(cmd));
    if (Tcl_Eval(interp, cmd) != TCL_OK)
	RETFAIL;

    RETNULL;
}

/*
 * LOAD("STCL_DELETE(INTEGER)STRING")
 * Create and initialize TCL interpreter
 *
 * return null string, or failure
 */
int
STCL_DELETE( LA_ALIST ) LA_DCL
{
    Tcl_Interp *interp;

    interp = lookup_handle(&tcl_handles, LA_INT(0));
    if (!interp)
	RETFAIL;

    Tcl_DeleteInterp(interp);
    RETNULL;
}
