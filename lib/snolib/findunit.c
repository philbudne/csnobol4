/* $Id$ */

/*
 * LOAD("IO_FINDUNIT()INTEGER")
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

IO_FINDUNIT( LA_ALIST ) LA_DCL
{
    int i;

    i = io_findunit();			/* from lib/io.c */
    if (i < 0) {
	RETFAIL;
    }
    RETINT( i );
}
