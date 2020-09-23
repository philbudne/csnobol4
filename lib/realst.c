/* $Id$ */

/*
 * convert from real to string
 * generic version using "%g"
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <ctype.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"
#include "str.h"

#include "equ.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
static char strbuf[64];
#endif /* NO_STATIC_VARS not defined */

#ifndef REALST_FORMAT
/* "l" is always redundant? */
#define REALST_FORMAT "%lg"
#endif

void
realst(sp, dp)
    struct spec *sp;
    struct descr *dp;
{
    char *bp;

    sprintf( strbuf, REALST_FORMAT, D_RV(dp) );
    /*
     * "g" format can print an integer for exact values.
     * make sure we have an exponent or a dot.
     *
     * %#g may add dot, but also trailing zeroes!
     */
    bp = strbuf;
    while (*bp && isdigit((unsigned char)*bp))
	bp++;

    if (*bp == '\0') {
	/* reached end of string. add trailing dot */
	*bp++ = '.';
	*bp = '\0';
    }

    S_A(sp) = (int_t) strbuf;		/* OY! */
    S_F(sp) = 0;
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(strbuf);
    CLR_S_UNUSED(sp);
}
