/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

int
lexcmp(a,b)
    struct spec *a, *b;
{
    char *x, *y;
    int i, j;

    i = S_L(a);
    j = S_L(b);
    x = S_SP(a);
    y = S_SP(b);

    while (i > 0 && j > 0) {
	if (*x != *y)
	    return *x - *y;
	i--;
	j--;
	x++;
	y++;
    }

    if (i == 0) {			/* end of a */
	if (j == 0)			/* end of b too? */
	    return 0;			/* yes; identical */
	return -1;			/* no; a < b */
    }
    else if (j == 0)			/* not end of a; end of b? */
	return 1;			/* yes; a > b */

    return -1;
}
