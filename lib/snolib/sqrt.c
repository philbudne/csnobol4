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

pmlret_t
SQRT( LA_ALIST ) {
    real_t x;

    (void) nargs;
    x = LA_REAL(0);
    if (x < 0) {
	RETFAIL;
    }
    RETREAL( sqrt( x ) );
}
