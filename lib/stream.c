/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "libret.h"
#include "syntab.h"

#include "macros.h"
#include "data.h"			/* for STYPE */

#ifdef DEBUG
#define DEBUGF(lev, x) if (lev <= DEBUG) printf x
#else  /* DEBUG not defined */
#define DEBUGF(lev, x)
#endif /* DEBUG not defined */


#define ACCEPT() { cp++; len--; }

enum stream_ret
stream( sp1, sp2, tp )
    struct spec *sp1;			/* OUT: prefix */
    struct spec *sp2;			/* IN: string OUT: remainder */
    struct syntab *tp;
{
    char *cp;
    int len;
    enum stream_ret ret;

    len = S_L(sp2);
    cp = S_SP(sp2);
    D_A(STYPE) = 0;			/* XXX in case no puts?? */

    DEBUGF(1,("stream"));
    DEBUGF(10,(" '%*s'", len, cp));
    DEBUGF(1,(" table %s\n", tp->name ));

    while (len > 0) {
	struct acts *ap;
	int tok;

	ap = tp->actions + tp->chrs[*cp];

	DEBUGF(2,(" '%c' (%d)", *cp, *cp ));

	/* token can never occur with CONTIN or ERROR? */
	tok = ap->put;
	if (tok) {
	    DEBUGF(2,(" put %d", tok ));
	    D_A(STYPE) = tok;
	}

#ifdef DEBUG
	switch (ap->act) {
	case AC_CONTIN:
	    DEBUGF(2,(" CONTIN\n"));
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
	case AC_CONTIN:
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
	    tp = ap->go;
	    break;
	}
	cp++; len--;			/* accept */
    }
    /* here when out of subject */
    ret = ST_EOS;

 break_loop:
    len = S_L(sp2) - len;		/* get match length */

    _SPEC(sp1) = _SPEC(sp2);		/* copy spec for prefix */
    S_L(sp1) = len;			/* set prefix length */

    if (ret != ST_EOS)
	S_O(sp2) += len;		/* bump suffix offset */

    S_L(sp2) -= len;			/* adjust suffix length */

    return ret;
}

/* 10/28/93 */
void
clertb(tp, act, sp)
    struct syntab *tp;
    enum action act;
{
    int i, j;

    /* find action index in list (SNABTB has one of each action type) */
    for (j = 0; ; j++)
	if (tp->actions[j].act == act)
	    break;

    for (i = 0; i < CHARSET; i++)
	tp->chrs[i] = j;
}

/* 10/28/93 */
void
plugtb(tp, act, sp)
    struct syntab *tp;
    enum action act;
    struct spec *sp;
{
    char *cp;
    int len;
    int j;

    len = S_L(sp);
    cp = S_SP(sp);

    /* find action index in list (SNABTB has one of each action type) */
    for (j = 0; ; j++)
	if (tp->actions[j].act == act)
	    break;

    while (len-- > 0) {
	tp->chrs[*cp++] = j;
    }
}

