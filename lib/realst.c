/* $Id$ */

/*
 * convert from real to string
 * generic version using "%f"
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "equ.h"

realst(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
    static char buf[32];		/* ??? */

    /* XXX sigh; prints trailing zeroes (but never uses exponent)
     * %g suppresses trailing zeroes (but suppresses DOT and uses exponent)
     */
    sprintf( buf, "%f", D_RV(dp) );
    S_A(sp) = (int_t) buf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(buf);
    CLR_S_UNUSED(sp);
}
