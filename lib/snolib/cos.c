/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("SIN(REAL)REAL")
 * cosine of angle in radians
 */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

COS( LA_ALIST ) LA_DCL
{
    RETREAL( cos( LA_REAL(0) ) );
}
