/* $Id$ */

/*
 * pat.c - pattern building
 * 10/27/93
 */

#ifndef STATIC_PAT
#include "h.h"
#include "snotypes.h"
#include "macros.h"
#define STATIC_PAT
#endif

STATIC_PAT void
linkor( d1, d2 )
    struct descr *d1, *d2;
{
    int_t a, i, j;

    a = D_A(d1);
    i = 0;
    for (;;) {
	j = D_A(a + 2*DESCR + i);
	if (j == 0)
	    break;
	i = j;
    }
    D_A(a + 2*DESCR + i) = D_A(d2);
}

/* S4D58 pp42-43 */
STATIC_PAT void
maknod(d1, d2, d3, d4, d5, d6)
    struct descr *d1, *d2, *d3, *d4, *d5, *d6;
{
    int_t a2;

    a2 = D_A(d2);
    D(a2 + DESCR) = D(d5);
    D_A(a2 + 2*DESCR) = D_A(d4);
    D_A(a2 + 3*DESCR) = D_A(d3);
    if (d6) {
	D(a2 + 4*DESCR) = D(d6);
    }

    /* NOTE: must be last, can have d1 == d6!!! */
    D(d1) = D(d2);
}

STATIC_PAT void
lvalue(d1,d2)
    struct descr *d1, *d2;
{
    int_t i, a, offset;

    a = D_A(d2);

    offset = D_A(a + 2*DESCR);
    i = D_A(a + 3*DESCR);

    while (offset != 0) {		/* zero offset terminates list */
	int_t j;

	j = D_A(a + offset + 3*DESCR);	/* fetch min length */
	if (j < i)
	    i = j;			/* get new minimum */

	offset = D_A(a + offset + 2*DESCR); /* fetch next offset */
    }
    D_A(d1) = i;
    D_F(d1) = 0;
    D_V(d1) = 0;
}

/* S4D58 pp19-20 */
STATIC_PAT void
cpypat(d1, d2, d3, d4, d5, d6)
    struct descr *d1, *d2, *d3, *d4, *d5, *d6;
{
    int_t r1, r2, r3;
    int_t a3, a4, a5;

    r1 = D_A(d1);
    r2 = D_A(d2);
    r3 = D_A(d6);

    a3 = D_A(d3);
    a4 = D_A(d4);
    a5 = D_A(d5);

#define F1(X) ((X) == 0 ? 0 : ((X) + a4))
#define F2(X) ((X) == 0 ? a5 : ((X) + a4))

    do {
	int_t v7, a8, v8, a9, v9;

	v7 = D_V(r2 + DESCR);
	D(r1 + DESCR) = D(r2 + DESCR);

	a8 = D_A(r2 + 2*DESCR);
	v8 = D_V(r2 + 2*DESCR);
	D_A(r1 + 2*DESCR) = F1(a8);
	D_F(r1 + 2*DESCR) = 0;
	D_V(r1 + 2*DESCR) = F2(v8);

	a9 = D_A(r2 + 3*DESCR);
	v9 = D_V(r2 + 3*DESCR);
	D_A(r1 + 3*DESCR) = a9 + a3;
	D_F(r1 + 3*DESCR) = 0;
	D_V(r1 + 3*DESCR) = v9 + a3;

	if (v7 == 3) {
	    D(r1 + 4*DESCR) = D(r2 + 4*DESCR);
	}

	r1 += (v7+1)*DESCR;
	r2 += (v7+1)*DESCR;
	r3 -= (v7+1)*DESCR;
    } while(r3 > 0);

    D_A(d1) = r1;
}
