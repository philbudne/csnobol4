/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("SQRT(REAL)REAL")
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <math.h>

SQRT( LA_ALIST ) LA_DCL
{
    real_t x;

    x = LA_REAL(0);
    if (x < 0) {
	RETFAIL;
    }
    RETREAL( sqrt( x ) );
}
