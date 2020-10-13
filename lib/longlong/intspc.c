/* $Id$ */

/*
 * long long to string (systems without long long library support)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"
#include "str.h"

#include "equ.h"

static VAR char strbuf[32];	      /* 2^64 is only 21 chars! */

void
intspc(struct spec *sp, struct descr *dp) {
    char *bp;
    int_t x;
    unsigned INT_T u;

    bp = strbuf + sizeof(strbuf);
    *--bp = '\0';

    x = D_A(dp);
    if (x < 0)
	u = -x;
    else
	u = x;

    while (u >= 10) {
	*--bp = (u % 10) + '0';
	u /= 10;
    }
    *--bp = u + '0';

    if (x < 0)
	*--bp = '-';

    S_A(sp) = (int_t) bp;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(bp);
    CLR_S_UNUSED(sp);
}
