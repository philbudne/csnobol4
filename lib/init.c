/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>			/* for usage! */
#include <signal.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "equ.h"			/* SIL equ's */
#include "res.h"			/* for data.h */
#include "data.h"			/* SIL data */

/* return type of signal handler functions */
#ifndef SIGFUNC_T
#define SIGFUNC_T void
#endif /* SIGFUNC_T not defined */

extern char *dynamic();

#define NDYNAMIC (64*1024)		/* default dynamic region size */

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
static int ndynamic;
static int pmstack;

/* global for access by io.c; */
int rflag;

/* global for access by host.c; */
char *params;
char **argv;
int firstarg;
int argc;
#endif /* NO_STATIC_VARS not defined */

extern int optind;
extern char *optarg;
extern int getopt();

static void
p( str )
    char *str;
{
    fputs( str, stderr );
}

static void
usage( jname )
    char *jname;
{
    extern const char snoname[], vers[], vdate[];
    fprintf( stderr, "%s version %s (%s)\n", snoname, vers, vdate );
    fprintf( stderr,
	    "Usage: %s [options...] [files...] [parameters...]\n", jname );
/* XXX stuff about parameters */
    p( "-b\ttoggle display of startup banner\n");
    fprintf(stderr,
	    "-d BYTES[k]\n\tsize of dynamic region in bytes (default: %d)\n",
	    NDYNAMIC*DESCR);
    p( "-f\ttoggle folding of identifiers to upper case (-CASE)\n");
    p( "-h\tthis message\n");
    p( "-k\ttoggle running programs with compilation errors (-[NO]ERRORS)\n");
    p( "-l\tenable listings (-LIST)\n");
    p( "-n\ttoggle running program after compilation (-[NO]EXECUTE)\n");
    p( "-p\ttoggle SPITBOL operators (-PLUSOPS)\n");
    p( "-r\ttoggle reading INPUT from after END statement\n");
    p( "-s\ttoggle display of statistics\n");
    p( "-u PARMS\n\tparameter data available via HOST(0)\n");
    p( "-M\tprocess multiple input files\n");
    p( "-P BYTES[k]\n");
    fprintf(stderr, "\tsize of pattern match stack in bytes (default: %d)\n",
	    SPDLDR);
    p( "\n");
    fprintf(stderr, "in memory region sizes a suffix of 'k' (1024) can be used\n");
    fprintf(stderr, "descriptor size is %d bytes\n", DESCR );
    exit(1);
}

static int
getk( str, out )
    char *str;
    int *out;
{
    char k;
    switch (sscanf(str, "%d%c", out, &k)) {
    case 2:				/* number & suffix */
	if (k == 'k' || k == 'K')
	    *out *= 1024;
	else
	    return 0;			/* not "K"; fail */
	/* FALL */
    case 1:				/* just number */
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
getargs(start, sp)
    int start;				/* which argument to start on */
    struct spec *sp;			/* dest spec, or NULL */
{
    int i;
    int len;
    char *parms;
    register char *pp;

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

/* called from main.c after init_data, before xfer to SIL BEGIN label */
void
init_args( ac, av )
    int ac;
    char *av[];
{
    int errs;
    int c;
    int multifile;

    ndynamic = NDYNAMIC * DESCR;
    pmstack = SPDLDR;

    /* save in globals for HOST(), getparm(), init() */
    argc = ac;
    argv = av;

    errs = 0;
    multifile = 0;			/* SITBOL behavior */

#ifdef vms
    argc = getredirection(argc, argv);
#endif /* vms defined */


    /*
     * ***** NOTE ******
     *
     * * Options are compatible (where possible) with Catspaw Macro SPITBOL
     *
     * * When adding options, update usage() function (above) and man page!!!
     *
     * * '+' at start is required on RH7 Linux to avoid broken default behavior
     *		and is HOPEFULLY harmless (-+ if given should fall into default
     *		case.  If we ever want a real -+ option, ANOTHER + will need
     *		to be added).
     */

    while ((c = getopt(argc, argv, "+bd:fhklnprsu:MP:")) != -1) {
	switch (c) {
	case 'b':
	    D_A(BANRCL) = !D_A(BANRCL);	/* toggle banner output */
	    break;

	case 'd':			/* number of dynamic descrs */
	    if (!getk(optarg, &ndynamic))
		errs++;
	    /* XXX enforce a minimum?? */
	    break;

	case 'f':			/* toggle case folding */
	    D_A(CASECL) = !D_A(CASECL);
	    break;

	case 'h':			/* help */
	    errs++;
	    break;

	case 'k':
	    /* toggle running programs with compile errors */
	    D_A(NERRCL) = !D_A(NERRCL);
	    break;

	case 'l':			/* -LIST */
	    /* XXX should take an argument!!! */
	    D_A(LISTCL) = 1;
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
	    params = optarg;
	    break;

	case 'M':			/* multi-file input */
	    multifile = !multifile;
	    break;

	case 'P':			/* pattern match stack size */
	    if (!getk(optarg, &pmstack))
		errs++;
	    break;

	default:
	    errs++;
	}
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
	io_input( argv[optind] );
	optind++;
	if (!multifile)			/* not in multi-file mode? */
	    break;			/* break out */
    }

    /* if no -u option, process any remaining items as arguments for HOST(0) */
    if (params == NULL && optind < argc) {
	params = getargs(optind, NULL);
    }

    firstarg = optind;			/* save for HOST(3) */

    if (errs) {
	usage(argv[0]);
    }

    io_init();				/* AFTER io_input calls! */
}

#ifndef NO_STATIC_VARS
volatile int math_error;		/* see macros.h */
#endif /* NO_STATIC_VARS defined */

static SIGFUNC_T
math_catch(sig)
    int sig;
{
#ifdef SIGFPE
    signal(SIGFPE, math_catch);
#endif /* SIGFPE defined */
#ifdef SIGOVER
    signal(SIGOVER, math_catch);
#endif /* SIGOVER defined */

    math_error = TRUE;
    /* XXX need to longjump out on some systems to avoid restarting insn? */
}

static SIGFUNC_T
err_catch(sig)
    int sig;
{
    D_A(SIGNCL) = sig;			/* save signal number for output */
    SYSCUT();
}

/* called by SIL INIT macro (first SIL op executed) */
void
init()
{
    char *ptr;
    int len;

    /****************
     * allocate dynamic data region
     */

    /* round down to even number of descr's */
    ndynamic = (ndynamic / DESCR) * DESCR;

    ptr = dynamic(ndynamic);

    if (ptr == NULL) {
	fprintf( stderr, "%s: could not allocate dynamic region of %d bytes\n",
		argv[0], ndynamic);
	exit(1);
    }

    bzero( ptr, ndynamic );		/* XXX needed? */

    D_A(FRSGPT) = D_A(HDSGPT) = (int_t) ptr; /* first dynamic descr */

    /* first descr past end of dynamic storage */
    D_A(TLSGP1) = (int_t) ptr + ndynamic;


    /****************
     * allocate pattern match stack
     */

    /* round down to even number of descr's */
    pmstack = (pmstack / DESCR) * DESCR;

    ptr = malloc(pmstack);		/* NOTE: malloc(), not dynamic() */
    if (ptr == NULL) {
	fprintf( stderr, "%s: could not allocate pattern stack of %d bytes\n",
		argv[0], pmstack);
	exit(1);
    }

    /* set up stack title */
    D_A(ptr) = (int_t) ptr;
    D_F(ptr) = TTL + MARK;
    D_V(ptr) = pmstack;			/* length */

    /* pointers to top of stack */
    D_A(PDLPTR) = D_A(PDLHED) = (int_t) ptr;

    /* pointer to end of stack for overflow checks */
    D_A(PDLEND) = (int_t) ptr + pmstack - NODESZ;

    /****************
     * setup signal handlers
     */

    signal( SIGINT, err_catch );

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

    tty_save();
}

/* 9/21/96 - set specifier to point to entire command line for &PARM */
int
getparm( sp )
    struct spec *sp;
{
    return getargs(0, sp) != NULL;
}
