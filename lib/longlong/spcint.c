/* $Id$ */

/*
 * convert from strings to long long using ad hoc code
 * for environments without long long library support
 * slower than system provided functions
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"
#include "str.h"

#include "equ.h"			/* for I and R */
#include "res.h"
#include "data.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

#include <ctype.h>

int
spcint(dp, sp)
    struct descr *dp;
    struct spec *sp;
{
    int_t len;
    char *cp;
    int_t temp;
    int signum;

    len = S_L(sp);
    cp = S_SP(sp);

    if (D_A(SPITCL)) {			/* SPITBOL features? */
	/* strip leading whitespace */
	while (len > 0 && (*cp == ' ' || *cp == '\t')) {
	    cp++;
	    len--;
	}
    }

    temp = 0;
    signum = 1;
    if (*cp == '-') {
	signum = -1;
	cp++;
    }

    while (len > 0) {
	if (!isdigit(*cp))
	    return FALSE;
	temp *= 10;
	/* could just multiply... nah! */
	if (signum > 0)
	    temp += *cp - '0';
	else
	    temp -= *cp - '0';
	cp++;
	len--;
    }

    D_A(dp) = temp;
    D_F(dp) = 0;			/* clear flags */
    D_V(dp) = I;			/* set type */

    return TRUE;			/* success */
}
