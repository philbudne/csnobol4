/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "syntab.h"

#include "macros.h"
#include "equ.h"
#include "res.h"
#include "data.h"			/* for STYPE */

#ifdef DEBUG
#define DEBUGF(lev, x) if (lev <= DEBUG) printf x
#else  /* DEBUG not defined */
#define DEBUGF(lev, x)
#endif /* DEBUG not defined */

enum stream_ret
stream(
    struct spec *sp1,			/* OUT: prefix */
    struct spec *sp2,			/* IN: string OUT: remainder */
    struct syntab *tp) {
    register unsigned char *cp;
    enum stream_ret ret;
    register int len;
    int_t put;

    len = S_L(sp2);
    cp = (unsigned char *)S_SP(sp2);
    put = 0;				/* XXX in case no puts?? */

    DEBUGF(1,("stream"));
    DEBUGF(10,(" '%*s'", len, cp));
    DEBUGF(1,(" table %s\n", tp->name ));

    for (; len > 0; cp++, len--) {
	register const struct acts *ap;
	unsigned aindex;

	aindex = tp->chrs[*cp];

	DEBUGF(2,(" '%c' (%d)", *cp, *cp ));

	/*
	 * handle CONTIN quickly (95% of time)
	 * always has magic value zero.
	 * 9/9/97
	 */
	if (aindex == 0) {
	    DEBUGF(2,(" CONTIN\n"));
	    continue;
	}

	ap = tp->actions + (aindex - 1);

	/* token can never occur with CONTIN or ERROR? */
	if (ap->put) {
	    put = ap->put;
	    DEBUGF(2,(" put %d", put ));
	}

#ifdef DEBUG
	switch (ap->act) {
	case AC_CONTIN:
	    DEBUGF(2,(" CONTIN\n"));	/* shoudl not happen */
	    break;
	case AC_STOP:
	    DEBUGF(2,(" STOP\n"));
	    break;
	case AC_STOPSH:
	    DEBUGF(2,(" STOPSH\n"));
	    break;
	case AC_ERROR:
	    DEBUGF(2,(" ERROR\n"));
	    break;
	case AC_GOTO:
	    DEBUGF(2,(" goto %s\n", ap->go->name));
	    break;
	}
#endif /* DEBUG defined */

	switch (ap->act) {
	case AC_CONTIN:			/* should not happen */
	    break;
	case AC_STOP:
	    cp++; len--;		/* accept */
	    /* FALL */
	case AC_STOPSH:
	    ret = ST_STOP;
	    goto break_loop;
	case AC_ERROR:
	    D_A(STYPE) = 0;
	    return ST_ERROR;		/* immediate return! */
	case AC_GOTO:
	    tp = ap->go;		/* goto new table */
	    break;
	}
    } /* for */
    /* here when out of subject */
    ret = ST_EOS;

 break_loop:
    D_A(STYPE) = put;
    len = S_L(sp2) - len;		/* get match length */

    _SPEC(sp1) = _SPEC(sp2);		/* copy spec for prefix */
    S_L(sp1) = len;			/* set prefix length */

    if (ret != ST_EOS)
	S_O(sp2) += len;		/* bump suffix offset */

    S_L(sp2) -= len;			/* adjust suffix length */

    return ret;
}

/* new 9/9/97; hide CONTIN crock */
static int
findact(enum action act, struct syntab *tp) {
    const struct acts *ap;
    register int j;

    /* CONTIN is always zero, others one-based */
    if (act == AC_CONTIN)
	return 0;

    /* find action index in list (SNABTB has one of each action type) */
    for (j = 1, ap = tp->actions; ; j++, ap++)
	if (ap->act == act)
	    break;

    return j;
}

/* 10/28/93 */
void
clertb(struct syntab *tp, enum action act) {
    int i, j;
    union {
	long l;
	char c[sizeof(long)];
    } u;
    register long *lp, l;

    j = findact(act, tp);
    /* setup long's worth of chars */
    for (i = 0; i < sizeof(u.c); i++)
	u.c[i] = j;

    /* stamp out long's with unrolled loop */
    l = u.l;
    lp = (long *) tp->chrs;
    i = CHARSET / sizeof(u.c) / 8;
    do {
	/* most ISA's have index+offset; RISC's tend not to have autoinc */
	lp[0] = l;
	lp[1] = l;
	lp[2] = l;
	lp[3] = l;
	lp[4] = l;
	lp[5] = l;
	lp[6] = l;
	lp[7] = l;
	lp += 8;
    } while (--i != 0);
}

/* 10/28/93 */
void
plugtb(struct syntab *tp, enum action act, struct spec *sp) {
    register unsigned char *cp;
    register int len;
    register int j;

    len = S_L(sp);
    cp = (unsigned char *)S_SP(sp);

    j = findact(act, tp);
    while (len > 0) {
	tp->chrs[*cp++] = j;
	len--;
    }
}

/* 8/5/97 [PLB59] */
int
any(struct spec *sp,			/* subject */
    struct descr *dp) {			/* (not)any arg str */
    register unsigned char c, *cp;
    struct descr *vp;
    register int i;

    c = *(unsigned char *)S_SP(sp);	/* get next subject char */
    vp = (struct descr *)D_A(dp);	/* ptr to char set var */
    cp = ((unsigned char *)vp) + BCDFLD; /* ptr to char set */
    i = D_V(vp);			/* length of char set */

    while (i > 0) {
	if (*cp == c)
	    return 1;
	cp++;
	i--;
    }
    return 0;
}
