/* $Id$ */

/*
 * convert from integer to string
 * generic version (using ato[ld] rather than superior strto[ld])
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "equ.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#else
static char strbuf[32];			/* 2^64 is only 21 chars! */
#endif

void
intspc(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
    sprintf( strbuf, "%ld", D_A(dp) );	/* %ld? */
    S_A(sp) = (int_t) strbuf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(strbuf);
    CLR_S_UNUSED(sp);
}
