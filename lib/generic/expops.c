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
    int_t ix, iy;
    int_t p;

    ix = D_A(x);
    iy = D_A(y);

    if (ix == 0 && iy < 0)		/* by definition of EXPINT macro */
	return 0;			/* fail */

#if 1
    if (iy < 0) {
	p = 0;
    }
    else {
	p = 1;
	for (;;) {
	    if (iy & 1)
		p *= ix;		/* XXX check overflow */
	    iy >>= 1;
	    if (iy == 0)
		break;
	    ix *= ix;			/* XXX check overflow */
	}
    }
    D(res) = D(x);			/* XXX copy F&V */
    D_A(res) = (int_t) p;
#else
    CLR_MATH_ERROR();
    p = pow( (double)ix, (double)iy );
    D(res) = D(x);			/* XXX copy F&V */
    D_A(res) = (int_t) p;
    if (MATH_ERROR())
	return 0;			/* fail */
#endif
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
