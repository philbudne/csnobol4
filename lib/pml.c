/* $Id$ */

/*
 * Functions for Poor-Mans LOAD -- link time funtions -- see doc/load.doc
 * used by dummy/load.c
 * used as a fallback by real loaders too!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"			/* LOAD_PROTO */
#include "lib.h"

struct pmlfunc {
    char *name;
    int (*addr)(LOAD_PROTO);
};

/* shorthand for function with same name for LOAD() and entry point */
#define PMLFUNC(NAME) PMLFUNC2(STRING(NAME),NAME)

#define PMPROTO(PROTO)
#define PMLFUNC2(NAME,ADDR) extern int ADDR(LOAD_PROTO);
#include "pml.h"
#undef PMLFUNC2

static const struct pmlfunc pmltab[] = {
#define PMLFUNC2(NAME,ADDR) { NAME, ADDR },
#include "pml.h"
    { NULL, NULL }			/* MUST BE LAST!! */
#undef PMLFUNC2
#undef PMPROTO
};

static const char *pm_prototypes[] = {
#define PMLFUNC2(NAME,ADDR)
#define PMPROTO(PROTO) PROTO,
#include "pml.h"
    ""
};

#define NPROTO (sizeof(pm_prototypes)/sizeof(pm_prototypes[0]))-1

#ifdef __STDC__
/* necessary on nextstep? */
#define NAME1 char *name
#define NAME2
#else  /* __STDC__ not defined */
#define NAME1 name
#define NAME2 char *name;
#endif /* __STDC__ not defined */

/* function of char *name which returns pointer to "loaded" function */
int (*pml_find(NAME1))(LOAD_PROTO)
    NAME2
{
    const struct pmlfunc *fp;

    for (fp = pmltab; fp->name; fp++) {
	/* XXX examine CASECL, use strcasecmp? */
	if (strcmp(name, fp->name) == 0)
	    break;
    }
    return fp->addr;
} /* pml_find */

/* return n'th prototype for function to auto-load */
int
getpmproto(sp,dp)
    struct spec *sp;			/* OUT: spec */
    struct descr *dp;			/* IN: which prototype */
{
    if (D_A(dp) >= NPROTO)
	return FALSE;

    S_A(sp) = (int_t) pm_prototypes[D_A(dp)];
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(pm_prototypes[D_A(dp)]);
    CLR_S_UNUSED(sp);
    return TRUE;
}
