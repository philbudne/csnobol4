/* $Id$ */

/* dummy functions for EXPINT/EXREAL
 * (attempts to execute code using exponentiation will
 * cause fatal errors (as specified in SIL reference))
 */

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
