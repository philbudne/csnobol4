/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("TAN(REAL)REAL")
 * tangent of angle in radians
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <math.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

TAN( LA_ALIST ) LA_DCL
{
    RETREAL( tan( LA_REAL(0) ) );
}
