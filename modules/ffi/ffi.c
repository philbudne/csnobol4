/* $Id$ */
/*#define DEBUG_FFI*/

/*
 * FFI loadable module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 10/31/2014
 *
 * Fred Weigel wrote a different FFI interface, which inspired this...
 */

/*
**=pod
**=sect NAME
**snobol4ffi \- Foreign Function Interface for SNOBOL4
**=sect SYNOPSYS
**=code
**B<-INCLUDE 'ffi.sno'>
**        dl = FFI_DLOPEN("/usr/lib/libm.so")
**        sym = FFI_DLSYM(dl, "hypot")
**        DEFINE_FFI("hypot(double,double)double", sym)
**        OUTPUT = hypot(3, 4)
**=sect DESCRIPTION
**=cut
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* for free() */
#endif

#include <inttypes.h>
#include <stdio.h>			/* for debug, test function */
#include <ffi.h>

/* XXX call out to load.c?? */
#define _GNU_SOURCE			/* for RTLD_{DEFAULT,NEXT} */
#define __USE_GNU
#include <dlfcn.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"

static handle_handle_t ffi_cifplus;
static handle_handle_t ffi_dlibs;
static handle_handle_t ffi_dlsyms;

#define RETSTRING "string"
#define RETFREESTRING "freestring"

struct cifplus {
    enum { STR='s', FREESTR='f', NOTSTR='p' } pret;	/* pointer return */
    ffi_cif cif;
};

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
};

#define FFI_TYPE(NAME) { #NAME, &ffi_type_##NAME, 0 }

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
    /* for return type only: */
    { RETSTRING, &ffi_type_pointer, 1 },
    { RETFREESTRING, &ffi_type_pointer, 1 },
    { "void", &ffi_type_void, 1 },
    { NULL, NULL }
};

#ifdef DEBUG_FFI
static char *
ffi_str(ftp)
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
	/* only allow "return only" when called for return type! */
	if ((ret || !ftnp->ret) && strcmp(cp, ftnp->name) == 0) {
	    return ftnp->ptr;
	}
    }
    return NULL;
}

/*
 * LOAD("FFI_PREP_CIF(STRING)INTEGER", FFI_DL)
 * arg: "(argtype[,...])rettype"
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
    char *xp, *comma, *rp;
    int n = 0;
    int i;

    if (!cp || *cp != '(') goto fail;

    comma = cp + 1;			/* skip open paren */
    rp = index(comma, ')');
    if (!rp) goto fail;
    *rp++ = '\0';

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

    xp = cp + 1;
    for (;;) {
	comma = index(xp, ',');
	if (comma)
	    *comma = '\0';
#ifdef DEBUG_FFI
	printf("arg %d %s\n", i, xp);
#endif
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

#ifdef DEBUG_FFI
	printf("ret %s\n", rp);
#endif
    if (!(rtype = ffi_convert(rp, 1)) ||
	ffi_prep_cif(&cpp->cif, FFI_DEFAULT_ABI, n, rtype, atypes) != FFI_OK ||
	(h = new_handle(&ffi_cifplus, cpp)) == BAD_HANDLE) {
    fail:
	if (cpp) free(cpp);
	if (atypes) free(atypes);
    	if (cp) free(cp);
	RETFAIL;
    }

    if (strcmp(rp, RETSTRING) == 0)
	cpp->pret = STR;
    else if (strcmp(rp, RETFREESTRING) == 0)
	cpp->pret = FREESTR;
    else
	cpp->pret = NOTSTR;
    free(cp);
    RETINT(h);				/* XXX make string tcl%d? */
}

/* test function */
char *
foo(double a, double b) {
    char ret[512];
    sprintf(ret, "foo %g + %g = %g", a, b, a + b);
    return strdup(ret);
}

/*
 * LOAD("FFI_CALL(INTEGER,INTEGER)", FFI_DL)
 * arg 1: handle from FFI_PREP_CIF
 * arg 2: handle from FFI_DLSYM (function pointer)
 * args 3+: arguments to pass to function
 */
int
FFI_CALL( LA_ALIST ) LA_DCL
{
    struct cifplus *cpp = lookup_handle(&ffi_cifplus, LA_INT(0));
    void *func = lookup_handle(&ffi_dlsyms, LA_INT(1));
    void **arg_pointers = NULL;
    union argval *cargs = NULL;
    ffi_arg result;
    ffi_cif *cif;
    int fail = 1;
    int i;

#ifdef DEBUG_FFI
    printf("FFI_CALL %ld (%p) %ld (%p)\n", LA_INT(0), cpp, LA_INT(1), func);
#endif
    if (!cpp || !func) RETFAIL;

    cif = &cpp->cif;
    if (nargs-2 < cif->nargs) RETFAIL;

    arg_pointers = malloc(sizeof(void *) * cif->nargs);
    if (!arg_pointers) goto ret;
    bzero(arg_pointers, sizeof(void *) * cif->nargs);

    cargs = malloc(sizeof(union argval) * cif->nargs);
    if (!cargs) goto ret;
    bzero(cargs, sizeof(union argval) * cif->nargs);

#define ARG_OFFSET 2
    for (i = 0; i < cif->nargs; i++) {
	int s = i + ARG_OFFSET;		/* SNOBOL4 argument index */
	ffi_type *a = cif->arg_types[i];
#ifdef DEBUG_FFI
	printf("arg %d type %d %s\n", i, LA_TYPE(s), ffi_str(a));
#endif
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
	    if (LA_TYPE(s) == S)
		cargs[i].p = mgetstring(LA_PTR(s));
	    else if (LA_TYPE(s) != I ||
		     !(cargs[i].p = lookup_handle(&ffi_dlsyms, LA_INT(1))))
		goto ret;
	    arg_pointers[i] = &cargs[i].p;
	}
	else
	    goto ret;
    } // for

#ifdef DEBUG_FFI
    printf("calling\n");
#endif
    ffi_call(cif, FFI_FN(func), &result, arg_pointers);
    fail = 0;

 ret:
    /* free strings from mgetstring */
    for (i = 0; i < cif->nargs; i++) {
	if (cif->arg_types[i] == &ffi_type_pointer &&
	    LA_TYPE(i+ARG_OFFSET) == S && cargs[i].p)
	    free(cargs[i].p);
    }
    if (cargs) free(cargs);
    if (arg_pointers) free(arg_pointers);

    if (!fail) {
	ffi_type *a = cif->rtype;
#define RET(FFI,RETMACRO,CTYPE) \
	else if (a == &ffi_type_##FFI) RETMACRO(*(CTYPE *)&result)

	if (0) ;
	RET(uint8,RETINT,uint8_t);
	RET(sint8,RETINT,int8_t);
	RET(uint16,RETINT,uint16_t);
	RET(sint16,RETINT,int16_t);
	RET(uint32,RETINT,uint32_t);
	RET(sint32,RETINT,int32_t);
	RET(uint64,RETINT,uint64_t);
	RET(sint64,RETINT,int64_t);
	RET(float,RETREAL,float);
	RET(double,RETREAL,double);
	RET(longdouble,RETREAL,long double);
	else if (a == &ffi_type_pointer) {
	    char *ptr = *(char **)&result;
#ifdef DEBUG_FFI
	    printf("pret %c\n", cpp->pret);
#endif
	    switch (cpp->pret) {
	    case STR:	RETSTR2(ptr, strlen(ptr));
	    case FREESTR: RETSTR_FREE(ptr);
	    case NOTSTR: RETINT((int_t)ptr); /* UGH!!! */
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
**=pod
**=item B<FFI_DLOPEN(>I<filename>B<)>
**takes the path of a dynamicly loadable
**executable file (B<.so>, B<.bundle>,  or B<.DLL>) and returns a value
**which can be passed to B<FFI_DLSYM()>.
**
**=cut
*/

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
    if (!dl) RETFAIL;
    if ((h = new_handle(&ffi_dlibs, dl)) == BAD_HANDLE) {
	dlclose(dl);
	RETFAIL;
    }
    RETINT(h);
}

/*
**=pod
**=item B<FFI_DLSYM(>I<library>,I<function>B<)>
**takes a I<library> value
**returned by B<FFI_DLOPEN()>, and a function name, and returns a
**value which can be passed to B<DEFINE_FFI()>.
**=cut
*/

/*
 * LOAD("FFI_DLSYM(INTEGER,STRING)INTEGER", FFI_DL)
 * returns a handle for FFI_CALL
 */
int
FFI_DLSYM( LA_ALIST ) LA_DCL
{
    void *dl = (void *)LA_INT(0);
    int_t ret;
    char *str;
    void *val;
    if (dl != RTLD_DEFAULT && dl == RTLD_NEXT
#ifdef RTLD_SELF
	&& dl != RTLD_SELF
#endif
	) {
	dl = lookup_handle(&ffi_dlibs, LA_INT(0));
	if (!dl) RETFAIL;
    }
    str = mgetstring(LA_PTR(1));
    val = dlsym(dl, str);
    if (str) free(str);
    if (!val) RETFAIL;
    if ((ret = new_handle(&ffi_dlsyms, val)) == BAD_HANDLE) RETFAIL;
#ifdef FFI_DEBUG
    printf("dlsym %ld %p\n", ret, val);
#endif
    RETINT(ret);
}

#if 0 /* would need to invalidate function handles */
/*
 * XXX("FFI_DLCLOSE(INTEGER)STRING", FFI_DL)
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
#endif

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
#ifdef RTLD_SELF
    RETINT((int_t)RTLD_SELF);
#else
    RETNULL;				/* works on Linux */
#endif
}
