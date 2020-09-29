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
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"			/* spec2str() */
#include "str.h"

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)

#define MAXSTR 512			/* XXX */

loadable_func_t *
os_load(char *function, char *file) {
    loadable_func_t *entry = NULL;
    int status;
    struct vms_descr {
	int len;
	char *ptr;
    } dfile, dsym;

    dfile.len = strlen(file);
    dfile.ptr = file;

    dsym.len = strlen(function);
    dsym.ptr = function;

    /* XXX LIB$ESTABLISH() "signal" handler??? */

    /* XXX pass LIB$M_FIS_MIXEDCASE flag? */
    status = LIB$FIND_IMAGE_SYMBOL(&dfile, &dsym, &entry);
    if (SUCCESS(status) && entry)
	return entry;

    return NULL;
}

void
unload(struct spec *sp) {
    /* can this be done?  need to keep track of items in use!! */
}
