/* $Id$ */

/*
 * str.c - string functions
 * 10/27/93
 */

#include "h.h"
#include "types.h"
#include "macros.h"

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

    _S(sp1) = _S(sp2);
    S_L(sp1) = len;
}

