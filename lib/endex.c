/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"				/* for data.h */
#include "snotypes.h"
#include "macros.h"
#include "equ.h"
#include "res.h"
#include "data.h"			/* for RETCOD */

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
#ifdef ENDEX_LONGJMP
#include <setjmp.h>
extern jmp_buf endex_jmpbuf;
#endif /* ENDEX_LONGJMP defined */
#endif /* NO_STATIC_VARS not defined */

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
#ifdef TRACE_DEPTH
    int i;
#endif /* TRACE_DEPTH defined */

    io_finish();
    inet_cleanup();

    /* if &ABEND set, dump core?! */
    if (x) {
	abort();
    }

#ifdef TRACE_DEPTH
    for (i = 0; i < MAX_DEPTH; i++)
	if (returns[i])
	    fprintf( stderr, "%8d %8d\n", i, returns[i]);
#endif /* TRACE_DEPTH defined */

#ifdef ENDEX_LONGJMP
    longjmp(endex_jmpbuf, 1);
#else  /* ENDEX_LONGJMP not defined */
    /* else exit w/ &CODE */
    exit(D_A(RETCOD));
#endif /* ENDEX_LONGJMP not defined */
}
