/* $Id$ */

/*
 * $Log$
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

#include "h.h"				/* const */

#ifndef MAIN
#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "data.h"
#endif

const char vers[] = "0.99.1";
const char vdate[] = "Oct 29, 1996";
const char snoname[] = "C-MAINBOL";

#ifdef MAIN
main() {
    puts(vers);
    exit(0);
}
#else

void
version()
{
    if( D_A(BANRCL) == 0 )
	return;

    io_printf(D_A(PUNCH),
"The Macro Implementation of SNOBOL4 in C (%s) Version %s\n", snoname, vers);
    io_printf(D_A(PUNCH), "    by Philip L. Budne, %s\n", vdate);
}
#endif
