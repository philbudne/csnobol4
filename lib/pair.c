/* $Id$ */

/*
 * pair.c - attribute pair searches
 * 10/27/93
 */

#include "h.h"
#include "types.h"
#include "macros.h"

/*
 * odd members are "type"
 * even members are "value"
 */

/* locate attribute pair by type */
int
locapt( d1, d2, d3 )
    struct descr *d1, *d2, *d3;
{
    int_t a, end;

    a = D_A(d2);
    end = a + D_V(a);

    while (a < end) {			/* XXX .LE.? */
	if (DCMP(a+DESCR, d3) == 0) {	/* compare types */
	    /* XXX 360 macros clear F&V! */
	    D(d1) = D(d2);		/* copy F & V */
	    D_A(d1) = a;
	    return 1;			/* true (success) */
	}
	a += 2*DESCR;
    }
    return 0;				/* false (failure) */
}

int
locapv( d1, d2, d3 )
    struct descr *d1, *d2, *d3;
{
    int_t a, end;

    a = D_A(d2);
    end = a + D_V(a);

    while (a < end) {			/* XXX .LE.? */
	if (DCMP(a+2*DESCR, d3) == 0) {	/* compare values */
	    /* XXX 360 macros clear F&V! */
	    D(d1) = D(d2);		/* copy F & V */
	    D_A(d1) = a;
	    return 1;			/* true (success) */
	}
	a += 2*DESCR;
    }
    return 0;				/* false (failure) */
}
