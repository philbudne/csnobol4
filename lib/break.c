/*
 * CSNOBOL4 breakpoints
 * Phil Budne
 * August 31, 2013
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for malloc */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"			/* chk_break prototype */
#include "str.h"
#include "load.h"

/* generated */
#include "equ.h"
#include "res.h"
#include "data.h"			/* SIL data */

typedef unsigned char break_t;		/* one byte/break for fast access */
static VAR int break_max;
static VAR break_t *breakpoints;	/* XXX need to free! */

/*
 * called from "INIT" routine if: &TRACE > 0, &STCOUNT > 0
 * non-zero return will cause KEYWORD TRACE event for "STNO"
 */
int
chk_break(int x) {
    int stn = D_A(STNOCL);
    if (!breakpoints || stn > break_max || stn == 0)
	return 0;
    (void) x;
    /* XXX what to do with value?? could:
     * post-decrement if non-zero (limit number of hits)
     * if non-zero: pre-decrement, and return !value (pass count)
     * OR -- allow signed value, and do BOTH!! have ~0 mean ALWAYS??
     * OR.... take two values
     */
    return breakpoints[stn-1];
}

/*
 * PMPROTO("BREAKPOINT(INTEGER,INTEGER)INTEGER")
 *
 * Usage;	BREAKPOINT(statement, enable)
 * Returns;	old value or failure
 */
#ifdef SHARED
void
break_cleanup(void) {
    free(breakpoints);
    breakpoints = NULL;
}
#endif

pmlret_t
BREAKPOINT( LA_ALIST ) {
    int stn = LA_INT(0);		/* unlikely to exceed 2^32-1! */
    int enab = LA_INT(1);
    int save;

    (void) nargs;
    if (stn <= 0)
	RETFAIL;

    if (!breakpoints) {
	if (!enab)
	    return 0;
	break_max = D_A(CSTNCL);
	breakpoints = (break_t *) malloc(break_max * sizeof(break_t));
	if (!breakpoints)
	    RETFAIL;
	bzero(breakpoints, break_max * sizeof(break_t));
#ifdef SHARED
	reg_cleanup(break_cleanup);
#endif
    }
    else if (stn > break_max) {
	break_t *nbreak;		/* was static?!? */
	int new_max;
	int new_slots;
	if (stn > D_A(CSTNCL))		/* only allow break on existing stmt */
	    RETFAIL;

	/* add twice the number of statements added since last allocation */
	new_slots = 2 * (D_A(CSTNCL)-break_max);
	new_max = D_A(CSTNCL) + new_slots;
	nbreak = (break_t *) realloc(breakpoints, new_max*sizeof(break_t));
	if (!nbreak)
	    RETFAIL;			/* realloc failed */

	/* clear new slots: */
	bzero(nbreak+break_max, new_slots*sizeof(break_t));
	breakpoints = nbreak;
	break_max = new_max;
    }
    stn--;				/* make zero based */

    save = breakpoints[stn];
    breakpoints[stn] = !!enab;		/* just zero or one for now */
    RETINT(save);
}
