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

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <stdio.h>			/* rename */

int
RENAME( LA_ALIST ) LA_DCL
{
    char path1[1024];			/* XXX */
    char path2[1024];			/* XXX */

    getstring(LA_PTR(0), path1, sizeof(path1) );
    getstring(LA_PTR(1), path2, sizeof(path2) );

    /* ANSI C, POSIX.1 and XPG3 have rename() */
    if (rename(path2, path1) < 0) {
	RETFAIL;
    }
    RETNULL;
}
