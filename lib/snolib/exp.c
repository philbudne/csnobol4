/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("EXP2(REAL)REAL")
 * exponential e ** x
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <math.h>

EXP2( LA_ALIST ) LA_DCL
{
    RETREAL( exp( LA_REAL(0) ) );
}
