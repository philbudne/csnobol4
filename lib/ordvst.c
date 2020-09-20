/* $Id$ */

/*
 *	ordvst.c
 *	Martin. D. Waller	July 28th, 1994
 *
 *	ORDVST is used by the SNOBOL4 system to sort the variables
 *	into alphabetical order prior to dumping. It does this by
 *	working down the chained list or string structures saving the
 *	address of each descriptor found. Once complete the list is
 *	sorted and the chain list of string structures re-written.
 *
 *	Integrated, simplified, 8/1/94 -phil
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio, h.h */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>
#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"
#include "str.h"

#include "equ.h"
#include "res.h"
#include "data.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

#ifdef ORDVST_DEBUG
/* PLB: dump a descriptor */
static void
dump_descr(struct descr *vp)
{
    printf("0x%08x v: 0x%08x f: 0x%02x t: 0x%06x\n",
	   vp, D_A(vp), D_F(vp), D_V(vp));
}

/*
 *	ordvst_dnv(nv)
 *
 *	This routine is callable to dump a natural variable. It is intended
 *	as a pure aid to debugging and nothing more.
 *
 */
static void
ordvst_dnv(nv)
    struct descr *nv;
{
    struct descr *vd;			/* Value field descriptor */
    struct descr *ad;			/* Attribute field descriptor */
    struct descr *ld;			/* Link field descriptor */
    char *n;

    vd = nv + 1;			/* Set up the value descriptor */
    ad = nv + ATTRIB/DESCR;		/* Set up the attribute descriptor */
    ld = nv + LNKFLD/DESCR;		/* Set up the link descriptor */
    n = ((char *) nv) + BCDFLD;		/* Pointer to the name itself */
    printf("**** Natural Variable -> %.*s\n",D_V(nv),n);
    printf("**** nv	@"); dump_descr(nv);
    printf("**** value	@"); dump_descr(vd);
    printf("**** attrib @"); dump_descr(ad);
    printf("**** link	@"); dump_descr(ld);
    printf("\n");
}
#endif /* ORDVST_DEBUG defined */

/*
 *	ordvst_strcmp(s1,l1,s2,l2)
 *
 *	This routine is called to compare two strings passed by reference
 *	along with a length argument. It will return -1, 0, +1 to indicate
 *	if s1 < s2, s1 = s2, or s1 > s2.
 */
static int
ordvst_strcmp(s1,l1, s2,l2)
    char *s1, *s2;
    int l1, l2;
{
    int i;

    i = l1;
    if (l2 < i)
	i = l2;

    while (i-- > 0) {			/* for common portion */
	if (*s1 != *s2)			/* If the characters are different */
	    return (*s1 - *s2);		/* Return the difference */
	s1++;
	s2++;
    }

    /* strings are identical, or one is a prefix of the other */
    return l1 - l2;			/* shorter string is "less" */
}

/*
 *	ordvst_cmp(d1,d2)
 *
 *	This routine is callable by qsort to compare the two descriptors.
 *	It will return the result of ordvst_strcmp defined above.
 *
 */
static int
#ifdef __STDC__
ordvst_cmp(void *v1, void *v2)
#else
ordvst_cmp(v1, v2)
    const void *v1, *v2;
#endif
{
    int l1,l2;				/* Length variables */
    char *n1, *n2;			/* Name pointers */
    const struct descr **d1 = (const struct descr **)v1;
    const struct descr **d2 = (const struct descr **)v2;

    l1 = D_V(*d1);			/* Set the first length */
    l2 = D_V(*d2);			/* Set the second length */
    n1 = ((char *) *d1) + BCDFLD;	/* Set the first name pointer */
    n2 = ((char *) *d2) + BCDFLD;	/* Set the second name pointer */

    return (ordvst_strcmp(n1,l1,n2,l2)); /* Compare the two */
}

/*
 *	ORDVST()
 *
 *	This is the main entry point from the SNOBOL4 system. It will sort
 *	the string structures into order. Should the sort fail due to a lack
 *	of dynamic memory then the sort is not performed and the original
 *	list of string structures will be left unchanged.
 */
void
ordvst()
{
    int bc;				/* Bin count */
    int i;				/* Looping variable */
    struct descr *bd;			/* Descriptor from the bins */
    struct descr *lnkd;			/* Link field descriptor */
    struct descr **vars;		/* array of var ptrs */
    struct descr **vp;			/* pointer to next stry to fill */
    int nvars;

    bd = (struct descr *) OBSTRT;	/* Locate the first bin */
#ifdef ORDVST_DEBUG
    printf("**** OBSTRT @ %#08x, %d buckets\n", bd, OBSIZ);
#endif /* ORDVST_DEBUG defined */

    /* first pass; count number of vars to allocate pointers for */
    nvars = 0;
    for (bc = 0; bc < OBSIZ; bc++, bd++) {	/* For all the bins we have */
	struct descr *nvcd;		/* Natural variable chain */
	struct descr *vd;		/* Value field descriptor */

	/* Pick up the start of the chain */
	nvcd = (struct descr *) D_A(bd);

#ifdef ORDVST_DEBUG
	if (nvcd != NULL) {
	    printf("**** OBSTRT[%d] @", bc); dump_descr(bd);
	}
#endif /* ORDVST_DEBUG defined */
	while (nvcd != NULL) {		/* Until the end of the chain */
	    vd = nvcd + 1;		/* Set up the value descriptor */

	    /* don't count strings that aren't natural variables!! */
	    if (D_V(vd) != S || D_F(vd) != 0) {
#ifdef ORDVST_DEBUG
		ordvst_dnv(nvcd);	/* Dump the natural variable */
#endif /* ORDVST_DEBUG defined */
		nvars++;
	    }
	    /*  Pick up the link descriptor */
	    nvcd = (struct descr *) D_A(nvcd + LNKFLD/DESCR);
	} /* while nvcd */
    } /* for each bucket */

    if (nvars == 0)			/* bloody unlikely */
	return;

    /* allocate array of pointers to variables */
    vars = (struct descr **) malloc( nvars * sizeof(struct descr *) );
    if (vars == NULL)
	return;
    vp = vars;				/* set up pointer to array */

    /* pass two; fill in array */
    bd = (struct descr *) OBSTRT;	/* Locate the first bin */
    for (bc = 0; bc < OBSIZ; bc++, bd++) { /* For all the bins we have */
	struct descr *nvcd;		/* Natural variable chain */
	struct descr *vd;		/* Value field descriptor */

	/* Pick up the start of the chain */
	nvcd = (struct descr *) D_A(bd);

	while (nvcd != 0) {		/* Until the end of the chain */
	    vd = nvcd + 1;		/* Set up the value descriptor */

	    /* don't save strings that aren't natural variables!! */
	    if (D_V(vd) != S || D_F(vd) != 0) {
		*vp++ = nvcd;		/* append to array */
	    }
	    lnkd = nvcd + LNKFLD/DESCR;	/*  Pick up the link descriptor */
	    nvcd = (struct descr *) D_A(lnkd); /* Move down the chain */
	} /* while nvcd */
    } /* for each bucket */

#ifdef ORDVST_DEBUG
    printf("**** Sorting\n");		/* Say that we are sorting */
#endif /* ORDVST_DEBUG defined */

    qsort((void *) vars, nvars, sizeof(struct descr *), ordvst_cmp);

#ifdef ORDVST_DEBUG
    printf("**** After Sorting\n\n");
    for (i = 0, vp = vars; i < nvars; i++, vp++) { /* for all entries */
	ordvst_dnv(*vp);
    }
#endif /* ORDVST_DEBUG defined */

    /* null out entire hash table */
    bzero( OBSTRT, OBSIZ * DESCR );

    /* link variables together in sorted order */
    vp = vars;
    for (i = nvars; i-- > 0; ) {
	lnkd = *vp++ + LNKFLD/DESCR;	/* point to var link field */

	if (i > 0)
	    D_A(lnkd) = (int_t) *vp;	/* point to next var */
	else
	    D_A(lnkd) = 0;		/* End of the list */
    } /* for all entries */

    /* set up first bin to point to sorted list */
    D_A(OBSTRT) = (int_t) vars[0];

    free(vars);
}


