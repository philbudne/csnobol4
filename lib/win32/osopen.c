/* $Id$ */

/*
 * MS-DOS open hook
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
osdep_open(fname, mode, fpp)
    const char *fname, *mode;
    FILE **fpp;
{
    if (strcmp(fname, "/dev/tty") == 0) {
	*fpp = fopen("CON:", mode);
	return TRUE;			/* matched */
    }
    if (strcmp(fname, "/dev/nul") == 0) {
	*fpp = fopen("NUL:", mode);
	return TRUE;			/* matched */
    }
    return FALSE;			/* no match */
}

