/* $Id$ */

/* debug functions to call from gdb! */

/*
 * TODO:
 * pspec()
 *
 * phil 8/97;
 * now uses "dt.h", io_printf()
 * XXX fix ptable()
 * XXX just use UNITP for output unit?
 */

/*#define DUMP				/* TEMP */

#ifdef DUMP
#include <stdio.h>

#include "snotypes.h"
#include "equ.h"
#include "res.h"
#include "data.h"
#include "macros.h"
#include "dt.h"

#define NULLVAL(VP) ((VP)->v == S && (VP)->a.ptr == NULL)

void parray();

void
more() {
    static count;
    char buf[128];

    if (count++ < 20)			/* XXX */
	return;

    count = 0;
    gets(buf);
}

void
pdescr(dp)
    struct descr *dp;
{
    int unit;

    unit = res.punch[0].a.i;

#ifdef DEBUG
    printf("v %d f ", D_V(dp) );
    if (D_F(dp) == 0) {
	putchar('0');
    }
    else {
	putchar('<');
	if (D_F(dp) & FNC)
	    putchar('F');
	if (D_F(dp) & TTL)
	    putchar('T');
	if (D_F(dp) & STTL)
	    putchar('S');
	if (D_F(dp) & MARK)
	    putchar('M');
	if (D_F(dp) & PTR)
	    putchar('P');
	putchar('>');
    }
    printf(" a %#x (%d)", D_A(dp), D_A(dp) );
#endif /* DEBUG defined */

    /* XXX just use default case for all but A, I, R, T? */
    switch (dp->v) {
    case A:
	parray(dp->a.ptr, 0);
	break;
    case B:				/* internal block */
	io_printf(unit, "BLOCK");
	break;
    case C:
	io_printf(unit, "CODE");
	break;
    case E:
	io_printf(unit, "EXPRESSION");
	break;
    case I:
	io_printf(unit, "%d", dp);
	break;
    case K:
	io_printf(unit, "KEYWORD");
	break;
    case L:
	/* pointer to spec */
	io_printf(unit, "LSTRING"); /* should not happen */
	break;
    case N:
	io_printf(unit, "NAME"); /* array, table element */
	break;
    case P:
	io_printf(unit, "PATTERN");
	break;
    case R:
	/* XXX use realst() and %S?? */
	io_printf(unit, "%F", dp);	/* XXX fixme */
	break;
    case S:
	/* XXX check for excessive length? */
	io_printf(unit, "'%v'", dp->a.ptr);
	break;
    case T:
	io_printf(unit, "TABLE(x,y)");	/* XXX get values */
	/* XXX save ptr if new, output index */
	/* XXX check FRZN flag */
	break;
    default:
	/* locate datatype name in DTATL table */
	{
	    int n, i;
	    struct pairblock *b;

	    /* XXX use locapv function? */
	    b = (struct pairblock *) res.dtatl[0].a.ptr;
	    n = b->title.v / 2 / DESCR;
	    for (i = 0; i < n; i++) {
		if (dp->v == b->pairs[i].value.v) {
		    io_printf(unit, "%v", b->pairs[i].type.a.ptr);
		    /* XXX save ptr if new, output index "#n" */
		    return;
		}
	    }
	}
	io_printf(unit, "EXTERNAL");
	break;
    }
}

void
dump_dyn() {
    int a;

    a = D_A(HDSGPT);
    while (a < D_A(FRSGPT)) {
	struct descr *dp;

	dp = (struct descr *) a;

	printf("%#x: ", a);
	pdescr(dp);
	putchar('\n');
	more();

	if (!(D_F(dp) & TTL)) {
	    puts("NO TITLE.");
	    break;
	}

	a += X_BKSIZE(dp);		/* XXX not tested */
    }
}

/* dump all strings/names */
void
dump_vars() {
    int i;
    int unit;

    unit = res.punch[0].a.i;
    for (i = 0; i < OBSIZ; i++) {
	struct nv *vp;

	/* run thru each chain */
	for (vp = (struct nv *)res.obstrt[i].a.ptr;
	     vp != NULL;
	     vp = (struct nv *) vp->lnkfld.a.ptr) {
#if 0
	    if (vp->value.v == 0 && vp->value.f == 0 && vp->value.a.i == 0)
		continue;
#endif
	    /* ignore null-valued variables */
	    if (NULLVAL(&vp->value))
		continue;

	    io_printf(unit, "%v = ", vp);
	    pdescr(&vp->value);
	    io_printf(unit, "\n");
	    more();
	}
    }
    /* XXX if arg > 1 dump all all saved objects (list may grow while
     * traversing it!)
     */
}

void
ptable(dp)
    struct descr *dp;
{
    int d;

#ifdef DEBUG
    if (!(D_F(dp) & TTL)) {		/* XXX check self ptr? */
	puts("no title");
	fflush(stdout);
	return;
    }
#endif /* DEBUG defined */

    d = D_V(dp) / DESCR;
    printf("initial size %d (%d entries)\n", d, d/2-1);
    /* XXX dump entries?? */
    dp = (struct descr *)dp[d].a.ptr;	/* XXX D_PTR? */
    while (dp != (struct descr *)1) {
	d = D_V(dp) / DESCR;
	printf("extent size %d (%d entries)\n", d, d/2-1);
	/* XXX dump entries?? */
	dp = (struct descr *)dp[d].a.ptr; /* XXX D_PTR? */
    }
    fflush(stdout);
}

void
parray(ap,elements)
    struct array *ap;
    int elements;
{
    int s, n;
    int unit;

    unit = res.punch[0].a.i;

#ifdef DEBUG
    if (!(ap->title.f & TTL)) {		/* XXX check self ptr? */
	puts("no title");
	fflush(stdout);
	return;
    }
#endif /* DEBUG defined */

    io_printf(unit, "ARRAY(%v)", ap->prototype.a.ptr );
    /* XXX save ptr if new, output index "#n" */

    if (!elements)
	return;

    n = ap->ndim.a.i;
    s = ap->title.v/DESCR - (n+2);

#ifdef DEBUG
    printf("%d entries, %d dimensions:\n", s, n);
#endif /* DEBUG defined */

    /* XXX call recursive helper function
     * each level loops for next innermost index printing cells
     */
}

void
indent( level )
    int level;
{
    level *= 4;

    while (level > 8) {
	putchar('\t');
	level -= 8;
    }

    while (level-- > 0) {
	putchar(' ');
    }
}

#ifdef DEBUG
/* dump code trees (as passed to TREPUB) */

void
pcode2( cp, level )
    struct codenode *cp;
    int level;
{

    while (cp && cp != (struct codenode *)1) {
	int n;

	indent(level);
	printf("%x: code: %x", cp, cp->code.a.ptr );
#if 0
	/* res used to be an array (now it's a struct with named members) */
	n = ((struct descr *)cp->code.a.ptr) - (struct descr *)&res;
	if (n >= 0 && n <= /*sizeof(res)/sizeof(res[0])*/ 12649)
	    printf(" res[%d]", n);
#endif /* 0 */
	putchar('\n');

	/* print children first */
	pcode2( cp->lson.a.ptr, level+1 );

	/* then print sibliings */
	cp = (struct codenode *) cp->rsib.a.ptr;
    }
}

void
pcode( dp )
    struct descr *dp;
{
    pcode2( dp, 0 );
}
#endif /* DEBUG defined */

static void
dump_keys( ptr )
    struct pairblock *ptr;
{
    int i, n;
    int unit;

    n = ptr->title.v / 2 / DESCR;
    unit = res.punch[0].a.i;
    for (i = 0; i < n; i++) {
	if (NULLVAL(&ptr->pairs[i].value))
	    continue;
	io_printf(unit, "&%v = ", ptr->pairs[i].type );
	pdescr(&ptr->pairs[i].value);
	io_printf(unit, "\n");
    }
}

/* dump unprotected keywords */
void
dump_ukeys() {
    dump_keys(res.knatl[0].a.ptr);	/* D_A(KNATL) */
}

/* dump protected keywords */
void
dump_pkeys() {
    dump_keys(res.kvatl[0].a.ptr);	/* D_A(KVATL) */
}

/* dump fields of a user datatype */
void
puser(dp)
    struct descr *dp;			/* pointer to block pointer */
{
    /* 
     * 1. lookup dp->v in DTATL table (see pdescr()) to get type name
     * 2. lookup name in FNCPL (function table) to get pointer
     *		to func_ld_block.
     * 3. func_ld_block "defn" field contains pointer to "datablock"
     *		which contains array of pointers to field names
     */
}
#endif /* DUMP defined */
