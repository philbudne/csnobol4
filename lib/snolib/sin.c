/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("SIN(REAL)REAL")
 * sine of angle in radians
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
#include "pmlproto.h"			/* prototype */

pmlret_t
SIN( LA_ALIST ) {
    (void) nargs;
    RETREAL( sin( LA_REAL(0) ) );
}
