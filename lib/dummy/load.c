/* $Id$ */

/* dummy functions for LOAD/LINK/UNLOAD */

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    UNDF();
    /* NOTREACHED */
}

int
link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    INTR10();
    /* NOTREACHED */
}

void
unload(sp)
    struct spec *sp;
{
}
