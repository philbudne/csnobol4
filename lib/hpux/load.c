/* $Id$ */

/*
 * load and run external functions using HP-UX shl_load()
 * -plb 5/22/97
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>
#include <dl.h>
#include <stdlib.h>			/* for malloc, getenv */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"


void *
os_load_library(const char *path) {
    /* XXX use PROG_HANDLE for null string? */
    return shl_load(path, BIND_DEFERRED|BIND_VERBOSE, 0L);
}

    if (handle == NULL)
	return NULL;

    /* see if it's one we've already mapped */
    for (lp = libs; lp; lp = lp->next) {
	if (lp->handle == handle) {
	    lp->refcount++;
	    return lp;
	}
    }

    /* a new one; add to list */
    lp = (struct lib *) malloc(sizeof(struct lib));
    lp->next = libs;
    lp->handle = handle;
    lp->refcount = 1;

    libs = lp;
    return lp;
}

void
os_unload_library(void *lib) {
    shl_unload(lib);
}

void *
os_find_symbol(void *lib, char *func) {
    void *entry;

    if (shl_findsym(&lib, func, TYPE_PROCEDURE, (void *)&entry) < 0)
	return NULL;
    return entry;
}
