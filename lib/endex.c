/* $Id$ */

#include "h.h"				/* for data.h */
#include "snotypes.h"
#include "macros.h"
#include "data.h"			/* for RETCOD */

#ifdef TRACE_DEPTH
#include <stdio.h>

#define MAX_DEPTH 50000
int depth;
int calls[MAX_DEPTH];
#endif /* TRACE_DEPTH defined */

void
endex( x )
    int x;
{
    int i;

    /* if &ABEND set, dump core?! */
    if (x) {
	/* XXX perform I/O cleanup?! */
	abort();
    }

#ifdef TRACE_DEPTH
    for (i = 0; i < MAX_DEPTH; i++)
	if (calls[i])
	    fprintf( stderr, "%8d %8d\n", i, calls[i]);
#endif /* TRACE_DEPTH defined */

    /* else exit w/ &CODE */
    exit(D_A(RETCOD));
}

