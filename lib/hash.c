/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

unsigned long
hash(sp)
    struct spec *sp;
{
    char *cp;
    int len, l2;
    unsigned long sum;

    sum = 0;
    len = S_L(sp);
    cp = S_SP(sp);

#if 0
    if (strncmp(cp, "END",3) == 0 && len == 3) {
	puts("hash: END");
    }
#endif /* 0 */

    sum = len;

    /* sum first four */
    l2 = 4;
    if (len < 4)
	l2 = len;
    len -= l2;
    while (l2-- > 0) {
	sum += (unsigned char)*cp++;
    }

    /* and last four */
    if (len > 4) {
	cp += (len - 4);
	len = 4;
    }
    while (len-- > 0) {
	sum += (unsigned char)*cp++;
    }
    return sum & 0xffff;
}
