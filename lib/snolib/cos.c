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

COS( LA_ALIST ) LA_DCL
{
    RETREAL( cos( LA_REAL(0) ) );
}
