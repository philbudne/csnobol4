/* $Id$ */

/*
 * support for exponentiation, using pow() function.
 *
 * pow exists in v6 (buth math.h didn't), SVID2, XPG2, XPG3, POSIX.1, ANSI C
 *
 * see dummy/exp.c for dummy version of this file
 */

# include <math.h>

# include "h.h"
# include "snotypes.h"
# include "macros.h"

# ifdef NO_STATIC_VARS
# include "equ.h"
# include "vars.h"
# endif /* NO_STATIC_VARS defined */

int
expint(result,x,y)
    struct descr *result, *x, *y;
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
/* perform X *= Y, checking for overflow */
#define SIGN (((int_t)1)<<(sizeof(int_t)*8-1))
#define MULT(X,Y) { \
    register int_t temp = (X) * (Y); \
    if (((X) ^ (Y) ^ temp) & SIGN) return 0; \
    X = temp; \
}

	p = 1;
	for (;;) {
	    if (iy & 1)
		MULT(p,ix);
	    iy >>= 1;
	    if (iy == 0)
		break;
	    MULT(ix,ix);
	}
    }
    D(result) = D(x);			/* XXX copy F&V */
    D_A(result) = (int_t) p;
#else
    CLR_MATH_ERROR();
    p = pow( (double)ix, (double)iy );
    D(result) = D(x);			/* XXX copy F&V */
    D_A(result) = (int_t) p;
    if (MATH_ERROR())
	return 0;			/* fail */
#endif
    return 1;				/* succeed */
}

int
exreal(result,x,y)
    struct descr *result, *x, *y;
{
    real_t r;

    /* XXX use repeated mutiplies? */
    CLR_MATH_ERROR();
    r = pow( D_RV(x), D_RV(y) );
    if (MATH_ERROR())
	return 0;			/* fail */
    D(result) = D(x);			/* XXX copy F&V */
    D_RV(result) = r;
    return 1;				/* succeed */
}
