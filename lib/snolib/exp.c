/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("EXP(REAL)REAL")
 * exponential e ** x
 */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

EXP( LA_ALIST ) LA_DCL
{
    RETREAL( exp( LA_REAL(0) ) );
}
