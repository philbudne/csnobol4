/* $Id$ */

/*
 * convert from integer to string using sprintf %lld (in ISO C99)
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

static VAR char strbuf[32];		/* 2^64 is only 21 chars! */

void
intspc(struct spec *sp, struct descr *dp) {
    sprintf( strbuf, "%lld", D_A(dp) );
    S_A(sp) = (int_t) strbuf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(strbuf);
    CLR_S_UNUSED(sp);
}
