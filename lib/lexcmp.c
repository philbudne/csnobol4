/* $Id$ */

#include "h.h"
#include "types.h"
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

    if (i == 0) {
	if (j == 0)
	    return 0;
	return 1;
    }
    return -1;
}
