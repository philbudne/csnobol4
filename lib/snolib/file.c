/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("FILE(STRING)STRING")
 *
 * Usage;	FILE("filename")
 * Returns;	null string or failure
 *
 * SITBOL version takes "stream" (a comma seperated list of files)
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <sys/types.h>
#include <sys/stat.h>

int
FILE( LA_ALIST ) LA_DCL
{
    char path[256];			/* XXX */
    struct stat st;

    getstring( LA_PTR(0), path, sizeof(path) );

    if (stat(path, &st) < 0) {
	RETFAIL;
    }
    RETNULL;
}
