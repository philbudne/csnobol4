/* $Id$ */

/*
 * dummy functions for LOAD/LINK/UNLOAD
 * now allows user to link in functions at build time!
 */

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
#ifdef NO_PML
    UNDF();
    /* NOTREACHED */
#else  /* NO_PML not defined */
    return pml_load(addr, sp1, sp2);
#endif /* NO_PML not defined */
}

int
link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
#ifdef NO_PML
    INTR10();
    /* NOTREACHED */
#else  /* NO_PML not defined */
    return pml_link(retval, args, nargs, addr);
#endif /* NO_PML not defined */
}

void
unload(sp)
    struct spec *sp;
{
}
