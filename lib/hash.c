/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

unsigned long
hash(sp)
    struct spec *sp;
{
    char *cp;
    int len;
    unsigned long sum;

    sum = 0;
    len = S_L(sp);
    cp = S_SP(sp);

#if 0
    if (strncmp(cp, "END",3) == 0 && len == 3) {
	puts("hash: END");
    }
#endif /* 0 */

    while (len-- > 0) {
	sum = (sum << 2) ^ *cp++;
    }
    return sum % 0xffff;
}
