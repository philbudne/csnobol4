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

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

extern char *getenv();
extern char *params;
extern char **argv;
extern int argc, firstarg;

int
HOST( LA_ALIST ) LA_DCL
{
    char buf[512];			/* XXX */
    char *env;
    int n;

    /* if first arg is null string, do HOST() */
    if (LA_TYPE(0) == S && LA_INT(0) == 0) {
	/* XXX seperate hwname(), osname()?  use defines?? */
	if (!sysname(buf)) {
	    RETFAIL;
	}
	RETSTR(buf, strlen(buf));
    }

    /* first arg must be int (try to convert to int?) */
    if (LA_TYPE(0) != I) {
	RETFAIL;
    }

    switch (LA_INT(0)) {
    case 0:				/* HOST(0); all parameters (or -u) */
	if (params)
	    RETSTR(params, strlen(params));
	RETSTR("",0);			/* else return null string */

    case 1:				/* HOST(1,s); run subprocess */
	D_V(retval) = I;		/* oof! blast return type! */
	if (nargs < 2 || LA_TYPE(1) != S) {
	    /* GNAT programs expect HOST(1) to return zero; */
	    RETINT(0);
	}
	getstring( LA_PTR(1), buf, sizeof(buf)); /* get arg as c-string */
	RETINT(system(buf));		/* run in sub-shell */

    case 2:				/* HOST(2,n); argument n */
	if (nargs < 2 || LA_TYPE(1) != I)
	    break;			/* missing, non-int */

	n = LA_INT(1);
	if (n < 0 || n >= argc)
	    break;			/* out of range */

	RETSTR(argv[n], strlen(argv[n])); /* return n'th arg */

    case 3:				/* HOST(3); first unused argument */
	D_V(retval) = I;		/* oof! blast return type! */
	RETINT(firstarg);

    case 4:				/* HOST(4,s); environment var s */
	if (nargs < 2 || LA_TYPE(1) != S) {
	    RETFAIL;
	}
	getstring( LA_PTR(1), buf, sizeof(buf));
	env = getenv(buf);
	if (!env)
	    break;
	RETSTR(env, strlen(env));

    default:
	break;
    }
    RETFAIL;
}

/* TEMP; */
#include <sys/utsname.h>

int
sysname(s)
    char *s;
{
    struct utsname u;
    extern const char vers[];
    extern const char snoname[];

    if (uname(&u) < 0)
	sprintf(s, "unknown:unix:%s %s",	/* XXX include vdate? */
		snoname, vers);
    else
	sprintf(s, "%s:%s %s:%s %s",	/* XXX include vdate? */
		u.machine, u.sysname, u.release, snoname, vers);

    return 1;
}
