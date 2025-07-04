/* $Id$ */

/*
 * SNOBOL4+ compatibility;
 * LOAD("CHOP(REAL)REAL")
 * discard fractional part of real number (no rounding)
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
#include "pmlproto.h"			/* for prototype */

/*
 * SunOS 4 has "aint" function, but it's not part of any stanard!
 *
 * floor() and ceil() are in SVID, XPG, POSIX and ANSI standards
 *	(and were in v7 and BSD4.2)
 */

pmlret_t
CHOP( LA_ALIST ) {
    double x;
    (void) nargs;
    x = LA_REAL(0);
    if (x >= 0) {
	RETREAL( floor(x) );
    }
    else {
	RETREAL( ceil(x) );
    }
}
