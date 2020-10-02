/* $Id$ */

/*
 * load and run external functions using HP-UX shl_load()
 * -plb 5/22/97
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <dl.h>
#include <stdlib.h>			/* for malloc, getenv */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

void *
os_load_library(const char *path) {
    /* XXX use PROG_HANDLE for null string? */
    return shl_load(path, BIND_DEFERRED|BIND_VERBOSE, 0L);
}

void
os_unload_library(void *lib) {
    shl_unload(lib);
}

void *
os_find_symbol(void *lib, char *func, void **stash) {
    void *entry;

    (void) stash;
    if (shl_findsym(&lib, func, TYPE_PROCEDURE, (void *)&entry) < 0)
	return NULL;
    return entry;
}

void
os_unload_function(const char *name, void *stash) {
    (void) name;
    (void) stash;
}
