/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("FILE2(STRING)STRING")
 * OPSYN("FILE2", "FILE")

 * Usage;	FILE("filename")
 * Returns;	null string or failure
 *
 * Predicate; Checks if the named file exists
 * SITBOL version takes "stream" (a comma seperated list of files)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "lib.h"			/* exists() */

int
FILE2( LA_ALIST ) LA_DCL		/* avoid stdio name collision */
{
    char path[1024];			/* XXX */

    getstring( LA_PTR(0), path, sizeof(path) );
    if (exists(path)) {
	RETNULL;
    }
    RETFAIL;
}
