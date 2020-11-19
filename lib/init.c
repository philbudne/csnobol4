/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for malloc */
#include <stdio.h>
#include <signal.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* xxx_FILENO */
#endif /* HAVE_UNISTD_H defined */

#ifdef HAVE_IO_H
#include <io.h>				/* _dup */
#endif /* HAVE_IO_H */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"			/* io_input() protos */
#include "str.h"			/* strlen() */
#include "units.h"			/* UNIT[IOPT] */

#include "equ.h"			/* {PS,IS,ST}SIZE NODESZ SPDLSZ */
#include "res.h"			/* UC SIL var names */
#include "data.h"			/* res */
#include "proc.h"			/* for SYSCUT() */
#include "load.h"			/* io_fname  */
#include "snobol4.h"	      /* INIT_OK, io_input_file, io_mkfile(_noclose) */
#include "globals.h"

#include "version.h"

/* return type of signal handler functions */
#ifndef SIGFUNC_T
#define SIGFUNC_T void
#endif /* SIGFUNC_T not defined */

#ifndef SIGFUNC_ARG
#define SIGFUNC_ARG int sig
#endif /* SIGFUNC_ARG not defined */

#ifndef DISCARD_SIGFUNC_ARG
#define DISCARD_SIGFUNC_ARG (void)sig
#endif /* SIGFUNC_ARG not defined */

#ifndef NDYNAMIC
#define NDYNAMIC (512*1024)		/* default dynamic region size */
#endif /* NDYNAMIC not defined */

#ifndef PSSIZE
#define PSSIZE SPDLSZ			/* default pattern stack size */
#endif /* PSSIZE not defined */

#ifndef ISSIZE
#define ISSIZE STSIZE
#endif /* ISSIZE not defined */

#ifdef HAVE_BUILD_VARS
extern const char build_date[];		/* from build.c */
#endif /* HAVE_BUILD_VARS defined */

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif /* STDOUT_FILENO not defined */

#ifdef NEED_GETOPT_EXTERNS
/* needed w/ own getopt & on SunOS4?! */
extern int optind;
extern char *optarg;
#endif

static VAR int xflag;
static VAR void *ppmstack;
static VAR FILE *termin;

static void init_signals(void);

static void
p(int flag, const char *str) {
    fprintf(stderr, "-%c\t%s\n", flag, str);
}

static char *
showk(int n) {
    static VAR char buf[32];		/* XXX VAR shouldn't matter? */
    int k, m;

    k = n / 1024;
    m = k / 1024;

    if (m*1024*1024 == n)
	sprintf(buf, "%dm", m);
    else if (k*1024 == n)
	sprintf(buf, "%dk", k);
    else
	sprintf(buf, "%d", n);

    return buf;
}

static int
usage(char *jname, int justversion) {
    extern const char snoname[], vers[], vdate[];
    fprintf( stderr, "%s version %s (%s)\n", snoname, vers, vdate );
#ifdef HAVE_BUILD_VARS
    fprintf( stderr, "built %s\n", build_date);
#endif /* HAVE_BUILD_VARS defined */
    if (justversion)
	return 0;

    fprintf( stderr,
	    "Usage: %s [options...] [files...] [parameters...]\n", jname );
/* XXX stuff about parameters */
    p('b',"disable display of startup banner");
    fprintf(stderr,
	    "-d DESCRS[km]\n\tsize of dynamic region in descriptors (default: %s)\n", showk(NDYNAMIC));
    p('f',"toggle folding of identifiers to upper case (-CASE)");
    p('g',"enable GC trace (&GTRACE)");
    p('h',"help (this message)");
    p('k',"toggle running programs with compilation errors (-[NO]ERRORS)");
    fprintf(stderr, 
	    "-l LISTINGFILE\n\tenable listing (-LIST) and specify file\n");
    p('n',"toggle running program after compilation (-[NO]EXECUTE)");
    p('p',"toggle SPITBOL operators (-PLUSOPS)");
    p('r',"toggle reading INPUT from after END statement");
    p('s',"toggle display of statistics");
    fprintf(stderr, "-u PARMS\n\tparameter data available via HOST(0)\n");
    p('v',"display version and exit");
    p('x',"force display of startup banner");
#ifdef BLOCKS
    p('B',"toggle SNOBOL4B operators (-[NO]BLOCKS)");
#endif
    fprintf(stderr, "-I DIR\tadd directory to search path\n");
    fprintf(stderr, "-L SOURCE\n");
    fprintf(stderr, "\tload source file before user program\n");
    p('M',"process multiple files for program code");
    p('N',"clear search path");
    fprintf(stderr, "-P DESCRS[km]\n");
    fprintf(stderr, "\tsize of pattern match stack in descriptors (default: %s)\n", showk(PSSIZE));
    fprintf(stderr, "-S DESCRS[km]\n");
    fprintf(stderr, "\tsize of interpreter stack in descriptors (default: %s)\n", showk(ISSIZE));
    fprintf(stderr, "-U\tmake all stdio I/O unbuffered\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "For memory region sizes a suffix of 'k' (1024) and 'm' (1024*1024)\n");
    fprintf(stderr, "can be used. A descriptor takes up %d bytes.\n", (int)DESCR );
    return 1;
}

static int
getk(char *str, size_t *out) {
    char suff;
    unsigned long val;
    switch (sscanf(str, "%lu%c", &val, &suff)) {
    case 2:				/* number & suffix */
	if (suff == 'k' || suff == 'K')
	    val *= 1024;
	else if (suff == 'm' || suff == 'M')
	    val *= 1024*1024;
	else
	    return 0;			/* bad suffix; fail */
	/* FALL */
    case 1:				/* just number */
	*out = val;
	return 1;			/* return OK */

    default:				/* no number */
	return 0;			/* fail */
    }
}

/*
 * create c-string of args from start .. argc in a malloc'ed buffer.
 * fills in an optional SPEC
 */

static char *
getargs(int start,			/* which argument to start on */
	struct spec *sp) {		/* dest spec, or NULL */
    int i;
    char *parms;
    register char *pp;
    int len;

    len = 0;
    for (i = start; i < argc; i++)
	len += strlen(argv[i]) + 1;	/* add one for space or NUL */

    parms = malloc(len);
    if (!parms)
	return NULL;			/* XXX perror & exit? */

    pp = parms;
    for (i = start; i < argc; i++) {
	register char *ap;

	if (pp != parms)
	    *pp++ = ' ';

	ap = argv[i];
	while ((*pp = *ap++))
	    pp++;
    }

    if (sp) {
	S_A(sp) = (int_t) parms;	/* OY! */
	S_F(sp) = 0;			/* NOTE: *not* a PTR! */
	S_V(sp) = 0;
	S_O(sp) = 0;
	S_L(sp) = len - 1;		/* omit trailing NUL */
	CLR_S_UNUSED(sp);
    }

    return parms;
}

/* return zero on failure */
static int
io_init(int interactive) {		/* here from init_args() */
    io_initvars();

    if (!interactive)
	return 1;

    init_signals();

    if (nfiles == 0) {			/* no input file(s)? */
	/* note: io_mkfile blows away preload list */
	io_input_file("-");		/* implicit "-"! */
    }
    else {
	if (!io_skip(UNITI)) {		/* force file open */
	    const char *fname;
	    fname = io_fname(UNITI);
	    if (!fname)
		fname = "unknown input file";
	    perror(fname);
	    return 0;
	}
    }

    /* XXX support -o outputfile? */

    if (!D_A(LISTCL) && !io_mkfile_noclose(UNITO, stdout, STDOUT_NAME)) {
	perror("could not attach stdout to OUTPUT");
	return 0;
    }

    if (!io_mkfile_noclose(UNITP, stderr, STDERR_NAME)) {
	perror("could not attach stderr to TERMINAL");
	return 0;
    }

    termin = term_input();		/* call system dependant function */
    if (termin && !io_mkfile_noclose(UNITT, termin, TERMIN_NAME)) {
	perror("could not open TERMINAL for input");
	return 0;
    }
    return 1;
} /* io_init */

/* called after -I paths have already been added, before preload, sources */
static void
pathinit(char *av0, int std_includes) {
    char *env;

    /* XXX SHARED FIX: some of this only needs to be done once */

    /* generate directory names for HOST() even if not in search path */

    /* /usr/local/lib/snobol4 or C:\Program Files\SNOBOL4 */
    snolib_base = getenv("SNOLIB");	/* old variable */
    if (!snolib_base)
	snolib_base = SNOLIB_BASE;

    /* versioned directory */
    /* /usr/local/lib/snobol4/x.y or C:\Program Files\SNOBOL4\x.y */
    snolib_vers = strjoin(snolib_base, DIR_SEP, VERSION, NULL);

    /* distribution files (version-specific) */
    snolib_vlib = strjoin(snolib_vers, DIR_SEP, "lib", NULL);

#ifdef HAVE_FIND_SNOLIB_DIRECTORY
    /* see if directories exist, if not, find via argv[0] */
    find_snolib_directory(av0, &snolib_vers, &snolib_vlib);
#else
    (void) av0;
#endif

    /* local, version-specific */
    snolib_vlocal = strjoin(snolib_vers, DIR_SEP, "local", NULL);

    /* local -- all versions */
    snolib_local = strjoin(snolib_base, DIR_SEP, "local", NULL);

    /* add to search path (after . & -I options) */
    env = getenv("SNOPATH");
    if (env) {
	io_add_lib_path(env);
    }
    else if (std_includes) {
	io_add_lib_dir(snolib_vlib);	/* dist, (version-specific) */
	io_add_lib_dir(snolib_vlocal);	/* local, version-specific */
	io_add_lib_dir(snolib_local);	/* local -- all versions */
	io_add_lib_dir(snolib_base);	/* old directory */
    }
#ifdef EXTRA_SNOPATH
    if (std_includes)
	io_add_lib_path(EXTRA_SNOPATH);
#endif
}

/* called from main.c after init_data, before xfer to SIL BEGIN label */
/* returns exit status, or INIT_OK on success */
int
init_args(int ac, char *av[], int interactive) {
    int errs;
    int c;
    int multifile;
    int justversion;
    int showpaths = 0;
    int std_includes = 1;

    /* in globals.h */
    rflag = lflag = unbuffer_all = nfiles = firstarg = 0;
    ndynamic = NDYNAMIC;
    pmstack = PSSIZE;
    istack = ISSIZE;

    /* save in globals for HOST(), getparm(), init() */
    argc = ac;
    argv = av;
    /* end in globals.h */

#ifdef vms
    argc = getredirection(argc, argv);
#endif /* vms defined */

    errs = 0;
    multifile = 0;			/* SITBOL behavior */
    justversion = 0;
    io_add_lib_dir(".");		/* XXX but not if set-uid??? */

    /*
     * ***** NOTE ******
     *
     * * Options are compatible (where possible) with Catspaw Macro SPITBOL
     *
     * * When adding options, update usage() function (above) and man page!!!
     *
     * * '+' at start is required w/ GNU libc (Linux) to avoid broken
     *		default (non POSIX) behavior (continues picking up
     *		switches after first file) and is HOPEFULLY harmless
     *		(-+ if given should fall into default case.  If we
     *		ever want a real -+ option, ANOTHER + will need to be
     *		added, but it better not want an argument!)
     */

    /* getopt not thread safe!!! use private vers that takes optind/optarg? */
    while ((c = getopt(argc, argv, "+bd:fghkl:nprsu:vxzBI:L:MNP:S:U")) != -1) {
	switch (c) {
	case 'b':
	    D_A(BANRCL) = 0;		/* disable banner output */
	    break;

	case 'd':			/* number of dynamic descrs */
	    if (!getk(optarg, &ndynamic))
		errs++;
	    /* XXX enforce a minimum?? */
	    break;

	case 'f':			/* toggle case folding */
	    D_A(CASECL) = !D_A(CASECL);
	    break;

	case 'g':
	    D_A(GCTRCL) = -1;		/* enable &GCTRACE */
	    break;

	case 'h':			/* help */
	    justversion = 0;
	    errs++;
	    break;

	case 'k':
	    /* toggle running programs with compile errors */
	    D_A(NERRCL) = !D_A(NERRCL);
	    break;

	case 'l':			/* -LIST */
	    {
		/* now takes an argument!!! */
		FILE *listfile;
		if (strcmp(optarg, "-") == 0)
		    listfile = fdopen(dup(STDOUT_FILENO), "w");
		else
		    listfile = fopen(optarg, "w");
		D_A(LISTCL) = lflag = 1;
		if (!listfile || !io_mkfile(UNITO, listfile, optarg)) {
		    perror(optarg);
		    return 1;
		}
	    }
	    break;

	case 'n':			/* toggle -[NO]EXECUTE */
	    D_A(EXECCL) = !D_A(EXECCL);
	    break;

	case 'p':			/* toggle -PLUSOPS */
	    D_A(SPITCL) = !D_A(SPITCL);
	    break;

	case 'r':			/* read INPUT from source after END */
	    rflag = !rflag;
	    break;

	case 's':			/* toggle statistics */
	    D_A(STATCL) = !D_A(STATCL);
	    break;

	case 'u':			/* parameter data */
	    params = strdup(optarg);
	    break;

	case 'v':			/* version */
	    justversion = 1;
	    errs++;
	    break;

	case 'x':
	    xflag = D_A(BANRCL) = 1;	/* force banner output */
	    break;

	case 'z':
	    showpaths = 1;
	    break;

#ifdef BLOCKS
	case 'B':			/* toggle BLOCKS */
	    D_A(BLOKCL) = !D_A(BLOKCL);
	    break;
#endif

	case 'I':			/* include path dir */
	    io_add_lib_dir(optarg);
	    break;

	case 'L':			/* pre-load files */
	    if (exists(optarg)) 
		io_input_file(optarg);
	    else {
		/* look for file on SNOPATH (BUG: not yet populated?) */
		/* XXX check for no DIR_SEP? */
		char *path = io_lib_find(NULL, optarg, ".inc");
		if (path)
		    io_input_file(path);
		else {
		    fprintf(stderr, "%s: file not found\n", optarg);
		    return 1;
		}
	    }
	    break;

	case 'M':			/* multi-file input */
	    multifile = !multifile;
	    break;

	case 'N':			/* no standard includes */
	    std_includes = 0;
	    break;

	case 'P':			/* pattern match stack size */
	    if (!getk(optarg, &pmstack))
		errs++;
	    break;

	case 'S':			/* interpreter stack size */
	    if (!getk(optarg, &istack))
		errs++;
	    break;

	case 'U':			/* make all stdio I/O unbuffered */
	    unbuffer_all = 1;
	    break;

	default:
	    errs++;
	}
    } /* while getopt */

    pathinit(av[0], std_includes);

    /* XXX option to disable? */
    io_preload();

    if (!unbuffer_all) {
	char *u = getenv("SNOBOL4UNBUFFERED"); /* like PYTHONUNBUFFERED */
	if (u && *u)
	    unbuffer_all = 1;
    }

    /*
     * append first file (or all additional args until "--" seen
     * in "multi-file" mode) to INPUT stream
     */
    while (optind < argc) {
	if (strcmp(argv[optind], "--") == 0) { /* terminator? */
	    optind++;			/* skip it */
	    break;			/* leave loop */
	}
	io_input_file( argv[optind] );
	if (!xflag)
	    D_A(BANRCL) = 0;		/* disable banner output */
	optind++;
	nfiles++;
	if (!multifile)			/* not in multi-file mode? */
	    break;			/* break out */
    }

    /* if no -u option, process any remaining items as arguments for HOST(0) */
    if (params == NULL && optind < argc) {
	params = getargs(optind, NULL);
    }

    firstarg = optind;			/* save for HOST(3) */

    if (errs)
	return usage(argv[0], justversion);

    if (!io_init(interactive))		/* AFTER io_input calls! */
	return 1;

    if (showpaths) {			/* after io_init() */
	io_show_paths();
	return 0;
    }

#ifdef HAVE_OS_INIT
    os_init();
#endif /* HAVE_OS_INIT defined */

    return INIT_OK;
}

volatile VAR int math_error;		/* see macros.h */

static SIGFUNC_T
math_catch(SIGFUNC_ARG) {
#ifdef SIGFPE
    signal(SIGFPE, math_catch);
#endif /* SIGFPE defined */
#ifdef SIGOVER
    signal(SIGOVER, math_catch);
#endif /* SIGOVER defined */
    DISCARD_SIGFUNC_ARG;

    math_error = TRUE;
    /* XXX need to longjump out on some systems to avoid restarting insn? */
}

/* handle fatal errors */
static SIGFUNC_T
err_catch(SIGFUNC_ARG) {
    D_A(SIGNCL) = sig;			/* save signal number for output */
    SYSCUT(NORET);
}

/* handle ^C (SIGINT) here */
static SIGFUNC_T
sig_catch(SIGFUNC_ARG) {
    if (D_A(COMPCL) || D_A(ERRLCL) == 0) /* in compiler or &ERRLIMIT == 0*/
	err_catch(sig);			/* treat as before */

    /*
     * top of INIT checks UINTCL and causes "User Interrupt" error
     * which can be caught with SETEXIT() and continued with :(SCONTINUE)
     * Keep count, so windoze code in io.c can detect ^C vs EOF
     */
    D_A(UINTCL)++;
    signal( SIGINT, sig_catch );	/* re-arm for Win32/VC10 */
}

#ifdef SIGTSTP
static SIGFUNC_T
suspend(SIGFUNC_ARG) {
    DISCARD_SIGFUNC_ARG;

    tty_suspend();			/* restore tty mode(s) */

    /* reestablish handler (does any system with job control,
     * in case signal() has System V semantics
     */
    signal(SIGTSTP, suspend);

    /* no need to restore tty modes; next I/O will reset as needed */
}
#endif /* SIGTSTP defined */

/* called by SIL INIT macro (first SIL op executed) */
void
init(void) {
    char *ptr;

    /****************
     * allocate dynamic data region
     */

    ndynamic *= DESCR;			/* get bytes */

    ptr = dynamic(ndynamic);

    if (ptr == NULL) {
	fprintf(stderr, "%s: could not allocate dynamic region of %lu bytes\n",
		argv[0], (unsigned long)ndynamic);
	exit(1);
    }

    bzero( ptr, ndynamic );		/* XXX needed? */

    D_A(FRSGPT) = D_A(HDSGPT) = (int_t) ptr; /* first dynamic descr */

    /* first descr past end of dynamic storage */
    D_A(TLSGP1) = (int_t) ptr + ndynamic;


    /****************
     * allocate pattern match stack
     */

    pmstack *= DESCR;			/* get bytes */

    ptr = ppmstack = malloc(pmstack);	/* NOTE: malloc(), not dynamic() */
    if (ptr == NULL) {
	fprintf(stderr, "%s: could not allocate pattern stack of %lu bytes\n",
		argv[0], (unsigned long)pmstack);
	exit(1);
    }

    /* set up stack title */
    D_A(ptr) = (int_t) ptr;
    D_F(ptr) = TTL + MARK;
    D_V(ptr) = pmstack;			/* length in bytes */

    /* pointers to top of stack */
    D_A(PDLPTR) = D_A(PDLHED) = (int_t) ptr;

    /* pointer to end of stack for overflow checks */
    D_A(PDLEND) = (int_t) ptr + pmstack - NODESZ;

    /****************
     * allocate interpreter stack
     */

    istack *= DESCR;			/* get bytes */

    ptr = malloc(istack);		/* NOTE: malloc(), not dynamic() */
    if (ptr == NULL) {
	fprintf(stderr, "%s: could not allocate interpreter stack of %lu bytes\n",
		argv[0], (unsigned long) istack);
	exit(1);
    }

    /* set up stack title */
    D_A(ptr) = (int_t) ptr;
    D_F(ptr) = TTL + MARK;
    D_V(ptr) = istack;			/* length in bytes */

    /* pointers to top of stack */
    D_A(STKPTR) = D_A(STKHED) = (int_t) ptr;

    /* pointer to end of stack, for overflow checks */
    D_A(STKEND) = (int_t) ptr + istack;
}

static void
init_signals(void) {
    /*
     * setup signal handlers
     */

    signal( SIGINT, sig_catch );

    /* catch bad memory references */
    signal( SIGSEGV, err_catch );
#ifdef SIGBUS
    signal( SIGBUS, err_catch );
#endif /* SIGBUS defined */

    /* catch math errors */
#ifdef SIGFPE
    signal(SIGFPE, math_catch);
#endif /* SIGFPE defined */
#ifdef SIGOVER
    signal(SIGOVER, math_catch);
#endif /* SIGOVER defined */

    /* catch resource limit errors */
#ifdef SIGXCPU
    signal(SIGXCPU, err_catch);
#endif /* SIGXCPU defined */
#ifdef SIGXFSZ
    signal(SIGXFSZ, err_catch);
#endif /* SIGXFSZ defined */

    /* catch network errors! */
#ifdef SIGPIPE
    signal(SIGPIPE, err_catch);
#endif /* SIGPIPE defined */

    /* catch suspend */
#ifdef SIGTSTP
    signal(SIGTSTP, suspend);
#endif /* SIGTSTP defined */
} /* init_signals */

/* 9/21/96 - set specifier to point to entire command line for &PARM */
int
getparm(struct spec *sp) {
    return getargs(0, sp) != NULL;
}

/* 10/14/2020! -- free memory allocated in getparm */
int
freeparm(struct spec *sp) {
    if (S_A(sp))
	free((void *)S_A(sp));
    return 0;
}

#ifdef SHARED
struct cleanup {
    struct cleanup *next;
    void (*func)(void);
};

static TLS struct cleanup *cleanups;
#endif

/* void function taking pointer to void function with no args */
void
reg_cleanup(void (*func)(void)) {
#ifdef SHARED
    struct cleanup *cp = malloc(sizeof(struct cleanup));
    cp->next = cleanups;
    cp->func = func;
    cleanups = cp;
#else
    (void) func;
#endif
}

#ifdef SHARED
/* cleanup for shared library */
void
cleanup(void) {
    struct cleanup *cp, *next;

    if (params)
	free(params);

    for (cp = cleanups; cp; cp = next) {
	next = cp->next;
	if (cp->func)
	    (cp->func)();
	free(cp);
    }
    cleanups = NULL;

    free((void *)D_A(HDSGPT));		/* free dynamic region */
    free((void *)D_A(STKHED));		/* stack */
    free(ppmstack);			/* pattern match stack */

    free(snolib_local);
    free(snolib_vers);
    free(snolib_vlib);
    free(snolib_vlocal);

    if (termin)
	fclose(termin);			/* last, in case. should be a dup */
}
#endif
