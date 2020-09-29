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
#include <stdlib.h>			/* for malloc, getenv */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"

struct lib {
    struct lib *next;
    shl_t handle;			/* from shl_load() */
    int refcount;
};

struct func {
    struct func *next;			/* next in loaded function list */
    struct func *self;			/* for validity check */
    loadable_func_t *entry;		/* function entry point */
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
libopen(char *path) {
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
libclose(struct lib *lib) {
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

loadable_func_t *
os_load(char *function, char *file) {
    struct func *fp; 
    shl_t handle;

    fp = (struct func *) malloc( sizeof (struct func) + strlen(function) );
    if (fp == NULL)
	return NULL;			/* fail */

    strcpy( fp->name, function);
    fp->lib = libopen(path);
    if (fp->lib == NULL) {
	free(fp);
	return NULL;			/* fail */
    }

    handle = fp->lib->handle;		/* get writable copy */
    if (shl_findsym(&handle, fp->name,
		    TYPE_PROCEDURE, (void *)&fp->entry) < 0 ||
	fp->entry == NULL) {
	libclose(fp->lib);
	free(fp);
	return NULL;
    }

    fp->self = fp;			/* make valid */
    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    return fp->entry;
}

void
unload(struct spec *sp) {
    struct func *fp, *pp;
    char name[128];			/* XXX */

    spec2str(sp, name, sizeof(name));	/* XXX mspec2str */

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
