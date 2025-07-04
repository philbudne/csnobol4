/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("LOG(REAL)REAL")
 * natural logarithm
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* for lib.h */
#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "lib.h"			/* for pmlproto.h */
#include "pmlproto.h"

pmlret_t
LOG( LA_ALIST ) {
    (void) nargs;
    RETREAL( log( LA_REAL(0) ) );
}
