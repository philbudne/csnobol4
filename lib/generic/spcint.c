/* $Id$ */

/*
 * convert from string to integer
 *
 * generic version
 * using scanf to detect whether we've matched the whole string.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "equ.h"			/* for I and R */
#include "res.h"
#include "data.h"			/* for SPITCL */

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

#define TC '|'
#define EXTRA 2				/* NUL + terminator */

int
spcint(dp, sp)
    struct descr *dp;
    struct spec *sp;
{
    char buffer[64];			/* ??? */
    int_t len;
    char *cp;
    long l;
    char t;

    len = S_L(sp);
    cp = S_SP(sp);

    if (D_A(SPITCL)) {			/* SPITBOL features? */
	/* strip leading whitespace */
	while (len > 0 && (*cp == ' ' || *cp == '\t')) {
	    cp++;
	    len--;
	}
    }
    
    if (len == 0) {
	D_A(dp) = 0;
	D_F(dp) = 0;			/* clear flags */
	D_V(dp) = I;			/* set type */
	return TRUE;
    }
    
    if (len > sizeof(buffer)-EXTRA)
	len = sizeof(buffer)-EXTRA;
    
    bcopy( cp, buffer, len );
    
    buffer[len++] = TC;
    buffer[len] = '\0';
    if (sscanf(buffer, "%ld%c", &l, &t) != 2 || t != TC)
	return FALSE;
    D_A(dp) = l;
    D_F(dp) = 0;			/* clear flags */
    D_V(dp) = I;			/* set type */
    
    return TRUE;			/* success */
}
