/* $Id$ */

#ifndef STATIC_HASH
#include "h.h"
#include "snotypes.h"
#include "macros.h"

/* machine generated; */
#include "equ.h"			/* for OBSIZ */

#define STATIC_HASH
#endif /* STATIC_HASH not defined */

STATIC_HASH void
hash(dp, sp)
    struct descr *dp;
    struct spec *sp;
{
    char *cp;
    int len, l2;
    unsigned sum;

    sum = 0;
    len = S_L(sp);
    cp = S_SP(sp);

#if 0
    if (strncmp(cp, "END",3) == 0 && len == 3) {
	puts("hash: END");
    }
#endif /* 0 */

    /* NOTE:
     *
     * This is pretty primative, but portable. It avoids expensive
     * operations (some machines (ie; old sparc) don't have multiply
     * or divide insns), and avoids scanning the entire string (since
     * all strings, not just identifiers are hashed into the "natural
     * variable" table.
     *
     * It's tempting to extract the first 4 and last 4 as 32-bit int's
     * but the strings don't always come in aligned (sparc'ed again),
     * partiularly when compiling and worse, the same string come in
     * with different alignments at different times!
     *

     * The small range of the sum is not a problem, since the default
     * hash table size is a power of 2 (256), only the low order bits
     * are used. Furthermore we use the power of two fact to our
     * advantage.  I tried expanding the hash table to 512 entries,
     * with little improvement, so I left it at 256.
     */

    /* start with length */
    sum = len;

    /* sum first four */
    l2 = 4;
    if (len < 4)
	l2 = len;
    len -= l2;
    while (l2-- > 0) {
	sum += (unsigned char)*cp++;
    }

    /* and last four (if any) */
    if (len > 4) {
	cp += (len - 4);
	len = 4;
    }
    while (len-- > 0) {
	sum += (unsigned char)*cp++;
    }

    /* printf("%u %d %d\n", sum, sum % OBSIZ, sum / OBSIZ); */

#if ((OBSIZ-1) & OBSIZ) == 0
    /* OBSIZ a power of two; use masking */
    D_A(dp) = (sum & (OBSIZ-1)) * DESCR;
#else
    /* OBSIZ not a power of two; use mod */
    D_A(dp) = (sum % OBSIZ) * DESCR;
#endif

    /*
     * use length for "assention" value.
     * 9/24/96; tried using "sum" and "sum/OBSIZ"; neither
     * perform consistently better than length!
     */
    D_V(dp) = S_L(sp);
}
