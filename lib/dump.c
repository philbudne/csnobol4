/* $Id$ */

/* debug functions to call from gdb! */

/*
 * todo:
 * ptable(), parray(), pspec()
 */

/*#define DUMP				/* TEMP */

#ifdef DUMP
#include <stdio.h>

#include "snotypes.h"
#include "equ.h"
#include "data.h"
#include "macros.h"

void
more() {
    static count;
    char buf[128];

    if (count++ < 20)			/* XXX */
	return;

    count = 0;
    gets(buf);
}

void
pdescr(dp)
    struct descr *dp;
{
    printf("v %d f <", D_V(dp) );
    if (D_F(dp) == 0) {
	putchar('0');
    }
    else {
	if (D_F(dp) & FNC)
	    putchar('F');
	if (D_F(dp) & TTL)
	    putchar('T');
	if (D_F(dp) & STTL)
	    putchar('S');
	if (D_F(dp) & MARK)
	    putchar('M');
	if (D_F(dp) & PTR)
	    putchar('P');
    }
    printf("> a %#x (%d)", D_A(dp), D_A(dp) );

    /* XXX follow ptr ?? */

    if (D_F(dp) & STTL) {
	char *cp;
	int i;

	putchar(' ');
	putchar('\'');

	/* XXX c.f. LOCSP */
	cp = (char *)dp + BCDFLD;
	i = D_V(dp);
	while (i-- > 0) {
	    char c;

	    c = *cp++;
	    if (c < 0x20 || c > 0x7e)
		c = '.';
	    putchar(c);
	}
	putchar('\'');
    }
    putchar('\n');
    fflush(stdout);
}

/* dump all of dynamic storage */
void
dump_dyn() {
    int a;

    a = D_A(HDSGPT);
    while (a < D_A(FRSGPT)) {
	struct descr *dp;

	dp = (struct descr *) a;

	printf("%#x: ", a);
	pdescr(dp);
	putchar('\n');
	more();

	if (!(D_F(dp) & TTL)) {
	    puts("NO TITLE.");
	    break;
	}

	/* XXX this is BKSIZE */
	if (D_F(dp) & STTL) {
	    a += DESCR*(4+((D_V(dp)-1)/CPD+1));
	}
	else {
	    /* XXX scan for pointers? */
	    a += D_V(dp) + DESCR;
	}
    }
}

/* dump all strings/names */
void
dump_vars() {
    int i;

    for (i = 0; i < OBSIZ; i++) {
	int_t a;

	/* get start of i'th hash chain */
	a = D_A(((struct descr *)OBSTRT)+i);
	while (a) {
	    pdescr(a);
	    printf(" :=\t");

	    /* XXX look at datatype / PTR bit?? */
	    pdescr(a + DESCR);
	    putchar('\n');
	    more();

	    a = D_A(a + LNKFLD);	/* get next in chain */
	}
    }
}

void
ptable(dp)
    struct descr *dp;
{
    struct descr *ep;
    int d;

    if (!(D_F(dp) & TTL)) {		/* XXX check self ptr? */
	puts("no title");
	fflush(stdout);
	return;
    }
    d = D_V(dp) / DESCR;
    printf("initial size %d (%d entries)\n", d, d/2-1);
    /* XXX dump entries?? */
    dp = (struct descr *)dp[d].a.ptr;	/* XXX D_PTR? */
    while (dp != (struct descr *)1) {
	d = D_V(dp) / DESCR;
	printf("extent size %d (%d entries)\n", d, d/2-1);
	/* XXX dump entries?? */
	dp = (struct descr *)dp[d].a.ptr; /* XXX D_PTR? */
    }
    fflush(stdout);
}

void
parray(dp)
    struct descr *dp;
{
    int s, n, i;
    if (!(D_F(dp) & TTL)) {		/* XXX check self ptr? */
	puts("no title");
	fflush(stdout);
	return;
    }
    n = D_A(dp+2);
    s = D_V(dp)/DESCR - 2 - n;

    printf("%d entries, %d dimensions:\n", s, n);
    /* XXX print prototype string @ D_PTR(dp+1)? */

    for (i = 0; i < n; i++) {
	printf("%d: s %d l %d\n", i+1, D_V(dp+(n-i+2)), D_A(dp+(n-i+2)));
    }
    fflush(stdout);
}
#endif /* DUMP defined */
