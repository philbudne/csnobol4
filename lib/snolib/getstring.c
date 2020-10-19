/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* malloc */

#include "h.h"
#include "equ.h"			/* BCDFLD */
#include "snotypes.h"			/* DESCR */
#include "macros.h"			/* D_V() etc */
#include "load.h"			/* prototypes */
#include "str.h"			/* memcpy */

SNOLOAD_API(void)
getstring(const void *vp,		/* pointer to "natural variable" */
	  char *dp,
	  int len) {
    size_t dlen;
    char *sp;

    if (!vp) {				/* null string? */
	*dp = '\0';
	return;
    }
    dlen = D_V(vp);			/* get length from title */
    sp = (char *) vp + BCDFLD;		/* get pointer to string */

    if (dlen > len-1)
	dlen = len-1;

    memcpy(dp, sp, dlen);
    dp[dlen] = '\0';
}

/* perform malloc, getstring */
SNOLOAD_API(char *)
mgetstring(const void *vp) {		/* pointer to "natural variable" */
    char *cp;
    int len;

    if (vp)
	len = D_V(vp);
    else
	len = 0;

    len++;
    cp = malloc(len);
    if (!cp)
	return NULL;

    getstring(vp, cp, len);

    return cp;
}
