/* $Id$ */

/* dummy functions for EXPINT/EXREAL */

int
expint(res,x,y)
    struct descr *res, *x, *y;
{
    UNDF();
}

int
exreal(res,x,y)
    struct descr *res, *x, *y;
{
    INTR10();
}
