/* $Id$ */

/*
 * support for exponentiation, using pow() function.
 *
 * pow exists in v6 (buth math.h didn't), SVID2, XPG2, XPG3, POSIX.1, ANSI C
 *
 * see dummy/exp.c for dummy version of this file
 */

# include "h.h"
# include "snotypes.h"
# include "macros.h"

# include <math.h>

int
expint(res,x,y)
    struct descr *res, *x, *y;
{
    int_t i;

    if (D_A(x) == 0 && D_A(y) < 0)	/* by definition of EXPINT macro */
	return 0;			/* fail */

    /* XXX use repeated mutiplies? */
    CLR_MATH_ERROR();
    i = pow( (double)D_A(x), (double)D_A(y) );
    if (MATH_ERROR())
	return 0;			/* fail */
    D(res) = D(x);			/* XXX copy F&V */
    D_A(res) = i;
    return 1;				/* succeed */
}

int
exreal(res,x,y)
    struct descr *res, *x, *y;
{
    real_t r;

    /* XXX use repeated mutiplies? */
    CLR_MATH_ERROR();
    r = pow( D_RV(x), D_RV(y) );
    if (MATH_ERROR())
	return 0;			/* fail */
    D(res) = D(x);			/* XXX copy F&V */
    D_RV(res) = r;
    return 1;				/* succeed */
}
