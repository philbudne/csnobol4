/* $Id$ */

/*
 * $Log$
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

#ifndef MAIN
#include "h.h"
#include "units.h"
#include "macros.h"
#include "snotypes.h"
#include "data.h"
#endif

char vers[] = "0.98.3";
char vdate[] = "Sept 22, 1996";

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
"The Macro Implementation of SNOBOL4 in C (C-MAINBOL) Version %s\n", vers);
    io_printf(D_A(PUNCH), "    Philip L. Budne, %s\n", vdate);
}
#endif
