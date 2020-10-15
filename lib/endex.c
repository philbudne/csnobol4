/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for exit(), abort() */
#include <stdio.h>			/* for lib.h */

#include "h.h"				/* for data.h */
#include "snotypes.h"
#include "macros.h"
#include "equ.h"
#include "res.h"
#include "data.h"			/* for RETCOD */
#include "lib.h"			/* for io_finish() */
#include "inet.h"			/* inet_cleanup() */

#ifdef SHARED
#include <setjmp.h>
extern VAR jmp_buf endex_jmpbuf;	/* UGH! */
#endif /* SHARED defined */

#ifdef TRACE_DEPTH
#define MAX_DEPTH 50000
int cdepth;
int tdepth[MAX_DEPTH];
int returns[MAX_DEPTH];
#endif /* TRACE_DEPTH defined */

void
endex(int x) {
#ifdef TRACE_DEPTH
    int i;
#endif /* TRACE_DEPTH defined */

    io_finish();
    inet_cleanup();
#ifdef SHARED
    cleanup();
#endif

    /* if &ABEND set, dump core?! */
    if (x) {
	abort();
    }

#ifdef TRACE_DEPTH
    for (i = 0; i < MAX_DEPTH; i++)
	if (returns[i])
	    fprintf( stderr, "%8d %8d\n", i, returns[i]);
#endif /* TRACE_DEPTH defined */

#ifdef SHARED
    longjmp(endex_jmpbuf, 1);
#else  /* SHARED not defined */
    /* else exit w/ &CODE */
    exit(D_A(RETCOD));
#endif /* SHARED not defined */
}
