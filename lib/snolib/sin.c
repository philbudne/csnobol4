/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("SIN(REAL)REAL")
 * sine of angle in radians
 */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

SIN( LA_ALIST ) LA_DCL
{
    RETREAL( sin( LA_REAL(0) ) );
}
