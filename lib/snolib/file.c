/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("FILE(STRING)STRING")
 *
 * Usage;	FILE("filename")
 * Returns;	null string or failure
 *
 * Predicate; Checks if the named file exists
 * SITBOL version takes "stream" (a comma seperated list of files)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

int
FILE( LA_ALIST ) LA_DCL
{
    char path[1024];			/* XXX */

    getstring( LA_PTR(0), path, sizeof(path) );
    if (exists(path)) {
	RETNULL;
    }
    RETFAIL;
}
