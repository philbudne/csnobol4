/* $Id$ */

/*
 * convert from real to string
 * generic version using "%g"
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "equ.h"

#include <ctype.h>
#include <stdio.h>

void
realst(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
    static char buf[64];		/* ??? */
    char *bp;

    sprintf( buf, "%lg", D_RV(dp) );
    /*
     * "g" format can print an integer for exact values.
     * make sure we have an exponent or a dot.
     */
    bp = buf;
    while (*bp && isdigit(*bp))
	bp++;

    if (*bp == '\0') {
	/* reached end of string. add trailing dot */
	*bp++ = '.';
	*bp = '\0';
    }

    S_A(sp) = (int_t) buf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(buf);
    CLR_S_UNUSED(sp);
}
