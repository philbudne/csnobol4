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
 *
 * While it might be tempting to add new functions here
 * (since it's already built in), consider putting your new functions
 * in a loadable module-- see the dynamic.sno -INCLUDE file for functions
 * to help you do this.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* getenv(),system(),free() */
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
#include "globals.h"
#include "version.h"			/* needed under DJGPP */

pmlret_t
HOST( LA_ALIST ) {
    char buf[1024];			/* XXX */
    char *str;
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
	str = mgetstring(LA_PTR(1));	/* get arg as c-string */
	io_flushall(0);			/* flush output buffers */
	n = system(str);		/* run in sub-shell */
	free(str);
	RETINT(n);

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
	    char *env;
	    str = mgetstring(LA_PTR(1));
	    env = getenv(str);
	    free(str);
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

    case HOST_VERSION_DATE:
	RETSTR(vdate);			/* from configure script */

#ifdef CC_IS
    case HOST_CC_IS:
	RETSTR(CC_IS);			/* from configure script */
#endif /* CC_IS defined */

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
/* (created before config.sno existed) */
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

#ifdef DIR_SEP
    case HOST_DIR_SEP:
	RETSTR(DIR_SEP);
#endif /* DIR_SEP defined */

#ifdef PATH_SEP
    case HOST_PATH_SEP:
	RETSTR(PATH_SEP);
#endif /* PATH_SEP defined */

#ifdef DEF_SNOPATH
    case HOST_DEF_SNOPATH:
	RETSTR(DEF_SNOPATH);
#endif /* DEF_SNOPATH defined */

#ifdef INCLUDE_DIR
    case HOST_INCLUDE_DIR:
	RETSTR(INCLUDE_DIR);
#endif /* INCLUDE_DIR defined */

#ifdef OBJECT_EXT
    case HOST_OBJ_EXT:
	RETSTR(OBJECT_EXT);
#endif /* OBJECT_EXT defined */

#ifdef SETUP_SYS
    case HOST_SETUP_SYS:
	RETSTR(SETUP_SYS);
#endif /* SETUP_SYS defined */

#ifdef SHARED_OBJ_SUBDIR
    case HOST_SHARED_OBJ_SUBDIR:
	RETSTR(SHARED_OBJ_SUBDIR);
#endif /* SHARED_OBJ_SUBDIR defined */

#ifdef CONFIG_CFLAGS
    case HOST_CONFIG_CFLAGS:
	RETSTR(CONFIG_CFLAGS);
#endif /* CONFIG_CFLAGS defined */

#ifdef CONFIG_CPPFLAGS
    case HOST_CONFIG_CPPFLAGS:
	RETSTR(CONFIG_CPPFLAGS);
#endif /* CONFIG_CPPFLAGS defined */

#ifdef CONFIG_LDFLAGS
    case HOST_CONFIG_LDFLAGS:
	RETSTR(CONFIG_LDFLAGS);
#endif /* CONFIG_LDFLAGS defined */

/****************
 * integer constants;
 */
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

/* string variables; */

    case HOST_SNOLIB_BASE:
	RETSTR(snolib_base);		/* library base directory in use */

    case HOST_SNOLIB_LOCAL:
	RETSTR(snolib_local);		/* local, version-independant files */

    case HOST_SNOLIB_VLIB:
	RETSTR(snolib_vlib);		/* distribution (version-specific) */

    case HOST_SNOLIB_VLOCAL:
	RETSTR(snolib_vlocal);		/* local, version-specific files */

    case HOST_SNOPATH_DIR:
	if (nargs >= 2 && LA_TYPE(1) == I) {
	    char *val = io_lib_dir(LA_INT(1)); /* n'th elt of search path */
	    if (val)
		RETSTR(val);
	}
	/* bad argument type, or out of range */
	break;

    case HOST_SNOLIB_VERS:
	RETSTR(snolib_vers);		/* versioned base directory */

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
