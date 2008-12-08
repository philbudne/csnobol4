/* $Id$ */

/*
 * $Log$
 * Revision 1.32  2005/12/11 16:52:46  phil
 * 1.1+
 *
 * Revision 1.31  2005/12/09 17:45:52  phil
 * update for 1.1 release
 *
 * Revision 1.30  2004/03/03 16:58:26  phil
 * update to 1.0+
 *
 * Revision 1.29  2004/02/19 03:58:18  phil
 * update to version 1.0
 *
 * Revision 1.28  2003/04/21 22:25:25  phil
 * include lib.h
 *
 * Revision 1.27  2002/03/13 17:47:31  phil
 * update version to 0.99.44+
 *
 * Revision 1.26  2002/03/03 05:02:52  phil
 * update version to 0.99.44
 *
 * Revision 1.25  2001/12/03 01:15:35  phil
 * add HAVE_CONFIG_H
 *
 * Revision 1.24  1998/06/01  04:24:26  phil
 * include vars.h
 *
 * Revision 1.23  1997/08/12  03:52:46  phil
 * 0.99.4
 *
 * Revision 1.20  1997/02/13  05:12:54  phil
 * 0.99.3
 *
 * Revision 1.19  1996/11/27  18:40:36  phil
 * 0.99.2
 *
 * Revision 1.17  1996/10/24  05:09:12  phil
 * 0.99.1
 *
 * Revision 1.16  1996/10/08  06:34:48  phil
 * 0.99
 *
 * Revision 1.15  1996/10/05  06:30:39  phil
 * 0.98.5
 *
 * Revision 1.14  1996/09/30  06:03:27  phil
 * 0.98.4
 *
 * Revision 1.13  1996/09/26  03:47:42  phil
 * added snoname
 * include h.h regardless
 * use const
 *
 * Revision 1.12  1996/09/23  05:20:44  phil
 * 0.98.3
 *
 * Revision 1.11  1996/09/21  07:20:12  phil
 * moved versions back here, added back ifdef MAIN, upped to 0.98.2
 *
 * Revision 1.10  1996/09/18  04:36:10  phil
 * use VERS/VDATE, removed MAIN
 *
 * Revision 1.9  1996/09/05  04:24:57  phil
 * updated to 0.98
 * added ifdef MAIN
 *
 * Revision 1.8  1995/01/21  17:48:53  budd
 * version 0.91.2 (BETA2_patch2)
 *
 * Revision 1.7  1994/07/05  21:42:58  budd
 * update for patch1
 *
 * Revision 1.6  94/06/30  00:40:11  budd
 * clean up for non-ansi compile
 * 
 * Revision 1.5  94/06/25  00:44:43  budd
 * *** empty log message ***
 * 
 * Revision 1.4  94/06/25  00:30:05  budd
 * update for beta2
 * 
 * Revision 1.3  94/05/27  23:51:18  budd
 * use snotypes.h
 * 
 * Revision 1.2  94/05/25  02:29:50  budd
 * moved date to second line
 * 
 * Revision 1.1  94/05/25  02:14:28  budd
 * Initial revision
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include "h.h"				/* const */

const char vers[] = "1.2";
const char vdate[] = "December 9, 2008";
const char snoname[] = "CSNOBOL4";

#ifdef MAIN
main() {
    puts(vers);
    return 0;
}
#else  /* MAIN not defined */

#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"

#include "equ.h"
#include "res.h"
#include "data.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

void
version()
{
    if( D_A(BANRCL) == 0 )
	return;

    io_printf(D_A(PUNCH),
"The Macro Implementation of SNOBOL4 in C (%s) Version %s\n", snoname, vers);
    io_printf(D_A(PUNCH), "    by Philip L. Budne, %s\n", vdate);
}
#endif /* MAIN not defined */
