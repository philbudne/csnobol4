/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("ATAN(REAL)REAL")
 * arc tangent
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"

pmlret_t
ATAN( LA_ALIST ) {
    real_t ret = atan(LA_REAL(0));
    if (!REAL_ISFINITE(ret))
	RETFAIL;
    RETREAL(ret);
}
