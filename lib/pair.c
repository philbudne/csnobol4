/* $Id$ */

/*
 * pair.c - attribute pair searches
 * 10/27/93
 */

#ifndef STATIC_PAIR			/* not included */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"
#include "equ.h"			/* R & I for DCMP */

#define STATIC_PAIR
#endif /* STATIC_PAIR not defined */

/*
 * odd members are "type"
 * even members are "value"
 */

/* locate attribute pair by type */
STATIC_PAIR int
locapt(struct descr *d1, struct descr *d2, struct descr *d3) {
    register int_t a;
    int_t end;

    a = D_A(d2);
    end = a + D_V(a);

    while (a < end) {			/* XXX .LE.? */
	if (DCMP(a+DESCR, d3)) {	/* compare types */
	    /* XXX 360 macros clear F&V! */
	    D(d1) = D(d2);		/* copy F & V */
	    D_A(d1) = a;
	    return 1;			/* true (success) */
	}
	a += 2*DESCR;
    }
    return 0;				/* false (failure) */
}

STATIC_PAIR int
locapv(struct descr *d1, struct descr *d2, struct descr *d3) {
    register int_t a;
    int_t end;

    a = D_A(d2);
    end = a + D_V(a);

    while (a < end) {			/* XXX .LE.? */
	int_t a2;

	a2 = a + 2*DESCR;
	if (DCMP(a2, d3)) {		/* compare values */
	    /* XXX 360 macros clear F&V! */
	    D(d1) = D(d2);		/* copy F & V */
	    D_A(d1) = a;
	    return 1;			/* true (success) */
	}
	a = a2;
    }
    return 0;				/* false (failure) */
}
