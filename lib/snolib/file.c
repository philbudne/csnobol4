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
#include <stdlib.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "lib.h"			/* exists() */
#include "str.h"			/* RETSTR_FREE */
#include "pmlproto.h"			/* for prototype */

pmlret_t
FILE2( LA_ALIST ) {			/* avoid stdio name collision */
    char *path = mgetstring(LA_PTR(0));
    int succ = exists(path);
    (void) nargs;
    free(path);
    RETPRED(succ);
}

pmlret_t
FILE_ISDIR( LA_ALIST ) {
    char *path = mgetstring(LA_PTR(0));
    int succ = isdir(path);
    (void) nargs;
    free(path);
    RETPRED(succ);
}

pmlret_t
FILE_ABSPATH( LA_ALIST ) {
    char *path = mgetstring(LA_PTR(0));
    int succ = abspath(path);
    (void) nargs;
    free(path);
    RETPRED(succ);
}

/* 2020-10-21 for setuputil.sno (before stat module available!) */
pmlret_t
FILE_NEWER( LA_ALIST ) {
    char *p1 = mgetstring(LA_PTR(0));
    char *p2 = mgetstring(LA_PTR(1));
    int ret = newer(p1, p2);
    (void) nargs;
    free(p1);
    free(p2);
    RETINT(ret);
}

/* 2020-12-23 for setuputil.sno (before stat module available!) */
pmlret_t
FILE_LIB_FIND( LA_ALIST ) {
    char *fname = mgetstring(LA_PTR(1));

    (void) nargs;

    if (abspath(fname)) {
	RETSTR_FREE(fname);
    }
    else {
	char *dir = nmgetstring(LA_PTR(0));
	char *ext = nmgetstring(LA_PTR(2));
	char *ret = io_lib_find(dir, fname, ext);
	if (dir)
	    free(dir);
	if (ext)
	    free(ext);
	if (ret)
	    RETSTR_FREE(ret);
	RETFAIL;
    }
}
