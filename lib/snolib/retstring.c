/* $Id$ */

#include "h.h"
#include "equ.h"			/* BCDFLD, etc */
#include "snotypes.h"			/* DESCR, etc */
#include "macros.h"			/* D_A() etc */

void
retstring( retval, cp, len )
    struct descr *retval;
    char *cp;
    int len;
{
    static struct spec sp[1];
    static char buffer[512];		/* XXX */

    bcopy( cp, buffer, len );		/* copy to static buffer?! */

    /* set up (static) specifier for string */
    bzero( (char *)sp, sizeof(sp) );
    S_L(sp) = len;
    S_O(sp) = 0;
    S_A(sp) = (int_t) buffer;

    /* point to specifier */
    D_F(retval) = 0;			/* NOTE: not marked as PTR! */
    D_V(retval) = L;			/* "linked string" */
    D_A(retval) = (int_t) sp;

}

