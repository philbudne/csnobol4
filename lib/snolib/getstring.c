/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "equ.h"			/* BCDFLD, etc */
#include "snotypes.h"			/* DESCR, etc */
#include "macros.h"			/* D_A() etc */

EXPORT(void)
getstring( vp, dp, len )
    void *vp;				/* pointer to "natural variable" */
    char *dp;
    int len;
{
    int dlen;
    char *sp;

    if (!vp) {				/* null string? */
	*dp = '\0';
	return;
    }
    dlen = D_V(vp);			/* get length from title */
    sp = (char *) vp + BCDFLD;		/* get pointer to string */

    if (dlen > len-1)
	dlen = len-1;

    bcopy( sp, dp, dlen );
    dp[dlen] = '\0';
}

