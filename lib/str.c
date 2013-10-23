/* $Id$ */

/*
 * str.c - string functions
 * 10/27/93
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <ctype.h>
#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"			/* for prototypes */
#include "str.h"

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

    bcopy(S_SP(subj), dp, (long)slen);	/* XXX SIZE_T */
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

/* 2/15/2012 */
/* copy from specifier to c-string */
char *
mspec2str(sp)
    struct spec *sp;
{
    int l = S_L(sp) + 1;
    char *str = malloc(l);
    if (str)
	spec2str(sp, str, l);
    return str;
}

/*  6/12/98 */

/*#define APDSP_NLENS 4096*/
#ifdef APDSP_NLENS
int apdsp_lens[APDSP_NLENS];
#endif /* APDSP_NLENS defined */

void
apdsp(base, str)
    struct spec *base, *str;
{
    register int len;
    register char *src, *dst;

    len = S_L(str);
    src = S_SP(str);
    dst = S_SP(base)+S_L(base);

#ifdef APDSP_NLENS
    if (len > APDSP_NLENS)
	apdsp_lens[APDSP_NLENS-1]++;
    else
	apdsp_lens[len]++;
#endif /* APDSP_NLENS defined */

    S_L(base) += len;

#define THRESH 4
    if (len >= THRESH) {		/* XXX also check alignment? */
	/* XXX non-overlap version here? */
	bcopy(src, dst, (long)len);	/* XXX SIZE_T */
    }
    else {
	while (len > 0) {
	    *dst++ = *src++;
	    len--;
	}
    }
}

/* added 3/4/2012 */
char *
strjoin
#ifdef __STDC__
	(char *str0, ...)
#else
	(va_alist) va_dcl
#endif
{
    va_list vp;
    int len;
    char *str, *tp;

#ifdef __STDC__
    va_start(vp, str0);
#else  /* __STDC__ not defined */
    char *str0;
    va_start(vp);

    str0 = va_arg(vp, char *);
#endif /* __STDC__ not defined */
    len = strlen(str0) + 1;
    while ((tp = va_arg(vp, char *)))
	len += strlen(tp);
    va_end(vp);

    str = malloc(len);
    if (!str)
	return NULL;

#ifdef __STDC__
    va_start(vp, str0);
#else  /* __STDC__ not defined */
    va_start(vp);
    str0 = va_arg(vp, char *);
#endif
    strcpy(str, str0);
    while ((tp = va_arg(vp, char *)))
	strcat(str, tp);
    va_end(vp);
    return str;
}

#ifdef BLOCKS
/* for BLOCKS; translated from the BAL macro 9/26/2013 */
/*
#define DEBUG_MERGSP
#define DEBUG_MERGSP2
*/
#ifdef DEBUG_MERGSP2
#define DUMP(S,L) dump(#S, S, L)
static void
dump(n, s, l)
     char *n, *s;
     int l;
{
    fprintf(stderr, "%-4s '", n);
    while (l-- > 0) {
	char c = *s++;
	if (c == '\0') c = '~';
	fputc(c, stderr);
    }
    fprintf(stderr, "'\n");
}
#else
#define DUMP(S,L)
#endif

void
mergsp(sp1, sp2, sp3)
    struct spec *sp1, *sp2, *sp3;
{
    int len = S_L(sp2);			/* GR1 "length" */
    char *dest = S_SP(sp1);		/* GR2 "first string" */
    char *src = S_SP(sp2);		/* GR3 "2nd string" */
    char bg = *S_SP(sp3);		/* GR4 "background" */
    char c;				/* GR5 */
#ifdef DEBUG_MERGSP
    fprintf(stderr, "mergsp len %d bg '%c'\n", len, bg);
#endif
    if (len < 1)
	return;
    DUMP(src, len);
    DUMP(dest, len);
    do {
	--len;
	c = src[len];
	if (c != bg)
	    dest[len] = c;
    } while (len > 0);
    DUMP(dest, S_L(sp2));
}

/* 10/11/2013 BLOCKS BLAND routine requires BLT-like behavior! */
void
movblk2(d1, d2, len)
    struct descr *d1, *d2;
    int_t len;
{
    while (len > 0) {
	*++d1 = *++d2;
	len -= DESCR;
    }
}
#endif /* BLOCKS */
