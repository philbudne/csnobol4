/* $Id$ */

#include <stdio.h>			/* for usage! */
#include <signal.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "data.h"			/* SIL data */

/* return type of signal handler functions */
#ifndef SIGFUNC_T
#define SIGFUNC_T void
#endif

#ifdef __STDC__
#ifndef SIGARG_T
#define SIGARG_T int
#endif
#endif

extern char *dynamic();
extern SIGFUNC_T SYSCUT(SIGARG_T);	/* never returns */

#define NDESCR 25000			/* default */

int ndescr;
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
    p( "Usage: %s [options...] [files...] [-- parameters]\n", jname );
    p( "-b\ttoggle display of startup banner\n");
    fprintf(stderr,
	    "-d ND\tSize of dynamic region in descriptors (def: %d)\n",
	    NDESCR);
    p( "-f\ttoggle folding of identifiers to upper case (-CASE)\n");
    p( "-k\ttoggle running programs with compilation errors (-[NO]ERRORS)\n");
    p( "-l\treenable listings (-LIST)\n");
    p( "-n\ttoggle running program after compilation (-[NO]EXECUTE)\n");
    p( "-p\ttoggle SPITBOL operators (-PLUSOPS)\n");
    p( "-r\ttoggle reading INPUT from after END statement\n");
    p( "-s\ttoggle display of statistics\n");
    p( "-u PARMS\tparameter data for program\n");
    exit(1);
}

void
init_args( ac, av )
    int ac;
    char *av[];
{
    int errs;
    int c;
    char k;
    int multifile;

    /* save in globals for HOST() */
    argc = ac;
    argv = av;

    errs = 0;
    ndescr = NDESCR;
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

    while ((c = getopt(argc, argv, "bd:fklnprsu:M")) != -1) {
	switch (c) {
	case 'b':
	    D_A(BANRCL) = !D_A(BANRCL);	/* toggle banner output */
	    break;

	case 'd':			/* number of dynamic descrs */
	    switch (sscanf(optarg, "%d%c", &ndescr, &k)) {
	    case 2:
		if (k == 'k' || k == 'K')
		    ndescr *= 1000;
		else
		    errs++;
		break;
	    case 1:
		break;
	    default:
		errs++;
	    }
	    /* XXX enforce a minimum?? */
	    break;

	case 'f':			/* toggle case folding */
	    D_A(CASECL) = !D_A(CASECL);
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

	case 'M':			/* SITBOL multi-file input */
	    multifile = !multifile;
	    break;

	default:
	    errs++;
	}
    }

    /* append additional args to input stream until "--" seen */
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

    /* XXX setup specifier pointing to params for &PARM? */

    io_init();				/* AFTER io_input calls! */

    if (errs) {
	usage(argv[0]);
    }
}

int math_error;				/* see macros.h */

static SIGFUNC_T
math_catch()
{
#ifdef SIGFPE
    signal(SIGFPE, math_catch);
#endif /* SIGFPE defined */
#ifdef SIGOVER
    signal(SIGOVER, math_catch);
#endif /* SIGOVER defined */
    math_error = TRUE;
}

void
init()
{
    char *dyn;
    int len;

    /* allocate dynamic data region */

    len = DESCR * ndescr;
    dyn = dynamic(len);

    if (dyn == NULL) {
	fprintf( stderr,
		"could not allocate dynamic region of %d bytes\n", len);
	exit(1);
    }

    bzero( dyn, len );			/* XXX needed? */

    D_A(FRSGPT) = D_A(HDSGPT) = (int_t) dyn; /* first dynamic descr */

    /* first descr past end of dyn */
    D_A(TLSGP1) = (int_t) dyn + len;

    /* XXX call wrapper functions which save reason for signal? */
    signal( SIGINT, SYSCUT );

    /* catch bad memory references */
    signal( SIGSEGV, SYSCUT );
#ifdef SIGBUS
    signal( SIGBUS, SYSCUT );
#endif /* SIGBUS defined */

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
