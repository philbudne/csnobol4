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
    static char *retbuf;
    static int retbuflen;

    if (len > retbuflen) {
	if (retbuf)
	    free(retbuf);
	retbuf = malloc(len);
	if (!retbuf) {
	    perror("retstring malloc");
	    exit(1);
	}
	retbuflen = len;
    }

    bcopy( cp, retbuf, len );		/* copy to buffer! */

    /* set up (static) specifier for string */
    S_A(sp) = (int_t) retbuf;
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = len;
    CLR_S_UNUSED(sp);

    /* point to specifier */
    D_F(retval) = 0;			/* NOTE: not marked as PTR! */
    D_V(retval) = L;			/* "linked string" */
    D_A(retval) = (int_t) sp;

}

