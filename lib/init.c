/* $Id$ */

#include <stdio.h>			/* for usage! */
#include <signal.h>
#include <sys/vadvise.h>

#include "h.h"
#include "types.h"
#include "macros.h"

char *malloc();

extern int SYSCUT();
extern struct descr FRSGPT[1];
extern struct descr HDSGPT[1];
extern struct descr TLSGP1[1];

extern struct descr LISTCL[1];		/* XXX TEMP! */
extern struct descr NERRCL[1];		/* -NOERROR switch */

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
    fprintf( stderr,
	    "-d ND\tSize of dynamic region in descriptors (def: %d)\n",
	    NDESCR);
    fprintf( stderr, "-k\trun programs with compilation errors\n");
    fprintf( stderr, "-r\tread INPUT from after END statement\n");
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

    while ((c = getopt(argc, argv, "d:klr")) != -1) {
	switch (c) {
	case 'd':			/* number of dynamic descrs */
	    /* XXX allow 'k' suffix? */
	    if (sscanf(optarg, "%d", &ndescr) != 1)
		errs++;
	    /* XXX enforce a minimum?? */
	    break;

	case 'k':
	    D_A(NERRCL) = 0;		/* run programs with errors */
	    break;

	case 'l':
	    D_A(LISTCL) = 1;		/* XXX TEMP!! re-enable listings! */
	    break;

	case 'r':			/* read INPUT from source after END */
	    rflag++;
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
    dyn = malloc(len);
    /* XXX use valloc then madvise on dynamic region?!?? */
    vadvise(VA_ANOM);			/* warn VM we're random during GC */

    if (dyn == NULL) {
	/* XXX output message to stderr! */
	exit(1);
    }

    bzero( dyn, len );			/* XXX needed? */

    D_A(FRSGPT) = D_A(HDSGPT) = (int) dyn; /* first dynamic descr */

    /* first descr past end of dyn */
    D_A(TLSGP1) = (int) dyn + len;

    /* XXX set up for SIGFPE?  call matherr()?? */

    signal( SIGINT, SYSCUT );		/* ?! */
#if 1
    signal( SIGBUS, SYSCUT );		/* ???!!! */
    signal( SIGSEGV, SYSCUT );		/* ???!!! */
#endif /* 1 */
}
