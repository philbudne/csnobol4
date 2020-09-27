/* $Id$ */

/*
 * tree.c - code tree operations
 * 10/27/93
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "equ.h"
#include "lib.h"

void
addsib(struct descr *d1, struct descr *d2) {
    int_t a1, a2, a3;

    a1 = D_A(d1);
    a2 = D_A(d2);
    a3 = D_A(a1 + FATHER);

    D(a2 + RSIB) = D(a1 + RSIB);
    D(a2 + FATHER) = D(a1 + FATHER);
    D(a1 + RSIB) = D(d2);
    D_V(a3 + CODE)++;
}

void
addson(struct descr *d1, struct descr *d2) {
    int_t a1, a2;

    a1 = D_A(d1);
    a2 = D_A(d2);

    D(a2 + FATHER) = D(d1);
    D(a2 + RSIB) = D(a1 + LSON);
    D(a1 + LSON) = D(d2);
    D_V(a1 + CODE)++;
}

void
insert(struct descr *d1, struct descr *d2) {
    int_t a1, a2, a3, a4, x, y;

    a1 = D_A(d1);
    a2 = D_A(d2);
    a3 = D_A(a1 + FATHER);
    a4 = D_A(a3 + LSON);

    /* updated from SNOBOL4+ version; */
    x = a4;
    do {
	y = x;
	x = D_A(y + RSIB);
    } while (x != a1);
    D(y + RSIB) = D(d2);

    D(a2 + FATHER) = D(a3);
    D(a2 + LSON) = D(d1);
    D_V(a2 + CODE)++;
    D(a1 + FATHER) = D(d2);
}

