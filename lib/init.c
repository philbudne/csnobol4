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

extern char *dynamic();
extern SIGFUNC_T SYSCUT();		/* never returns */

#define NDESCR 25000			/* default */

int ndescr;
int rflag;

extern int optind;
extern char *optarg;
extern int getopt();

void
usage( jname )
    char *jname;
{
    fprintf( stderr, "Usage: %s [options...] [files...]\n", jname );
    fprintf( stderr, "-b\ttoggle display of startup banner\n");
    fprintf( stderr,
	    "-d ND\tSize of dynamic region in descriptors (def: %d)\n",
	    NDESCR);
    fprintf( stderr, "-f\ttoggle folding of identifiers to upper case\n");
    fprintf( stderr, "-k\trun programs with compilation errors\n");
    fprintf( stderr, "-l\treenable listings.\n");
    fprintf( stderr, "-p\ttoggle SPITBOL operators (-PLUSOPS)\n");
    fprintf( stderr, "-r\ttoggle reading INPUT from after END statement\n");
    fprintf( stderr, "-s\ttoggle display of statistics\n");
    exit(1);
}

void
init_args( argc, argv )
    int argc;
    char *argv[];
{
    int errs;
    int c;

    errs = 0;
    ndescr = NDESCR;

    /*
     * NOTE:
     *
     * * Options are compatible (where possible) with Catspaw Macro SPITBOL
     *
     * * When adding options, update usage() function (above) and man page!!!
     */
#ifdef vms
    argc = getredirection(argc, argv);
#endif /* vms defined */
    while ((c = getopt(argc, argv, "bd:fklprs")) != -1) {
	switch (c) {
	case 'b':
	    D_A(BANRCL) = !D_A(BANRCL);	/* toggle banner output */
	    break;

	case 'd':			/* number of dynamic descrs */
	    /* XXX allow 'k' suffix? */
	    if (sscanf(optarg, "%d", &ndescr) != 1)
		errs++;
	    /* XXX enforce a minimum?? */
	    break;

	case 'f':
	    D_A(CASECL) = !D_A(CASECL);	/* toggle case folding */
	    break;

	case 'k':
	    /* toggle running programs with compile errors */
	    D_A(NERRCL) = !D_A(NERRCL);
	    break;

	case 'p':			/* -PLUSOPS */
	    D_A(SPITCL) = !D_A(SPITCL);
	    break;

	case 'l':			/* -LIST */
	    /* XXX should take an argument!!! */
	    D_A(LISTCL) = 1;
	    break;

	case 'r':			/* read INPUT from source after END */
	    rflag = !rflag;
	    break;

	case 's':
	    D_A(STATCL) = !D_A(STATCL);	/* toggle statistics */
	    break;

	default:
	    errs++;
	}
    }

    while (optind < argc) {
	io_input( argv[optind] );
	optind++;
    }


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
	fprintf( stderr, "could not allocate dynamic region of %d bytes\n",
		len);
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
