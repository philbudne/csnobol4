/* $Id$ */

/*
 * Functions for Poor-Mans LOAD -- link time funtions
 * used by dummy/load.c
 * could be used as a fallback by real loaders!!
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"			/* LOAD_PROTO */

struct pmlfunc {
    char *name;
    int (*addr)(LOAD_PROTO);
};

#define PMLFUNC(NAME) PMLFUNC2(STRING(NAME),NAME)

#define PMLFUNC2(NAME,ADDR) extern int ADDR(LOAD_PROTO);
#include "pml.h"
#undef PMLFUNC2

static struct pmlfunc pmltab[] = {
#define PMLFUNC2(NAME,ADDR) { NAME, ADDR },
#include "pml.h"
    { NULL, NULL }			/* MUST BE LAST!! */
};

typedef int (*load_func_t)(LOAD_PROTO);

load_func_t
pml_find(name)
    char *name;
{
    struct pmlfunc *fp;

    for (fp = pmltab; fp->name; fp++) {
	/* XXX examine CASECL, use strcasecmp? */
	if (strcmp(name, fp->name) == 0)
	    break;
    }
    return fp->addr;
} /* pml_find */

int
pml_load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    char name[256];			/* XXX */
    struct pmlfunc *fp;
    int l;

    l = S_L(sp1);
    if (l > sizeof(name)-1)
	return FALSE;

    strncpy( name, S_SP(sp1), l);
    name[l] = '\0';

    D_A(addr) = (int_t) pml_find(name);

    return D_A(addr) != NULL;
} /* pml_load */

int
pml_link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    int (*func)(LOAD_PROTO);

    func = (int (*)(LOAD_PROTO)) D_A(addr);
    if (func == NULL)
	return FALSE;

    return (func)( retval, D_A(nargs), (struct descr *)D_A(args) );
}
