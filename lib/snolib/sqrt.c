/* $Id$ */

/*
 * SPARC SITBOL compatibility;
 * LOAD("SQRT(REAL)REAL")
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <math.h>

SQRT( LA_ALIST ) LA_DCL
{
    RETREAL( sqrt( LA_REAL(0) ) );
}
