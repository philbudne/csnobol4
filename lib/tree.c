/* $Id$ */

/*
 * tree.c - code tree operations
 * 10/27/93
 */

#include "h.h"
#include "types.h"
#include "macros.h"
#include "equ.h"

void
addsib( d1, d2 )
    struct descr *d1, *d2;
{
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
addson( d1, d2 )
    struct descr *d1, *d2;
{
    int_t a1, a2;

    a1 = D_A(d1);
    a2 = D_A(d2);

    D(a2 + FATHER) = D(d1);
    D(a2 + RSIB) = D(a1 + LSON);
    D(a1 + LSON) = D(d2);
    D_V(a1 + CODE)++;
}

void
insert( d1, d2 )
    struct descr *d1, *d2;
{
    int_t a1, a2, a3, a4;

    a1 = D_A(d1);
    a2 = D_A(d2);
    a3 = D_A(a1 + FATHER);
    a4 = D_A(a3 + LSON);

    D(a2 + FATHER) = D(a1 + FATHER);
    D(a1 + FATHER) = D(d2);
    D(a4 + RSIB) = D(d2);
    D(a2 + LSON) = D(d1);
    D_V(a2 + CODE)++;
}

