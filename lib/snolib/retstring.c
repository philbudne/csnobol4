/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for malloc */
#include <stdio.h>			/* for perror() */

#include "h.h"
#include "equ.h"			/* BCDFLD, etc */
#include "snotypes.h"			/* DESCR, etc */
#include "macros.h"			/* D_A() etc */
#include "load.h"			/* prototypes */
#include "str.h"

static VAR struct spec retspec[1];

SNOLOAD_API(void)
retstring(struct descr *retval, const char *cp, int len) {
#ifdef RETSTRING_STATIC			/* NOTE! not thread safe! */
    char *buf = cp;
#define STYPE L				/* ordinary linked string */
#else
    char *buf = malloc(len);
    memcpy(buf, cp, len);		/* copy to buffer! */
#define STYPE M				/* malloc'ed linked string */
#endif
    /* set up (static) specifier for string */
    S_A(retspec) = (int_t) buf;
    S_F(retspec) = 0;			/* NOTE: *not* a PTR! */
    S_V(retspec) = 0;
    S_O(retspec) = 0;
    S_L(retspec) = len;
    CLR_S_UNUSED(retspec);

    /* point to specifier */
    D_F(retval) = 0;			/* NOTE: not marked as PTR! */
    D_V(retval) = STYPE;		/* "malloc'ed linked string" */
    D_A(retval) = (int_t) retspec;
#ifdef DEBUG_RETSTRING
    printf("retstring buf@%p retspec@%p\n", buf, retspec);
#endif
}

/*
 * return a string which resides in malloc'ed memory using type "M"
 * memory will be freed via a call to relstring (below)
 */
SNOLOAD_API(void)
retstring_free(struct descr *retval, const char *cp, int len) {
    /* set up (static) specifier for string */
    S_A(retspec) = (int_t) cp;
    S_F(retspec) = 0;			/* NOTE: *not* a PTR! */
    S_V(retspec) = 0;
    S_O(retspec) = 0;
    S_L(retspec) = len;
    CLR_S_UNUSED(retspec);

    /* point to specifier */
    D_F(retval) = 0;			/* NOTE: not marked as PTR! */
    D_V(retval) = M;			/* "malloc'ed linked string" */
    D_A(retval) = (int_t) retspec;
#ifdef DEBUG_RETSTRING
    printf("retstring_free buf@%p retspec@%p\n", buf, retspec);
#endif
}

/*
 * Called from LNKFNC after generating variable.  retval
 * SHOULD be pointing at retspec (above), but if someone had
 * their OWN copy of retstring it wouldn't be visible to us,
 * so follow the gourd, er retval.  (Two gourd meanings: the
 * spiritual "follow the drinking gourd", and the market
 * scene in "Life of Brian")
 */
int
relstring(struct descr *retval) {
    struct spec *sptr;
    void *buf;
    if (D_V(retval) != M)
	return 0;
    sptr = D_PTR(retval);
    buf = (void *)S_A(sptr);		/* get buf pointer back */
#ifdef DEBUG_RETSTRING
    printf("relstring buf@%p spec@%p\n", buf, sptr);
#endif
    if (buf)
	free(buf);
    return 1;
}
