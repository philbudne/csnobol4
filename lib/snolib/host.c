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

#ifdef HAVE_BUILD_VARS
extern const char build_files[];
extern const char build_lib[];
extern const char build_date[];
extern const char build_dir[];
#endif /* HAVE_BUILD_VARS */

#ifndef BPC
#define BPC 8				/* 8 bits/char */
#endif /* BPC not defined */

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
	if (nargs >= 2 && LA_TYPE(1) == I) {
	    n = LA_INT(1);
	    if (n >= 0 && n < argc)
		RETSTR(argv[n]);	/* return n'th command line arg */
	}
	/* bad argument type, or out of range */
	break;

    case 3:				/* HOST(3); first unused argument */
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(firstarg);

    case 4:				/* HOST(4,s); environment var s */
	if (nargs >= 2 && LA_TYPE(1) == S) {
	    getstring(LA_PTR(1), buf, sizeof(buf));
	    env = getenv(buf);
	    if (env)
		RETSTR(env);
	}
	/* fail if bad argument type, or no such variable */
	break;


/****************
 * CSNOBOL4 extensions
 * mostly useful for diagnosing build problems
 */

/* configuration variables, from config.h (written by autoconf script) */
#ifdef CONFIG_GUESS
    case 2000:
	RETSTR(CONFIG_GUESS);		/* build architecture */
#endif /* CONFIG_GUESS defined */

#ifdef CONFIG_HOST
    case 2001:
	RETSTR(CONFIG_HOST);		/* host where autoconf was run */
#endif /* CONFIG_HOST defined */

#ifdef CONFIG_DATE
    case 2002:
	RETSTR(CONFIG_DATE);		/* date autoconf was run */
#endif /* CONFIG_DATE defined */

#ifdef CONFIG_OPTIONS
    case 2003:
	RETSTR(CONFIG_OPTIONS);		/* autoconf options */
#endif /* CONFIG_OPTIONS defined */

/* variables from build.c, created before linking snobol4 executable */
#ifdef HAVE_BUILD_VARS
    case 2100:
	RETSTR(build_date);

    case 2101:
	RETSTR(build_dir);

    case 2102:
	RETSTR(build_files);
#endif /* HAVE_BUILD_VARS defined */

/* defines, from Makefile */
#ifdef SNOLIB_DIR
    case 2200:
	RETSTR(SNOLIB_DIR);
#endif /* SNOLIB_DIR defined */

#ifdef SNOLIB_FILE
    case 2202:
	RETSTR(SNOLIB_FILE);
#endif /* SNOLIB_FILE defined */

/* integer constants; */
    case 2300:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(sizeof(INT_T)*BPC);	/* INTEGER size */
    case 2301:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(sizeof(REAL_T)*BPC);	/* REAL size */
    case 2302:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(sizeof(void *)*BPC);	/* pointer size */
    case 2303:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(sizeof(long)*BPC);	/* long size */
    case 2304:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(DESCR*BPC);		/* descriptor size */
    case 2305:
	RETTYPE = I;			/* oof! blast return type! */
	RETINT(SPEC*BPC);		/* specifier size */

/*
 * NOTE!! All of the above 2xxx values are related to internals, and
 * the build environment.  Perhaps it should be kept that way, and
 * other values added in a different range?
 */

    default:
	break;
    }
    RETFAIL;
}
