/* $Id$ */

/*
 * Tcl/Tk loadable module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 6/23/2004
 *
 * Inspired by Arjen Markus' "ftcl" FORTRAN/Tcl interface
 * As mentioned in Usenix ";login:" newsletter June 2004
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

/* only allows a single instance!! */
static Tcl_Interp *stcl_interp;		/* XXX static!! */

/*
 * LOAD("STCL_START(STRING)STRING")
 * Take arg as filename; initialize Tcl interpreter, read file.
 * Call only once!!
 *
 * returns null string or failure
 */
int
STCL_INIT( LA_ALIST ) LA_DCL
{
    char file[1024];			/* XXX */

    stcl_interp = Tcl_CreateInterp();
    if (!stcl_interp)
	RETFAIL;

    if (Tcl_Init(stcl_interp) == TCL_ERROR)
	RETFAIL;

    getstring(LA_PTR(0), file, sizeof(file));
    if (Tcl_EvalFile(stcl_interp, file) != TCL_OK)
	RETFAIL;

    RETNULL;
}

/*
 * LOAD("STCL_GETVAR(STRING)STRING")
 * return value of a Tcl variable (all Tcl variables are strings)
 */
int
STCL_GETVAR( LA_ALIST ) LA_DCL
{
    char name[1024];			/* XXX */
    char *val;

    getstring(LA_PTR(0), name, sizeof(name));
    val = Tcl_GetVar(ftcl_interp, name, 0);
    RETSTR(val);
}

/*
 * LOAD("STCL_SETVAR(STRING,STRING)STRING")
 * Set value of a Tcl variable
 *
 * returns null string or failure
 */
int
STCL_GETVAR( LA_ALIST ) LA_DCL
{
    char name[1024];			/* XXX */
    char val[1024];			/* XXX */

    getstring(LA_PTR(0), name, sizeof(name));
    getstring(LA_PTR(1), val, sizeof(val));
    if (!Tcl_SetVar(stcl_interp, name, value, 0))
	RETFAIL;
    RETNULL;
}
