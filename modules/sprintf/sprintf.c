/* $Id$ */

/*
 * LOAD("SPRINTF(STRING,INTEGER)STRING")
 *
 * Usage;	SPRINTF(format,int)
 * Returns;	null string or failure
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

int
SPRINTF( LA_ALIST ) LA_DCL
{
    char fmt[256];			/* XXX */
    char buf[256];			/* XXX */

    getstring( LA_PTR(0), fmt, sizeof(fmt) );
    sprintf( buf, fmt, LA_INT(1) );

    RETSTR(buf, strlen(buf));
}
