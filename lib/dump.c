/* $Id$ */

/* debug functions to call from gdb! */

#ifdef DUMP
#include "types.h"

#include "equ.h"

extern struct descr FRSGPT, HDSGPT;
extern struct descr OBSTRT[OBSIZ];

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
    printf("%#x <", dp->a.i );

    if (dp->f == 0) {
	putchar('0');
    }
    else {
	if (dp->f & FNC)
	    putchar('F');
	if (dp->f & TTL)
	    putchar('T');
	if (dp->f & STTL)
	    putchar('S');
	if (dp->f & MARK)
	    putchar('M');
	if (dp->f & PTR)
	    putchar('P');
    }
    printf("> %d", dp->v );

    if (dp->f & STTL) {
	char *cp;
	int i;

	putchar(' ');
	putchar('\'');

	/* XXX c.f. LOCSP */
	cp = (char *)dp + BCDFLD;
	i = dp->v;
	while (i-- > 0) {
	    char c;

	    c = *cp++;
	    if (c < 0x20 || c > 0x7e)
		c = '.';
	    putchar(c);
	}
	putchar('\'');
    }
}

void
dump_dyn() {
    int a;

    a = HDSGPT.a.i;
    while (a < FRSGPT.a.i) {
	struct descr *dp;

	dp = (struct descr *) a;

	printf("%#x: ", a);
	pdescr(dp);
	putchar('\n');
	more();

	if (!(dp->f & TTL)) {
	    puts("NO TITLE.");
	    break;
	}

	/* XXX this is BKSIZE */
	if (dp->f & STTL) {
	    a += DESCR*(4+((dp->v-1)/CPD+1));
	}
	else {
	    /* XXX scan for pointers? */
	    a += dp->v + DESCR;
	}
    }
}

void
dump_vars() {
    int i;

    for (i = 0; i < OBSIZ; i++) {
	int_t a;

	a = OBSTRT[i].a.i;
	while (a) {
	    pdescr(a);
	    printf(" :=\t");

	    /* XXX look at datatype / PTR bit?? */
	    pdescr(a + DESCR);
	    putchar('\n');
	    more();

	    a = ((struct descr *) (a + LNKFLD))->a.i;
	}
    }
}
#endif /* DUMP defined */
