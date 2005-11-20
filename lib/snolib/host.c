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

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* getenv(),system() */
#else  /* HAVE_STDLIB_H not defined */
extern char *getenv();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>			/* for lib.h, sprintf() */
#include <ctype.h>			/* isdigit() */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */
#include "host.h"			/* HOST() function codes */
#include "lib.h"			/* osname(),hwname(),io_flushall() */
#include "str.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
extern int argc, firstarg;
extern int pmstack, ndynamic, istack;
extern char **argv;
extern char *params;
#endif /* NO_STATIC_VARS not defined */

#ifdef HAVE_BUILD_VARS
extern const char build_files[];
extern const char build_lib[];
extern const char build_date[];
extern const char build_dir[];
#endif /* HAVE_BUILD_VARS defined */

#ifndef BPC
#define BPC 8				/* 8 bits/char */
#endif /* BPC not defined */

int
HOST( LA_ALIST ) LA_DCL
{
    char buf[512];			/* XXX */
    char *env;
    int_t n;

    switch (LA_TYPE(0)) {
    case S:				/* STRING */
	/*
	 * handle arbitrary strings!!
	 */
	getstring(LA_PTR(0), buf, sizeof(buf));

	/* XXX use strcasecmp (not universal)
	 * HOST_SYSINFO is null string so it's moot at the moment
	 */
	if (strcmp(buf, HOST_SYSINFO) == 0) {
	    char os[256], hw[256];
	    extern const char snoname[], vers[];

	    /* use routines from SYSDEP/sys.c; */
	    osname(os);
	    hwname(hw);

	    sprintf(buf, "%s:%s:%s %s", hw, os, snoname, vers);
	    RETSTR(buf);
	}
	else {
	    char *cp = buf;

	    /* try to convert to int */
	    n = 0;
	    while(*cp && isdigit((unsigned char)*cp))
		n = n*10 + *cp++ - '0';
	    if (*cp)
		RETFAIL;
	}
	break;
    case I:				/* INTEGER */
	n = LA_INT(0);
	break;
    case R:				/* REAL */
	n = (int_t)LA_REAL(0);
	break;
    default:
	RETFAIL;
    }

    switch (n) {
    case HOST_PARAMS:			/* HOST(0); all parameters (or -u) */
	RETSTR(params);

    case HOST_SYSCMD:			/* HOST(1,s); run subprocess */
	if (nargs < 2 || LA_TYPE(1) != S) {
	    /* GNAT programs expect "HOST(1)" to return zero; */
	    RETINT(0);
	}
	getstring(LA_PTR(1), buf, sizeof(buf)); /* get arg as c-string */
	io_flushall(0);			/* flush output buffers */
	RETINT(system(buf));		/* run in sub-shell */

    case HOST_ARGN:			/* HOST(2,n); argument n */
	if (nargs >= 2 && LA_TYPE(1) == I) {
	    n = LA_INT(1);
	    if (n >= 0 && n < argc)
		RETSTR(argv[n]);	/* return n'th command line arg */
	}
	/* bad argument type, or out of range */
	break;

    case HOST_FIRSTARG:			/* HOST(3); first unused argument */
	RETINT(firstarg);

    case HOST_GETENV:			/* HOST(4,s); environment var s */
	if (nargs >= 2 && LA_TYPE(1) == S) {
	    getstring(LA_PTR(1), buf, sizeof(buf));
	    env = getenv(buf);
	    /* RETSTR handles NULL as empty string, we want to RETFAIL */
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
    case HOST_CONFIG_GUESS:
	RETSTR(CONFIG_GUESS);		/* build architecture */
#endif /* CONFIG_GUESS defined */

#ifdef CONFIG_HOST
    case HOST_CONFIG_HOST:
	RETSTR(CONFIG_HOST);		/* host where autoconf was run */
#endif /* CONFIG_HOST defined */

#ifdef CONFIG_DATE
    case HOST_CONFIG_DATE:
	RETSTR(CONFIG_DATE);		/* date autoconf was run */
#endif /* CONFIG_DATE defined */

#ifdef CONFIG_OPTIONS
    case HOST_CONFIG_OPTIONS:
	RETSTR(CONFIG_OPTIONS);		/* autoconf options */
#endif /* CONFIG_OPTIONS defined */

/* variables from build.c, created before linking snobol4 executable */
#ifdef HAVE_BUILD_VARS
    case HOST_BUILD_DATE:
	RETSTR(build_date);

    case HOST_BUILD_DIR:
	RETSTR(build_dir);

    case HOST_BUILD_FILES:
	RETSTR(build_files);
#endif /* HAVE_BUILD_VARS defined */

/* defines, from Makefile */
#ifdef SNOLIB_DIR
    case HOST_SNOLIB_DIR:
	RETSTR(SNOLIB_DIR);
#endif /* SNOLIB_DIR defined */

#ifdef SNOLIB_FILE
    case HOST_SNOLIB_FILE:
	RETSTR(SNOLIB_FILE);
#endif /* SNOLIB_FILE defined */

#ifdef CC
    case HOST_CC:
	RETSTR(CC);
#endif /* HOST_CC defined */

#ifdef COPT
    case HOST_COPT:
	RETSTR(COPT);
#endif /* HOST_COPT defined */

#ifdef SO_EXT
    case HOST_SO_EXT:
	RETSTR(SO_EXT);
#endif /* SO_EXT defined */

#ifdef SO_CFLAGS
    case HOST_SO_CFLAGS:
	RETSTR(SO_CFLAGS);
#endif /* SO_CFLAGS defined */

#ifdef SO_LD
    case HOST_SO_LD:
	RETSTR(SO_LD);
#endif /* SO_LD defined */

#ifdef SO_LDFLAGS
    case HOST_SO_LDFLAGS:
	RETSTR(SO_LDFLAGS);
#endif /* SO_LDFLAGS defined */

#ifdef DL_EXT
    case HOST_DL_EXT:
	RETSTR(DL_EXT);
#endif /* DL_EXT defined */

#ifdef DL_CFLAGS
    case HOST_DL_CFLAGS:
	RETSTR(DL_CFLAGS);
#endif /* DL_CFLAGS defined */

#ifdef DL_LD
    case HOST_DL_LD:
	RETSTR(DL_LD);
#endif /* DL_LD defined */

#ifdef DL_LDFLAGS
    case HOST_DL_LDFLAGS:
	RETSTR(DL_LDFLAGS);
#endif /* DL_LDFLAGS defined */

/* integer constants; */
    case HOST_INTEGER_BITS:
	RETINT(sizeof(INT_T)*BPC);	/* INTEGER size */
    case HOST_REAL_BITS:
	RETINT(sizeof(REAL_T)*BPC);	/* REAL size */
    case HOST_POINTER_BITS:
	RETINT(sizeof(void *)*BPC);	/* pointer size */
    case HOST_LONG_BITS:
	RETINT(sizeof(long)*BPC);	/* long size */
    case HOST_DESCR_BITS:
	RETINT(DESCR*BPC);		/* descriptor size */
    case HOST_SPEC_BITS:
	RETINT(SPEC*BPC);		/* specifier size */
    case HOST_CHAR_BITS:
	RETINT(sizeof(char)*BPC);	/* char size */

/* integer variables; */
    case HOST_DYNAMIC_SIZE:
	RETINT(ndynamic/DESCR);		/* dynamic region in DESCRs */
    case HOST_PMSTACK_SIZE:
	RETINT(pmstack/DESCR);		/* pattern match stack length */
    case HOST_ISTACK_SIZE:
	RETINT(istack/DESCR);		/* interpreter stack length */

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
