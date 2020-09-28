/*
 * system independant wrapper for LOAD()
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>			/* free() */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"			/* mspec2str() */
#include "str.h"

/* support for SIL "LOAD" opcode -- load external function */
int
load(struct descr *addr,		/* OUT */
     struct spec *sp1,			/* function */
     struct spec *sp2) {		/* library */
    char *fname = mspec2str(sp1);
    char *lname = mspec2str(sp2);	/* XXX sub SNOLIB_FILE if empty? */
    loadable_func_t *entry;		/* function address */

    /* always try PML first? Only if lname is empty?? */
    entry = pml_find(fname);
    if (!entry) {
	entry = os_load(fname, lname);

	if (!entry) {
	    char *l2 = strjoin(lname, DL_EXT, NULL);
	    entry = os_load(fname, l2);
	    free(l2);
	}

	if (!entry && !abspath(lname)) {
	    char *path = io_lib_find("shared", lname, DL_EXT);
	    if (!path)
		path = io_lib_find("dynload", lname, DL_EXT);
	    if (path) {
		entry = os_load(fname, path);
		free(path);
	    }
	    if (!entry) {
		path = io_lib_find(NULL, lname, DL_EXT);
		if (path) {
		    entry = os_load(fname, path);
		    free(path);
		}
	    }
	}
    }
    free(fname);
    free(lname);
    if (entry) {
	D_A(addr) = (int_t) entry;
	D_F(addr) = D_V(addr) = 0;	/* clear flags, type */
	return TRUE;
    }
    return FALSE;
}

/* support for SIL "LINK" opcode -- call external function */
int
callx(struct descr *retval, struct descr *args,
      struct descr *nargs, struct descr *addr) {
    loadable_func_t *entry = (loadable_func_t *)D_A(addr);
    /* XXX check for zero V & F fields?? */
    if (!entry)
	return FALSE;			/* fail (fatal error??) */

    return (entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}
