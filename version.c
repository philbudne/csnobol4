/* $Id$ */

/*
 * $Log$
 * Revision 1.1  94/05/25  02:14:28  budd
 * Initial revision
 * 
 */

#include "units.h"
#include "macros.h"
#include "types.h"
#include "data.h"

static const char vers[] = "0.90.1";
static const char date[] = "May 24, 1994";

void
version()
{
    if( D_A(BANRCL) == 0 )
	return;

    io_printf(D_A(PUNCH), "
The Macro Implementation of SNOBOL4 for Un*x, (Version %s)\n", vers);
    io_printf(D_A(PUNCH), "    Philip L. Budne, %s\n", date);
}
