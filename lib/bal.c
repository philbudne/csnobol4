/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"			/* getbal() prototype */

int
getbal(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
#if 0
    UNDF();
#else  /* not 0 */
    int len;
    char *cp;
    int count, j;

    cp = S_SP(sp) + S_L(sp);
    len = D_A(dp);

    count = 0;
    j = 1;
    while (len > 0) {
	if (*cp == '(')
	    count++;

	if (*cp == ')') {
	    --count;
	    if (count < 0)
		return 0;		/* false (fail) */
	}

	/* NOT under close-paren test!! */
	if (count == 0)
	    break;

	len--;
	cp++;
	j++;
    }
    if (len == 0)
	return 0;
    S_L(sp) += j;
    return 1;				/* true (success) */
#endif /* not 0 */
}

