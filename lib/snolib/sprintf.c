/* $Id$ */

/*
 * Format integers
 *
 * this is primarily a test of RETSTR();  
 * CONVERT() should do radix conversions (like in SITBOL)!!
 */

/*
 * LOAD("SPRINTF(STRING,INTEGER)STRING")
 *
 * Usage;	SPRINTF(format,int)
 *			ie; SPRINTF("%x",32)
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
