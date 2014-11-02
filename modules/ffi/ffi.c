/* $Id$ */

/*
 * FFI loadable module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 10/31/2014
 *
 * Fred Weigel wrote a different FFI interface, which inspired this...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* for free() */
#endif

#include <inttypes.h>
#include <dlfcn.h>			/* XXX call out to load.c?? */

#include <ffi.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"

#include <stdio.h>			/* TEMP */

static handle_handle_t ffi_cifplus;
static handle_handle_t ffi_dlibs;

struct cifplus {
    enum { NORM, STR, FREESTR } ret;
    ffi_cif cif;
};

#ifdef __STDC__
#define FFI_TYPE(NAME) { #NAME, &ffi_type_##NAME, 0 }
#else
#define FFI_TYPE(NAME) { "NAME", &ffi_type_/**/NAME, 0 }
#endif

#define RETSTRING "string"
#define RETFREESTRING "freestring"

const struct ffi_type_name {
    char *name;
    ffi_type *ptr;
    int ret;
} ffi_type_names[] = {
    /* integer by length: */
    FFI_TYPE(uint8),
    FFI_TYPE(sint8),
    FFI_TYPE(uint16),
    FFI_TYPE(sint16),
    FFI_TYPE(uint32),
    FFI_TYPE(sint32),
    FFI_TYPE(uint64),
    FFI_TYPE(sint64),
    /* floating point: */
    FFI_TYPE(float),
    FFI_TYPE(double),
    FFI_TYPE(longdouble),
    /* pointer */
    FFI_TYPE(pointer),
    /* integer aliases: */
    FFI_TYPE(uchar),
    FFI_TYPE(schar),
    FFI_TYPE(ushort),
    FFI_TYPE(sshort),
    FFI_TYPE(ushort),
    FFI_TYPE(sshort),
    FFI_TYPE(uint),
    FFI_TYPE(sint),
    FFI_TYPE(ulong),
    FFI_TYPE(slong),
    /* for return type: */
    { RETSTRING, &ffi_type_pointer, 1 },
    { RETFREESTRING, &ffi_type_pointer, 1 },
    { "void", &ffi_type_void, 1 },
    { NULL, NULL }
};

#if 1
static char *
pr_type(ftp)
    ffi_type *ftp;
{
    const struct ffi_type_name *ftnp;
    for (ftnp = ffi_type_names; ftnp->name; ftnp++)
	if (ftnp->ptr == ftp)
	    return ftnp->name;
    return "UNK";
}
#endif

static ffi_type *
ffi_convert(cp, ret)
    char *cp;
    int ret;
{
    const struct ffi_type_name *ftnp;
    for (ftnp = ffi_type_names; ftnp->name; ftnp++) {
	if ((ret || !ftnp->ret) &&
	    strcmp(cp, ftnp->name) == 0) {
	    return ftnp->ptr;
	}
    }
    return NULL;
}

/*
 * LOAD("FFI_PREP_CIF(STRING,STRING)INTEGER", FFI_DL)
 * arg types(s), return type
 * Create and initialize an ffi_cif
 *
 * return handle, or failure
 * XXX handle trailing ... on args!!
 */
int
FFI_PREP_CIF( LA_ALIST ) LA_DCL
{
    struct cifplus *cpp = NULL;
    char *cp = mgetstring(LA_PTR(0));
    ffi_type *rtype, **atypes = NULL;
    snohandle_t h;
    char *xp, *comma;
    int n = 0;
    int i;

    if (!cp)
	goto fail;

    comma = cp;
    for (;;) {
	comma = index(comma, ',');
	n++;
	if (!comma)
	    break;
	comma++;
    }

    atypes = malloc(sizeof(ffi_type *)*n);
    if (!atypes)
	goto fail;

    xp = cp;
    for (;;) {
	comma = index(xp, ',');
	if (comma)
	    *comma = '\0';
	if (!(atypes[i] = ffi_convert(xp, 0)))
	    goto fail;
	i++;
	if (!comma)
	    break;
	xp = comma + 1;
    }

    cpp = malloc(sizeof(struct cifplus));
    if (!cpp)
	goto fail;
    bzero(cpp, sizeof(struct cifplus));

    free(cp);
    /* DON'T PUT ANYTHING HERE! */
    cp = mgetstring(LA_PTR(1));		/* get return type */
    if (!cp ||
	!(rtype = ffi_convert(cp, 1)) ||
	ffi_prep_cif(&cpp->cif, FFI_DEFAULT_ABI, n, rtype, atypes) != FFI_OK ||
	(h = new_handle(&ffi_cifplus, cpp)) == BAD_HANDLE) {
    fail:
	if (cpp) free(cpp);
	if (atypes) free(atypes);
    	if (cp) free(cp);
	RETFAIL;
    }

    if (strcmp(cp, RETSTRING) == 0)
	cpp->ret = STR;
    else if (strcmp(cp, RETFREESTRING) == 0)
	cpp->ret = FREESTR;
    free(cp);
    RETINT(h);				/* XXX make string tcl%d? */
}

/* test function */
char *
foo(double a, double b) {
    char ret[512];
    sprintf(ret, "foo %f + %f = %f", a, b, a + b);
    return strdup(ret);
}

/*
 * LOAD("FFI_CALL(CIF,ADDR,)", FFI_DL)
 */
int
FFI_CALL( LA_ALIST ) LA_DCL
{
    int fail = 1;
    struct cifplus *cpp = lookup_handle(&ffi_cifplus, LA_INT(0));
    void *func = (void *)LA_INT(1);	/* YIKES!!! */
    void **arg_pointers = NULL;
    ffi_arg result;
    ffi_cif *cif;
    union argval {
	int8_t s8;
	uint8_t u8;
	int16_t s16;
	uint16_t u16;
	int32_t s32;
	uint32_t u32;
	int64_t s64;
	uint64_t u64;
	float f;
	double d;
	long double ld;
	void *p;
    } *cargs = NULL;
    int i;

    if (!cpp)
	RETFAIL;

    cif = &cpp->cif;
    if (nargs-2 < cif->nargs)
	RETFAIL;

    arg_pointers = malloc(sizeof(void *) * cif->nargs);
    if (!arg_pointers)
	goto ret;
    bzero(arg_pointers, sizeof(void *) * cif->nargs);

    cargs = malloc(sizeof(union argval) * cif->nargs);
    if (!cargs)
	goto ret;
    bzero(cargs, sizeof(union argval) * cif->nargs);

    for (i = 0; i < cif->nargs; i++) {
	int s = i + 2;			/* SNOBOL4 argument index */
	ffi_type *a = cif->arg_types[i];
	/*printf("arg %d type %d %s\n", i, LA_TYPE(s), pr_type(a));*/

#define ARG(TYPE,FIELD) \
	else if (a == &ffi_type_##TYPE) { \
	    if (LA_TYPE(s) == I) cargs[i].FIELD = LA_INT(s); \
	    else if (LA_TYPE(s) == R) cargs[i].FIELD = LA_REAL(s); \
	    else goto ret; \
	    arg_pointers[i] = &cargs[i].FIELD; \
	}
	if (0) ;
	ARG(uint8,u8)
	ARG(sint8,s8)
	ARG(uint16,u16)
	ARG(sint16,s16)
	ARG(uint32,u32)
	ARG(sint32,s32)
	ARG(uint64,u64)
	ARG(sint64,s64)
	ARG(float,f)
	ARG(double,d)
	ARG(longdouble,ld)
	else if (a == &ffi_type_pointer) {
	    if (LA_TYPE(s) != S) goto ret;
	    cargs[i].p = mgetstring(LA_PTR(s));
	    /* XXX accept "I"??? */
	    arg_pointers[i] = &cargs[i].p;
	}
	else
	    goto ret;
    } // for

    ffi_call(cif, FFI_FN(func), &result, arg_pointers);
    fail = 0;

 ret:
    /* free any mgetstring pointers! */
    for (i = 0; i < cif->nargs; i++) {
	if (cif->arg_types[i] == &ffi_type_pointer &&
	    LA_TYPE(i+1) == S && cargs[i].p)
	    free(cargs[i].p);
    }

    if (cargs) free(cargs);
    if (arg_pointers) free(arg_pointers);

    if (!fail) {
	ffi_type *a = cif->rtype;
/* bash these out via a macro? */
	if (a == &ffi_type_uint8)	    RETINT(*(uint8_t *)&result);
	else if (a == &ffi_type_sint8)	    RETINT(*(int8_t *)&result);
	else if (a == &ffi_type_uint16)	    RETINT(*(uint16_t *)&result);
	else if (a == &ffi_type_sint16)	    RETINT(*(int16_t *)&result);
	else if (a == &ffi_type_uint32)	    RETINT(*(uint32_t *)&result);
	else if (a == &ffi_type_sint32)	    RETINT(*(int32_t *)&result);
	else if (a == &ffi_type_uint64)	    RETINT(*(uint64_t *)&result);
	else if (a == &ffi_type_sint64)	    RETINT(*(int64_t *)&result);
	else if (a == &ffi_type_float)	    RETREAL(*(float *)&result);
	else if (a == &ffi_type_double)	    RETREAL(*(double *)&result);
	else if (a == &ffi_type_longdouble) RETREAL(*(long double *)&result);/*?*/
	else if (a == &ffi_type_pointer) {
	    char *str = *(char **)&result;
	    switch (cpp->ret) {
	    case STR:
		RETSTR2(str, strlen(str));
	    case FREESTR:
		RETSTR_FREE(str);
	    case NORM:
		/* On Un*x long always large enough for pointer? */
		RETINT((int_t)str);
	    }
	}
    }
    RETFAIL;
}

/*
 * LOAD("FFI_FREE_CIF(INTEGER)STRING", FFI_DL)
 */
int
FFI_FREE_CIF( LA_ALIST ) LA_DCL
{
    struct cifplus *cpp = lookup_handle(&ffi_cifplus, LA_INT(0));

    if (!cpp)
	RETFAIL;

    if (cpp->cif.arg_types)
	free(cpp->cif.arg_types);
    free(cpp);

    remove_handle(&ffi_cifplus, LA_INT(0)); /* gone to SNOBOL world... */
    RETNULL;
}

/****************************************************************/

/*
 * LOAD("FFI_DLOPEN(STRING)INTEGER", FFI_DL)
 */
int
FFI_DLOPEN( LA_ALIST ) LA_DCL
{
    int_t h;
    // take empty string to mean NULL pointer
    char *str = LA_PTR(0) ? mgetstring(LA_PTR(0)) : NULL;

    /* XXX should call out to load.c!!! */
    void *dl = dlopen(str, RTLD_LAZY);	/* XXX take mode arg??? */
    if (str) free(str);
    if (!dl || (h = new_handle(&ffi_cifplus, dl)) == BAD_HANDLE)
	RETFAIL;
    RETINT(I);
}

/*
 * LOAD("FFI_DLSYM(INTEGER,STRING)STRING", FFI_DL)
 */
int
FFI_DLSYM( LA_ALIST ) LA_DCL
{
    void *h = (void *)LA_INT(0);
    char *str;
    void *val;
    if (h != RTLD_SELF && h != RTLD_DEFAULT && h == RTLD_NEXT) {
	h = lookup_handle(&ffi_dlibs, LA_INT(0));
	if (!h)
	    RETFAIL;
    }
    str = mgetstring(LA_PTR(1));
    val = dlsym(h, str);
    if (str) free(str);
    if (!val)
	RETFAIL;
    RETINT((int_t)val);
}

/*
 * LOAD("FFI_DLCLOSE(INTEGER)STRING", FFI_DL)
 */
int
FFI_DLCLOSE( LA_ALIST ) LA_DCL
{
    void *dlp = lookup_handle(&ffi_dlibs, LA_INT(0));
    if (!dlp)
	RETFAIL;
    dlclose(dlp);
    remove_handle(&ffi_dlibs, LA_INT(0)); /* gone to SNOBOL world... */
    RETNULL;
}

/*
 * LOAD("FFI_RTLD_NEXT()INTEGER", FFI_DL)
 * return RTLD_NEXT value for FFI_DLSYM
 * (could also use C program to output include file line)
 */
int
FFI_RTLD_NEXT( LA_ALIST ) LA_DCL
{
    RETINT((int_t)RTLD_NEXT);
}

/*
 * LOAD("FFI_RTLD_DEFAULT()INTEGER", FFI_DL)
 * return RTLD_DEFAULT value for FFI_DLSYM
 * (could also use C program to output include file line)
 */
int
FFI_RTLD_DEFAULT( LA_ALIST ) LA_DCL
{
    RETINT((int_t)RTLD_DEFAULT);
}

/*
 * LOAD("FFI_RTLD_SELF()INTEGER", FFI_DL)
 * return RTLD_SELF value for FFI_DLSYM
 * (could also use C program to output include file line)
 */
int
FFI_RTLD_SELF( LA_ALIST ) LA_DCL
{
    RETINT((int_t)RTLD_SELF);
}
