/* $Id$ */

/*
 * load and run external functions for Win32
 * -plb 1/5/98
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <string.h>			/* strcmp() */
#include <stdlib.h>			/* malloc(), getenv() */
#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"

struct func {
    struct func *next;			/* next in loaded function list */
    HMODULE handle;			/* from LoadLibrary() */
    char name[1];			/* for unload (MUST BE LAST)! */
};

#ifdef NO_STATIC_VARS
#include "vars.h"
#define funcs loadptr
#else  /* NO_STATIC_VARS not defined */
/* keep list of loaded functions (for UNLOAD) */
static struct func *funcs;
#endif /* NO_STATIC_VARS not defined */

loadable_func_t *
os_load(char *fname, char *lname) {
    HMODULE handle;
    loadable_func_t *entry;		/* function entry point */
    struct func *fp; 

    /*
     * try loading given path;
     * system will scan various directories
     * (including appl dir, cwd, SYSTEM(32),
     * windows dir, and dirs in PATH var)
     */

    handle = LoadLibrary(lname);
    if (!handle)
	return NULL;

    entry = (loadable_func_t *) GetProcAddress(handle, fname);
    if (entry == NULL)
	goto fail;

    fp = (struct func *) malloc( sizeof (struct func) + strlen(fname) );
    if (fp == NULL) {
    fail:
	FreeLibrary(handle);
	return NULL;
    }

    strcpy(fp->name, fname);
    fp->handle = handle;
    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    return entry;
}

void
unload(struct spec *sp) {
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

    FreeLibrary(fp->handle);
    free(fp);				/* free name block */
}
