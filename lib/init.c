/* $Id$ */

#include <stdio.h>			/* for usage! */
#include <signal.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "data.h"			/* SIL data */
#include "equ.h"			/* SIL equ's */

/* return type of signal handler functions */
#ifndef SIGFUNC_T
#define SIGFUNC_T void
#endif

extern char *dynamic();
extern char *malloc();

#define NDYNAMIC 25000			/* default dynamic region size */

int ndynamic = NDYNAMIC * DESCR;
int pmstack = SPDLDR;
int rflag;

extern int optind;
extern char *optarg;
extern int getopt();

char parambuf[512];
char *params;
char **argv;
int firstarg, argc;

void
p( str )
    char *str;
{
    fputs( str, stderr );
}

void
usage( jname )
    char *jname;
{
    p( "Usage: %s [options...] [files...] [parameters...]\n", jname );
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
    fprintf(stderr,
	    "-P BYTES[k]\n\tsize of pattern match stack in bytes (default: %d)\n",
	    SPDLDR);
    exit(1);
}

int
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

void
init_args( ac, av )
    int ac;
    char *av[];
{
    int errs;
    int c;
    int multifile;

    /* save in globals for HOST(), getparm(), init() */
    argc = ac;
    argv = av;

    errs = 0;
    multifile = 0;			/* SITBOL behavior */

#ifdef vms
    argc = getredirection(argc, argv);
#endif /* vms defined */

    /*
     * NOTE:
     *
     * * Options are compatible (where possible) with Catspaw Macro SPITBOL
     *
     * * When adding options, update usage() function (above) and man page!!!
     */

    while ((c = getopt(argc, argv, "bd:fhklnprsu:MP:")) != -1) {
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

    /* process any remaining items as arguments (if no -u option) */
    firstarg = optind;
    if (params == NULL && optind < argc) {
	while (optind < argc) {
	    if (parambuf[0])
		strcat(parambuf, " ");
	    strcat(parambuf, argv[optind++] );
	}
	params = parambuf;
    }

    if (errs) {
	usage(argv[0]);
    }

    io_init();				/* AFTER io_input calls! */
}

int math_error;				/* see macros.h */

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
    /* save sig in resident storage for use in message? */
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
}

/* 9/21/96 - set specifier to point to entire command line for &PARM */
int
getparm( sp )
    struct spec *sp;
{
    static char parm[2048];		/* XXX */
    int i;

    parm[0] = '\0';
    for (i = 0; i < argc; i++) {
	if (parm[0])
	    strcat(parm, " ");
	strcat(parm, argv[i] );
    }
    S_A(sp) = (int_t) parm;		/* OY! */
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(parm);
    CLR_S_UNUSED(sp);

    return 1;
}
