/* $Id$ */

/*
 * convert from integer to string
 * generic version (using ato[ld] rather than superior strto[ld])
 */

#include "h.h"
#include "types.h"
#include "macros.h"

#include "equ.h"

void
intspc(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
    static char buf[32];		/* 2^64 is only 21 chars! */

    sprintf( buf, "%d", D_A(dp) );	/* %ld? */
    S_A(sp) = (int_t) buf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(buf);
    CLR_S_UNUSED(sp);
}
