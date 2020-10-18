/* $Id$ */

/*
 * load and run external functions for VMS
 * doesn't try to deal with deactivation (can it be done?)
 * compiles, but not tested
 * -plb 5/20/2003
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <lib$routines.h>
#include <stsdef.h>

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"			/* prototypes */
#include "str.h"			/* strdup, strlen */

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)

#define MAXSTR 512			/* XXX */

void *
os_load_library(const char *file) {
    return strdup(file);
}

void *
os_find_symbol(void *lib, const char *function, void **stash) {
    struct vms_descr {
	int len;
	char *ptr;
    } dfile, dsym;
    void *value;

    dfile.len = strlen(lib);		/* string! */
    dfile.ptr = (char *)lib;

    dsym.len = strlen(function);
    dsym.ptr = (char *)function;

    /* XXX LIB$ESTABLISH() "signal" handler??? */

    /* XXX pass LIB$M_FIS_MIXEDCASE flag? */
    status = LIB$FIND_IMAGE_SYMBOL(&dfile, &dsym, &value);
    if (SUCCESS(status) && value)
	return value;

    return NULL;
}

void
os_unload_function(const char *name, void *stash) {
    (void) name;
    (void) stash;
}

void
os_unload_library(void *lib) {
    /* can this be done?  need to keep track of items in use!! */
    free(lib);				/* strdup'ed name */
}
