/* $Id$ */

/*
 * load and run external functions for systems using dlopen()/dlsym()
 * -plb 4/13/97
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>
#ifdef ANSI_STRINGS
#include <string.h>
#else  /* ANSI_STRINGS not defined */
#include <strings.h>                    /* berkeley strings */
#endif /* ANSI_STRINGS not defined */

/* XXX ifdef HAVE_STDLIB_H? */
#include <stdlib.h>			/* malloc(), getenv() */

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

struct func {
    struct func *next;			/* next in loaded function list */
    struct func *self;			/* for validity check */
    int (*entry)(LOAD_PROTO);		/* function entry point */
    void *handle;			/* from dlopen() */
    char name[1];			/* for unload (MUST BE LAST)! */
};

/* keep list of loaded functions (for UNLOAD) */
static struct func *funcs;

#define PATHLEN 256			/* XXX use MAXPATHLEN from param.h? */

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    struct func *fp; 
    int l1;

    fp = (struct func *) malloc( sizeof (struct func) + S_L(sp1) );
    if (fp == NULL)
	return FALSE;			/* fail */

    l1 = S_L(sp1);			/* XXX check if <= sizeof(fp->name)? */
    strncpy( fp->name, S_SP(sp1), l1);
    fp->name[l1] = '\0';
    fp->handle = NULL;			/* assume internal! */

    /* try "poor mans load" first!!! */
    fp->entry = pml_find(fp->name);
    if (fp->entry == NULL) {		/* not found by pml */
	char path[PATHLEN];
	char temp[PATHLEN];
	char *snolib;
	long len;			/* size of code+data */
	void *lib;
	int l2;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

	/* XXX try pml here? */
	l2 = S_L(sp2);			/* XXX check if .le. sizeof(path)? */
	if (sp2 && S_A(sp2) && l2) {
	    char *tp;
	    char temp2[PATHLEN];
	    struct stat st;

	    strncpy(temp, S_SP(sp2), l2 );
	    temp[l2] = '\0';

	    strcpy(temp2, temp);	/* save copy */
	    tp = index(temp, ' ');	/* look for space */
	    if (tp)
		*tp = '\0';		/* blot out space */

	    if (stat(temp, &st) < 0)	/* test if prefix exists */
		sprintf( path, "%s/%s", snolib, temp2 ); /* no prepend path */
	    else
		strcpy( path, temp2 );
	}
	else {				/* no path */
	    sprintf( path, "%s/%s", snolib, SNOLIB_A );
	}

	/* SunOS4 only supports LAZY mode */
	fp->handle = dlopen(temp, RTLD_LAZY);
	if (fp->handle == NULL) {
#if 0
	    printf("dlerror: %s\n", dlerror());	/* XXX TEMP */
#endif
	    free(fp);
	    return FALSE;		/* fail */
	}

	fp->entry = (int (*)(LOAD_PROTO)) dlsym(fp->handle, fp->name);
	if (fp->entry == NULL) {
	    dlclose(fp->handle);
	    free(fp);
	    return FALSE;
	}
    } /* not found by pml */
    fp->self = fp;			/* make valid */
    fp->next = fp;			/* link into list (for unload) */
    funcs = fp;

    D_A(addr) = (int_t) fp;
    return TRUE;			/* success */
}

/* support for SIL "LINK" opcode -- call external function */
int
link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    struct func *fp;

    fp = (struct func *) D_A(addr);
    if (fp == NULL)
	return FALSE;

    if (fp->self != fp)			/* validate, in case unloaded */
	return FALSE;			/* fail (fatal error??) */

    return (fp->entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}

void
unload(sp)
    struct spec *sp;
{
    struct func *fp, *pp;
    char name[128];			/* XXX */

    strncpy( name, S_SP(sp), S_L(sp) );	/* XXX watch length? */
    name[S_L(sp)] = '\0';

    for (pp = NULL, fp = funcs; fp != NULL; pp = fp, fp = fp->next) {
	if (strcmp(fp->name, name) == 0)
	    break;
    }

    if (fp == NULL)			/* not found */
	return;

    /* unlink from list */
    if (pp == NULL) {			/* first */
	funcs = fp->next;
    }
    else {				/* not first */
	pp->next = fp->next;
    }

    dlclose(fp->handle);

    fp->self = 0;			/* invalidate self pointer!! */
    free(fp);				/* free name block */
}
