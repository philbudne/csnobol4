/* $Id$ */

/*
 * SPARC SITBOL compatibility;
 * LOAD("SIN(REAL)REAL")
 * cosine of angle in radians
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <math.h>

SIN( LA_ALIST ) LA_DCL
{
    RETREAL( sin( LA_REAL(0) ) );
}
