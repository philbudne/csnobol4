/*
 * system independant wrapper for LOAD()
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* free() */
#endif

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"			/* mspec2str() */
#include "str.h"

/* support for SIL "LOAD" opcode -- load external function */
int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    char *fname = mspec2str(sp1);
    char *lname = mspec2str(sp2);	/* XXX sub SNOLIB_FILE if empty? */
    int (*entry)(LOAD_PROTO);		/* function entry point */

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
callx(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    int (*entry)(LOAD_PROTO) = (int (*)(LOAD_PROTO))D_A(addr);

    /* XXX check for zero V & F fields?? */
    if (!entry)
	return FALSE;			/* fail (fatal error??) */

    return (entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}
