/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("LOG(REAL)REAL")
 * natural logarithm
 */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

LOG( LA_ALIST ) LA_DCL
{
    RETREAL( log( LA_REAL(0) ) );
}
