/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio, h.h */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>			/* for perror() */

#include "h.h"
#include "equ.h"			/* BCDFLD, etc */
#include "snotypes.h"			/* DESCR, etc */
#include "macros.h"			/* D_A() etc */
#include "load.h"			/* prototypes */
#include "str.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#else
static struct spec retspec[1];
static char *retbuf;
static int retbuflen;
#endif

EXPORT(void)
retstring( retval, cp, len )
    struct descr *retval;
    const char *cp;
    int len;
{

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
    S_A(retspec) = (int_t) retbuf;
    S_F(retspec) = 0;			/* NOTE: *not* a PTR! */
    S_V(retspec) = 0;
    S_O(retspec) = 0;
    S_L(retspec) = len;
    CLR_S_UNUSED(retspec);

    /* point to specifier */
    D_F(retval) = 0;			/* NOTE: not marked as PTR! */
    D_V(retval) = L;			/* "linked string" */
    D_A(retval) = (int_t) retspec;

}

