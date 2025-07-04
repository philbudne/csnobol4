/* $Id$ */

/*
 * LOAD("IO_FINDUNIT()INTEGER")
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "lib.h"
#include "pmlproto.h"			/* for prototype */

pmlret_t
IO_FINDUNIT( LA_ALIST ) {
    int i;

    (void) args;
    (void) nargs;
    i = io_findunit();			/* from lib/io.c */
    if (i < 0) {
	RETFAIL;
    }
    RETINT( i );
}
