/* $Id$ */

/*
 * load and run external functions for systems using dlopen()/dlsym()
 * -plb 4/13/97
 *
 * called by loadx.c 2/15/2012
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* sprintf() */
#include <dlfcn.h>
#include <stdlib.h>			/* malloc(), getenv() */

#include "h.h"
#include "snotypes.h"
#include "lib.h"			/* spec2str() */
#include "str.h"

#ifndef RTLD_LAZY
#define RTLD_LAZY 0			/* Needed on FreeBSD 2.2.1-RELEASE */
#endif /* RTLD_LAZY not defined */

/* called from loadx.c */
void *
os_load_library(const char *lname) {
    /*
     * SunOS4 (and others) only support LAZY mode.
     * RTLD_GLOBAL could cause collisions between modules??
     */
    if (!*lname) {
	/* lookup in self */
	return dlopen(NULL, RTLD_LAZY);
    }

    /* if lname doesn't have a DIR_SEP, prepend ./ */
    if (strchr(lname, DIR_SEP[0]) == 0) {
	char *path = strjoin(".", DIR_SEP, lname, NULL);
	void *osval = dlopen(path, RTLD_LAZY);
	free(path);
	return osval;
    }

    return dlopen(lname, RTLD_LAZY);
}

void
os_unload_library(void *osval) {
    dlclose(osval);
}

void *
os_find_symbol(void *osval, const char *function, void **stash) {
    (void) stash;
    return dlsym(osval, function);
}

void
os_unload_function(const char *name, void *stash) {
    (void) name;
    (void) stash;
}
