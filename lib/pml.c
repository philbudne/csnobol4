/* $Id$ */

/*
 * Functions for Poor-Mans LOAD -- link time funtions
 * used by dummy/load.c
 * could be used as a fallback by real loaders!!
 */

#ifndef NO_PML
#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"			/* LOAD_PROTO */

struct pmlfunc {
    char *name;
    int (*addr)(LOAD_PROTO);
};

/* shorthand for function with same name for LOAD() and entry point */
#define PMLFUNC(NAME) PMLFUNC2(STRING(NAME),NAME)

#define PMLOAD(PROTO)
#define PMLFUNC2(NAME,ADDR) extern int ADDR(LOAD_PROTO);
#include "pml.h"
#undef PMLFUNC2

static struct pmlfunc pmltab[] = {
#define PMLFUNC2(NAME,ADDR) { NAME, ADDR },
#include "pml.h"
    { NULL, NULL }			/* MUST BE LAST!! */
#undef PMLFUNC2
#undef PMLOAD
};

char *pm_prototypes[] = {
#define PMLFUNC2(NAME,ADDR)
#define PMLOAD(PROTO) PROTO,
#include "pml.h"
    ""
};

#ifdef __STDC__
/* necessary on nextstep? */
#define NAME1 char *name
#define NAME2
#else
#define NAME1 name
#define NAME2 char *name;
#endif

/* function of char *name which returns pointer to "loaded" function */
int (*pml_find(NAME1))(LOAD_PROTO)
    NAME2
{
    struct pmlfunc *fp;

    for (fp = pmltab; fp->name; fp++) {
	/* XXX examine CASECL, use strcasecmp? */
	if (strcmp(name, fp->name) == 0)
	    break;
    }
    return fp->addr;
} /* pml_find */
#endif /* NO_PML not defined */
