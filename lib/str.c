/* $Id$ */

/*
 * str.c - string functions
 * 10/27/93
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include <ctype.h>

void
trimsp( sp1, sp2 )
    struct spec *sp1, *sp2;
{
    char *cp;
    int len;

    len = S_L(sp2);
    cp = S_SP(sp2);

    while (len > 0 && cp[len-1] == ' ')
	len--;

    _SPEC(sp1) = _SPEC(sp2);
    S_L(sp1) = len;
}

void
raise( sp )
    struct spec *sp;
{
    register char *cp;
    register int len;

    len = S_L(sp);
    cp = S_SP(sp);

    while (len-- > 0) {
	if (islower(*cp))
	    *cp = toupper(*cp);
	cp++;
    }
}

/* 8/19/96 */
int
raise2( sp1, sp2 )
    struct spec *sp1, *sp2;
{
    register char *sp, *dp;
    register int len;
    register int raised;

    len = S_L(sp1);
    sp = S_SP(sp1);
    dp = S_SP(sp2);
    raised = 0;

    while (len-- > 0) {
	if (islower(*sp)) {
	    *dp++ = toupper(*sp);
	    sp++;
	    raised++;
	}
	else
	    *dp++ = *sp++;
    }
    return raised > 0;
}


