/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#define ALPHSZ 128			/* XXX macros.h */
#define MASK (ALPHSZ-1)

#if ALPHSZ & MASK
#include "you lose"
#endif /* ALPHSZ & MASK */

void
rplace(sp1, sp2, sp3)
    struct spec *sp1, *sp2, *sp3;
{
#if 0
    INTR10();
#else  /* not 0 */
    int l1, l2, l3;
    char *cp1, *cp2, *cp3;
    char table[ALPHSZ];
    int i;

    l1 = S_L(sp1);
    l2 = S_L(sp2);
    l3 = S_L(sp3);

    cp1 = S_SP(sp1);
    cp2 = S_SP(sp2);
    cp3 = S_SP(sp3);

    /* XXX check if l2 == l3? */

    /* set up identity vector */
    for (i = 0; i < ALPHSZ; i++)
	table[i] = i;

    /* read translations into vector */
    while (l2-- > 0 && l3-- > 0)
	table[ *cp2++ & MASK ] = *cp3++;

    /* translate string in place */
    while (l1-- > 0) {
	*cp1 = table[ *cp1 & MASK ];
	cp1++;
    }	
#endif /* not 0 */
}

