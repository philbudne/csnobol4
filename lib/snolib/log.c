/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("LOG(REAL)REAL")
 * natural logarithm
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

LOG( LA_ALIST ) LA_DCL
{
    RETREAL( log( LA_REAL(0) ) );
}
