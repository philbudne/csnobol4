/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * LOAD("SYSTEM(STRING)INTEGER")
 *
 * Usage;	SYSTEM("shell command")
 * Returns;	exit status
 */

SYSTEM( LA_ALIST ) LA_DCL
{
    char cmd[256];			/* XXX */

    getstring( LA_PTR(0), cmd, sizeof(cmd) );
    RETINT( system(cmd) );
}
