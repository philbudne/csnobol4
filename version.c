/* $Id$ */

/*
 * $Log$
 */

#include "units.h"
#include "macros.h"
#include "types.h"
#include "data.h"

static const char vers[] = "0.90.1";
static const char date[] = "5/24/1994";

void
version()
{
    if( D_A(BANRCL) == 0 )
	return;

    io_printf(D_A(PUNCH), "
The Macro Implementation of SNOBOL4 for Un*x, (Version %s, %s)\n", vers, date);
    io_printf(D_A(PUNCH), "    by Philip L. Budne\n");
}
