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
    fprintf( stderr, "-k\trun programs with compilation errors\n");
    fprintf( stderr, "-r\tread INPUT from after END statement\n");
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

    D_A(LISTCL) = 0;		/* XXX TEMP!! kill listings!! */

    while ((c = getopt(argc, argv, "bd:klrs")) != -1) {
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

	case 'l':
	    D_A(LISTCL) = 1;		/* XXX TEMP!! re-enable listings! */
	    /* XXX should take listing file!!! */
	    break;

	case 'r':			/* read INPUT from source after END */
	    rflag++;
	    break;

	case 's':
	    D_A(STATCL) = !D_A(STATCL);
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

    /* XXX set up for SIGFPE?  call matherr()?? */

    /* XXX call wrapper functions which save reason for signal? */
    signal( SIGINT, SYSCUT );

    /* ???!!! */
    signal( SIGBUS, SYSCUT );
    signal( SIGSEGV, SYSCUT );
}
