/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "lib.h"

/*
 * Experimental:
 * LOAD("SSET(INTEGER,INTEGER,INTEGER[,INTEGER])INTEGER")
 *
 * Usage;	SSET(unit, offset, whence, scale)
 * Returns;	scaled file position
 */

int
SSET( LA_ALIST ) LA_DCL
{
    int_t unit, offset, whence, scale, oof;

    unit = LA_INT(0);
    offset = LA_INT(1);
    whence = LA_INT(2);
    scale = LA_INT(3);
    if (scale == 0)
	scale = 1;

    if (io_sseek( unit, offset, whence, scale, &oof )) {
	RETINT( oof );
    }
    RETFAIL;
}
