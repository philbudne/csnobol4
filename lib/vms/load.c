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

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

#define MAXSTR 512			/* XXX */

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    char symbol[MAXSTR];
    int (*entry)(LOAD_PROTO);

    spec2str(sp1, symbol, sizeof(symbol));
    entry = pml_find(symbol);
    if (entry != NULL) {		/* not found by pml */
	D_A(addr) = (int_t) entry;
	D_F(addr) = D_V(addr) = 0;	/* clear flags, type */
	return TRUE;
    } /* found by PML */
    else {				/* not found by PML */
	char file[MAXSTR];
	int status, value;
	struct vms_descr {
	    int len;
	    char *ptr;
	} dfile, dsym;

	dsym.len = strlen(symbol);
	dsym.ptr = symbol;

	spec2str(sp2, file, sizeof(file));
	dfile.len = strlen(file);
	dfile.ptr = file;

	value = 0;

	/* XXX LIB$ESTABLISH() "signal" handler??? */

	/* XXX pass LIB$M_FIS_MIXEDCASE flag? */
	status = LIB$FIND_IMAGE_SYMBOL(&dfile, &dsym, &value);
	if (SUCCESS(status) && value != 0) {
	    D_A(addr) = value;
	    D_F(addr) = D_V(addr) = 0;	/* clear flags, type */
	    return TRUE;
	}
    } /* not found by pml */

    /* not found by PML or FIND_IMAGE_SYMBOL */
    return FALSE;
}

/* support for SIL "LINK" opcode -- call external function */
int
callx(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    int (*entry)(LOAD_PROTO);

    /* XXX check for zero V & F fields?? */

    /* XXX no validation!! */
    entry = (int (*)(LOAD_PROTO)) D_A(addr);
    if (entry == NULL)
	return FALSE;

    return (entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}

void
unload(sp)
    struct spec *sp;
{
    /* can this be done?  need to keep track of items in use!! */
}
