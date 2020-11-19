/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("RENAME(STRING,STRING)STRING")
 *
 * Usage;	RENAME("dest","src)
 * Returns;	null string or failure
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for free() */
#include <stdio.h>			/* rename */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

pmlret_t
RENAME( LA_ALIST ) {
    char *path1;
    char *path2;
    int ret;

    (void) nargs;
    path1 = mgetstring(LA_PTR(0));
    path2 = mgetstring(LA_PTR(1));

    /* ANSI C, POSIX.1 and XPG3 have rename() */
    ret = rename(path2, path1);
    free(path1);
    free(path2);
    if (ret < 0)
	RETFAIL;
    RETNULL;
}
