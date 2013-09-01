/*
 * CSNOBOL4 breakpoints
 * Phil Budne
 * August 31, 2013
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */
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

typedef unsigned char break_t;
static int break_max = -1;
static break_t *breakpoints;

/*
 * called from "INIT" routine if: &TRACE > 0, &STCOUNT > 0
 * non-zero return will cause KEYWORD TRACE event for "STNO"
 */
int
chk_break(x)
    int x;
{
    int stn = D_A(STNOCL);
    if (!breakpoints || stn > break_max)
	return 0;
    /* XXX what to do with value?? could:
     * post-decrement if non-zero (limit number of hits)
     * if non-zero: pre-decrement, and return !value (pass count)
     * OR -- allow signed value, and do BOTH!! have ~0 mean ALWAYS??
     * OR.... take two values
     */
    return breakpoints[stn];
}

/*
 * PMPROTO("BREAKPOINT(INTEGER,INTEGER)INTEGER")
 *
 * Usage;	BREAKPOINT(statement, enable)
 * Returns;	old value or failure
 */
int
BREAKPOINT( LA_ALIST ) LA_DCL
{
    int stn = LA_INT(0);
    int enab = LA_INT(1);
    int save;
    if (!breakpoints) {
	if (!enab)
	    return 0;
	break_max = D_A(CSTNCL);
	breakpoints = (break_t *) malloc(break_max * sizeof(break_t));
	if (!breakpoints)
	    RETFAIL;
	bzero(breakpoints, break_max * sizeof(break_t));
    }
    if (stn > break_max)
	RETFAIL;
    save = breakpoints[stn];
    breakpoints[stn] = !!enab;		/* just zero or one for now */
    RETINT(save);
}
