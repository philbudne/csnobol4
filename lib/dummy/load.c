/* $Id$ */

/*
 * dummy functions for LOAD/LINK/UNLOAD
 * now allows user to link in functions at build time!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

extern int (*pml_find())(LOAD_PROTO);

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
#ifdef NO_PML
    UNDF();
    /* NOTREACHED */
#else  /* NO_PML not defined */
    char name[256];			/* XXX */

    spec2str( sp1, name, sizeof(name) );
    D_A(addr) = (int_t) pml_find(name);
    if (D_A(addr) == NULL)
	return FALSE;

    D_F(addr) = D_V(addr) = 0;		/* clear flags, type */
    return TRUE;
#endif
} /* pml_load */

int
link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
#ifdef NO_PML
    INTR10();
    /* NOTREACHED */
#else  /* NO_PML not defined */
    int (*func)(LOAD_PROTO);

    /* XXX check for zero V & F fields?? */
    func = (int (*)(LOAD_PROTO)) D_A(addr);
    if (func == NULL)
	return FALSE;

    return (func)( retval, D_A(nargs), (struct descr *)D_A(args) );
#endif
}

void
unload(sp)
    struct spec *sp;
{
}
