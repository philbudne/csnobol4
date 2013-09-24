/* $Id$ */

/*
 * load and run external functions for systems using dlopen()/dlsym()
 * -plb 4/13/97
 *
 * called by loadx.c 2/15/2012
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>			/* sprintf() */
#include <dlfcn.h>

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* malloc(), getenv() */
#endif /* HAVE_STDLIB_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"			/* spec2str() */
#include "str.h"

#ifndef RTLD_LAZY
#define RTLD_LAZY 0			/* Needed on FreeBSD 2.2.1-RELEASE */
#endif /* RTLD_LAZY not defined */

/* keep list of loaded functions (for UNLOAD) */
struct func {
    struct func *next;			/* next in loaded function list */
    void *handle;			/* from dlopen() */
    char name[1];			/* for unload (MUST BE LAST)! */
};

static struct func *funcs;

/* called from loadx.c */
void *
os_load(fname, lname)
    char *fname, *lname;
{
    void *handle;
    int (*entry)(LOAD_PROTO);		/* function entry point */
    struct func *fp; 

    /* XXX if lname doesn't have a DIR_SEP, prepend . + DIR_SEP? */
    /* XXX if lname is empty, pass NULL (self) to dlopen?? */

    /*
     * SunOS4 (and others) only support LAZY mode.
     * RTLD_GLOBAL could cause collisions between modules??
     */
    if (index(lname, DIR_SEP[0]) == 0) {
	char *path = strjoin(".", DIR_SEP, lname, NULL);
	handle = dlopen(path, RTLD_LAZY);
	free(path);
    }
    else
	handle = dlopen(lname, RTLD_LAZY);
    /* XXX free allocated path, if any */
    if (!handle)
	return NULL;

    entry = (int (*)(LOAD_PROTO)) dlsym(handle, fname);
    if (!entry) {
#ifdef TRY_UNDERSCORE
	char name2[1024];		/* XXX */

	name2[0] = '_';
	strncpy(name2+1, fname, sizeof(name2)-2);
	name2[sizeof(name2)-1] = '\0';

	entry = (int (*)(LOAD_PROTO)) dlsym(handle, name2);
	if (entry == NULL)
	    goto fail;
#else  /* TRY_UNDERSCORE not defined */
	goto fail;
#endif /* TRY_UNDERSCORE not defined */
    } /* dlsym failed */
    fp = (struct func *) malloc(sizeof(struct func) + strlen(fname));
    if (fp == NULL) {
    fail:
	dlclose(handle);
	return NULL;
    }

    strcpy(fp->name, fname);
    fp->handle = handle;
    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    return entry;
}

void
unload(sp)
    struct spec *sp;
{
    struct func *fp, *pp;
    char name[1024];			/* XXX */

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

    dlclose(fp->handle);
    free(fp);				/* free name block */
}
