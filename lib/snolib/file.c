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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
extern void *malloc();
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "lib.h"			/* exists() */

int
FILE2( LA_ALIST ) LA_DCL		/* avoid stdio name collision */
{
    char *path = mgetstring(LA_PTR(0));
    int succ = exists(path);
    free(path);
    RETPRED(succ);
}

int
FILE_ISDIR( LA_ALIST ) LA_DCL
{
    char *path = mgetstring(LA_PTR(0));
    int succ = isdir(path);
    free(path);
    RETPRED(succ);
}

int
FILE_ABSPATH( LA_ALIST ) LA_DCL
{
    char *path = mgetstring(LA_PTR(0));
    int succ = abspath(path);
    free(path);
    RETPRED(succ);
}
