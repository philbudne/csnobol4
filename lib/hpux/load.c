/* $Id$ */

/*
 * load and run external functions using HP-UX shl_load()
 * -plb 5/22/97
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>
#include <dl.h>

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc, getenv */
#endif /* HAVE_STDLIB_H defined */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

struct lib {
    struct lib *next;
    shl_t handle;			/* from shl_load() */
    int refcount;
};

struct func {
    struct func *next;			/* next in loaded function list */
    struct func *self;			/* for validity check */
    int (*entry)(LOAD_PROTO);		/* function entry point */
    struct lib *lib;
    char name[1];			/* for unload (MUST BE LAST)! */
};

/* keep list of loaded functions (for UNLOAD) */
static struct func *funcs;

/* list of loaded libs */
static struct lib *libs;

#define PATHLEN 256			/* XXX use MAXPATHLEN from param.h? */

/* create refcounted lib interface */

static struct lib *
libopen(path)
    char *path;
{
    shl_t handle;
    struct lib *lp;

    /* XXX use PROG_HANDLE for null string? */
    handle = shl_load(path, BIND_DEFERRED|BIND_VERBOSE, 0L);
    if (handle == NULL)
	return NULL;

    /* see if it's one we've already mapped */
    for (lp = libs; lp; lp = lp->next) {
	if (lp->handle == handle) {
	    lp->refcount++;
	    return lp;
	}
    }

    /* a new one; add to list */
    lp = (struct lib *) malloc(sizeof(struct lib));
    lp->next = libs;
    lp->handle = handle;
    lp->refcount = 1;

    libs = lp;
    return lp;
}

int
libclose(lib)
    struct lib *lib;
{
    struct lib *lp, *pp;
    int ret;

    /* find previous, if any */
    for (lp = libs, pp = NULL; lp && lp != lib; pp = lp, lp = lp->next)
	;

    if (!lp)
	return 0;			/* not found */

    ret = 1;
    if (--(lp->refcount) <= 0) {
	/* detach library */
	if (lp->handle != PROG_HANDLE && shl_unload(lp->handle) < 0)
	    ret = 0;

	/* unlink from list */
	if (pp)
	    pp->next = lp->next;
	else
	    libs = lp->next;
	free(lp);
    }
    return ret;
}

/*****************/

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    struct func *fp; 
    shl_t handle;
    int l1;

    l1 = S_L(sp1);
    fp = (struct func *) malloc( sizeof (struct func) + l1 );
    if (fp == NULL)
	return FALSE;			/* fail */

    strncpy( fp->name, S_SP(sp1), l1 );
    fp->name[l1] = '\0';
    fp->lib = NULL;			/* assume internal! */

    /* try "poor mans load" first!!! */
    fp->entry = pml_find(fp->name);
    if (fp->entry == NULL) {		/* not found by pml */
	char path[PATHLEN*2];		/* room for directory name */
	char *snolib;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

	if (sp2 && S_A(sp2) && S_L(sp2)) {
	    struct stat st;
	    char temp[PATHLEN];

	    spec2str( sp2, temp, sizeof(temp) );
	    if (temp[0] != '/' && stat(temp, &st) < 0) {
		/* not absolute and file does not exist; prepend libdir */
		/* XXX limit length of snolib??? */
		sprintf( path, "%s/%s", snolib, temp );
	    }
	    else
		strcpy( path, temp );
	}
	else {				/* no path */
	    /* XXX use special "self" handle (search main program)?? */
	    /* XXX limit length of snolib??? */
	    sprintf( path, "%s/%s", snolib, SNOLIB_FILE );
	}

	fp->lib = libopen(path);
	if (fp->lib == NULL) {
	    free(fp);
	    return FALSE;		/* fail */
	}

	handle = fp->lib->handle;	/* get writable copy */
	if (shl_findsym(&handle, fp->name,
			TYPE_PROCEDURE, (void *)&fp->entry) < 0 ||
	    fp->entry == NULL) {
	    libclose(fp->lib);
	    free(fp);
	    return FALSE;
	}
    } /* not found by pml */
    fp->self = fp;			/* make valid */

    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    D_A(addr) = (int_t) fp;
    D_F(addr) = D_V(addr) = 0;		/* clear flags, type */
    return TRUE;			/* success */
}

/* support for SIL "LINK" opcode -- call external function */
int
callx(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    struct func *fp;
#ifdef DUMP
    int i;
    struct shl_descriptor *dp;
#endif

    /* XXX check for zero V & F fields?? */
    fp = (struct func *) D_A(addr);
    if (fp == NULL)
	return FALSE;

    if (fp->self != fp)			/* validate, in case unloaded */
	return FALSE;			/* fail (fatal error??) */

#ifdef DUMP
    printf("calling %s entry %x lib %x handle %x\n",
	   fp->name, fp->entry, fp->lib, (fp->lib ? fp->lib->handle : 0));
    i = 0;
    while (shl_get(i++, &dp) == 0 && dp) {
	printf("%s text %x-%x data %x-%x handle %x\n",
	       dp->filename,
	       dp->tstart, dp->tend,
	       dp->dstart, dp->dend,
	       dp->handle);
    }
#endif
    return (fp->entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}

void
unload(sp)
    struct spec *sp;
{
    struct func *fp, *pp;
    char name[128];			/* XXX */

    spec2str(sp, name, sizeof(name));

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

    libclose(fp->lib);

    fp->self = 0;			/* invalidate self pointer!! */
    free(fp);				/* free name block */
}

