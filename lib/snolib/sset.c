/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * Experimental:
 * SSET("SET(INTEGER,INTEGER,INTEGER[,INTEGER])INTEGER")
 *
 * Usage;	SSET(unit, offset, whence, scale)
 * Returns;	scaled file position
 */

int
SSET( LA_ALIST ) LA_DCL
{
    int_t unit, offset, whence, scale;

    unit = LA_INT(0);
    offset = LA_INT(1);
    whence = LA_INT(2);
    scale = LA_INT(3);
    if (scale == 0)
	scale = 1;

    RETINT( io_sseek( unit, offset, whence, scale ) );
}
