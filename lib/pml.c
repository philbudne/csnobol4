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
#include "lib.h"			/* after load.h */
#include "str.h"

struct pmlfunc {
    const char *name;
    loadable_func_t *addr;
};

/* shorthand for function with same name for LOAD() and entry point */
#define PMLFUNC(NAME) PMLFUNC2(#NAME,NAME)

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

#define NPROTO (int)(sizeof(pm_prototypes)/sizeof(pm_prototypes[0]))-1

/* function of char *name which returns pointer to "loaded" function */
loadable_func_t *
pml_find(char *name) {
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
getpmproto(struct spec *sp,		/* OUT: spec */
	   struct descr *dp) {		/* IN: which prototype */
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
