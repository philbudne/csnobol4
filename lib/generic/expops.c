/* $Id$ */

#include "h.h"
#include "types.h"

#include "macros.h"
#include "flags.h"

#include <math.h>

int
expint(res,x,y)
    struct descr *res, *x, *y;
{
#if 0
    UNDF();
#else
    int_t i;

    if (D_A(x) == 0 && D_A(y) < 0)
	return 0;			/* fail */

    /* XXX use repeated mutiplies? */
    CLR_MATH_ERROR();
    i = pow( (double)D_A(x), (double)D_A(y) );
    if (MATH_ERROR())
	return 0;			/* fail */
    D(res) = D(x);			/* XXX copy F&V */
    D_A(res) = i;
    return 1;				/* succeed */
#endif
}

int
exreal(res,x,y)
    struct descr *res, *x, *y;
{
#if 0
    INTR10();
#else
    real_t r;

    /* XXX use repeated mutiplies? */
    CLR_MATH_ERROR();
    r = pow( D_RV(x), D_RV(y) );
    if (MATH_ERROR())
	return 0;			/* fail */
    D(res) = D(x);			/* XXX copy F&V */
    D_RV(res) = r;
    return 1;				/* succeed */
#endif
}
