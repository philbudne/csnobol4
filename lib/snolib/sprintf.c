/* $Id$ */

/*
 * Format single int or float
 *
 * this is primarily a test of RETSTR()
 * CONVERT() should do radix conversions (like in SITBOL)!!
 *
 * original 6/94
 * support real too 9/96
 */

/*
**=pea
**=sect NAME
**snobol4fork - POSIX fork for SNOBOL4
**=sect SYNOPSYS
**=code
**-INCLUDE 'sprintf.sno'
**
**        string = B<SPRINTF(>I<format>,I<value>B<)>
**=ecode
**=sect DESCRIPTION
**B<SPRINTF()> can be used to format B<INTEGER> and B<REAL> values.
**=cut
*/
/*
 * LOAD("SPRINTF(STRING,)STRING", SPRINTF_DL)
 *
 * Usage;	SPRINTF(format,value)
 *			ie; SPRINTF("%x",32)
 *			    SPRINTF("%10.2f", 3.14)
 * Returns;	string (fails if second argument not INTEGER or REAL)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "str.h"

/* machine generated: */
#include "equ.h"			/* I & R */

int
SPRINTF( LA_ALIST ) LA_DCL
{
    char fmt[1024];			/* XXX */
    char buf[2048];			/* XXX */

    getstring(LA_PTR(0), fmt, sizeof(fmt));
    switch (LA_TYPE(1)) {
    case I:
	sprintf(buf, fmt, LA_INT(1));
	break;
    case R:
	sprintf(buf, fmt, LA_REAL(1));
	break;
    default:
	RETFAIL;
    }
    RETSTR(buf);
}

/*
**=pea
**=sect SEE ALSO
**B<snobol4(>1B<)>, B<sprintf(>3B<)>
**=sect AUTHOR
**Philip L. Budne
**=cut
*/
