/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("SQRT(REAL)REAL")
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"

SQRT( LA_ALIST ) LA_DCL
{
    real_t x;

    x = LA_REAL(0);
    if (x < 0) {
	RETFAIL;
    }
    RETREAL( sqrt( x ) );
}
