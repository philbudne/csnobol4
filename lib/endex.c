/* $Id$ */

#include "h.h"				/* for data.h */
#include "snotypes.h"
#include "macros.h"
#include "equ.h"
#include "res.h"
#include "data.h"			/* for RETCOD */

#ifdef TRACE_DEPTH
#include <stdio.h>

#define MAX_DEPTH 50000
int cdepth;
int tdepth[MAX_DEPTH];
int returns[MAX_DEPTH];
#endif /* TRACE_DEPTH defined */

void
endex( x )
    int x;
{
    int i;

    tty_restore();

    /* if &ABEND set, dump core?! */
    if (x) {
	/* XXX perform I/O cleanup?! */
	abort();
    }

#ifdef TRACE_DEPTH
    for (i = 0; i < MAX_DEPTH; i++)
	if (returns[i])
	    fprintf( stderr, "%8d %8d\n", i, returns[i]);
#endif /* TRACE_DEPTH defined */,

    /* else exit w/ &CODE */
    exit(D_A(RETCOD));
}

