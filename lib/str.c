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
    register char *cp;
    register int len;

    len = S_L(sp2);
    cp = S_SP(sp2) + len - 1;

    while (len > 0 && isspace(*cp)) {
	len--;
	cp--;
    }

    _SPEC(sp1) = _SPEC(sp2);
    S_L(sp1) = len;
}

void
raise1( sp )
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

/* 8/19/96 -- for GENVUP/VPXPTR */
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

/* support for LPAD/RPAD 8/26/96 */
int
pad(dir,out,subj,pad)
    struct descr *dir;			/* LPAD=0,RPAD=1 */
    struct spec *out, *subj, *pad;
{
    int npad, slen;
    char *dp;
    char pc;

    slen = S_L(subj);
    npad = S_L(out) - slen;
    dp = S_SP(out);

    if (S_L(pad) == 0)			/* null string */
	pc = ' ';			/* default to space */
    else
	pc = *S_SP(pad);

    if (D_A(dir) == 0) {		/* LPAD */
	while (npad-- > 0)
	    *dp++ = pc;
    }

    bcopy(S_SP(subj), dp, slen);
    dp += slen;

    if (D_A(dir) != 0) {		/* RPAD */
	while (npad-- > 0)
	    *dp++ = pc;
    }

    return 0;
}

/* support for REVERSE 9/18/96 */
int
reverse(dest, src)
    struct spec *dest, *src;
{
    register char *sp, *dp;
    register int len;

    len = S_L(src);
    sp = S_SP(src) + len;
    dp = S_SP(dest);

    while (len--) {
	*dp++ =  *--sp;
    }
    return 0;
}

/* support for SUBSTR 9/18/96 */
int
substr(dest, src, pos)
    struct spec *dest, *src;
    struct descr *pos;
{
    register char *sp, *dp;
    register int len;

    sp = S_SP(src) + D_A(pos);
    dp = S_SP(dest);
    len = S_L(dest);

    while (len--) {
	*dp++ =  *sp++;
    }
    return 0;
}

/* copy from specifier to c-string */
void
spec2str(sp, dest, size)
    struct spec *sp;
    char *dest;
    int size;
{
    int l;

    l = S_L(sp);
    if (l > size-1)
	l = size-1;

    strncpy(dest, S_SP(sp), l);
    dest[l] = '\0';
}

