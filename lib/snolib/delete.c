/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("DELETE(STRING)STRING")
 *
 * Usage;	DELETE("filename")
 * Returns;	null string or failure
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* unlink() */
#endif /* HAVE_UNISTD_H defined */

#include <stdlib.h>			/* for free() */

#ifdef UNLINK_IN_STDIO_H		/* Windoze! */
#include <stdio.h>
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

pmlret_t
DELETE( LA_ALIST ) {
    char *path;
    int ret;

    (void) nargs;
    path = mgetstring(LA_PTR(0));
    ret = unlink(path);
    free(path);
    if (ret < 0)
	RETFAIL;
    RETNULL;
}
