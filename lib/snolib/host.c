/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("HOST(,)")
 *
 * Usage;	varies!
 * Returns;	varies!
 *
 * simulates basic HOST() functions.
 * NOTE: Performs *NO* argument conversions (not even string to integer)!!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

#ifdef NO_STATIC_VARS
#include "vars.h"
#else
extern int argc, firstarg;
extern char **argv;
extern char *params;
#endif

extern char *getenv();			/* use <stdlib.h> if avail? */

int
HOST( LA_ALIST ) LA_DCL
{
    char buf[512];			/* XXX */
    char *env;
    int n;

    /* if first arg is null string, do HOST() */
    if (LA_TYPE(0) == S && LA_INT(0) == 0) {
	char os[256], hw[256];
	extern const char snoname[], vers[];

	/* use routines from SYSDEP/sys.c; */
	osname(os);
	hwname(hw);

	sprintf(buf, "%s:%s:%s %s", hw, os, snoname, vers);
	RETSTR(buf);
    }

    /* first arg must be int (try to convert to int?) */
    if (LA_TYPE(0) != I) {
	RETFAIL;
    }

    switch (LA_INT(0)) {
    case 0:				/* HOST(0); all parameters (or -u) */
	RETSTR(params);

    case 1:				/* HOST(1,s); run subprocess */
	RETTYPE = I;			/* oof! blast return type! */
	if (nargs < 2 || LA_TYPE(1) != S) {
	    /* GNAT programs expect HOST(1) to return zero; */
	    RETINT(0);
	}
	getstring( LA_PTR(1), buf, sizeof(buf)); /* get arg as c-string */
	io_flushall(0);			/* flush output buffers */
	RETINT(system(buf));		/* run in sub-shell */

    case 2:				/* HOST(2,n); argument n */
	if (nargs < 2 || LA_TYPE(1) != I)
	    break;			/* missing, non-int */

	n = LA_INT(1);
	if (n < 0 || n >= argc)
	    break;			/* out of range; fail */
	RETSTR(argv[n]);		/* return n'th command line arg */

    case 3:				/* HOST(3); first unused argument */
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(firstarg);

    case 4:				/* HOST(4,s); environment var s */
	if (nargs < 2 || LA_TYPE(1) != S) {
	    RETFAIL;
	}
	getstring(LA_PTR(1), buf, sizeof(buf));
	env = getenv(buf);
	if (!env)
	    break;			/* fail if no such variable */
	RETSTR(env);

    default:
	break;
    }
    RETFAIL;
}
