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

#include "h.h"
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
