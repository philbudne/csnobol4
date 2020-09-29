/* $Id$ */

/*
 * load and run external functions for NextStep based systems
 *	including MacOS X/Darwin/Rhapsody
 * -plb 11/3/2000
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>

/* NS: /NextDeveloper/Headers/mach-o/dyld.h */
#include <mach-o/dyld.h>

#include <stdlib.h>			/* malloc(), getenv() */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"

struct func {
    struct func *next;			/* next in loaded function list */
    loadable_func_t *entry;		/* function entry point */
    NSModule handle;			/* from NSLinkModule */
    char name[1];			/* for unload (MUST BE LAST)! */
};

/* keep list of loaded functions (for UNLOAD) */
static struct func *funcs;

#define PATHLEN 256			/* XXX use MAXPATHLEN from param.h? */

loadable_func_t
os_load(char *func, char *file) {
    struct func *fp; 
    NSObjectFileImage ofi;
    NSSymbol sym;
    int opt;

    fp = (struct func *) malloc( sizeof (struct func) + strlen(func) );
    if (fp == NULL)
	return FALSE;			/* fail */

    strcpy(fp->name, func);


    if (NSCreateObjectFileImageFromFile(file, &ofi) !=
	NSObjectFileImageSuccess) {
	free(fp);
	return FALSE;			/* fail */
    }

#ifdef NSLINKMODULE_OPTION_PRIVATE
    /* MacOS X; avoid symbol clashes */
    opt = NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW;
#else  /* NSLINKMODULE_OPTION_PRIVATE not defined */
    opt = TRUE;		 /* old "bindnow" */
#endif /* NSLINKMODULE_OPTION_PRIVATE not defined */
    fp->handle = NSLinkModule(ofi, file, opt);
    if (!fp->handle) {
	/* XXX NSDestroyObjectFileImage(ofi); ? keep ref count?? */
	free(fp);
	return NULL;			/* fail */
    }

#ifdef NSLINKMODULE_OPTION_PRIVATE
    sym = NSLookupSymbolInModule(fp->handle, fp->name);
#else  /* NSLINKMODULE_OPTION_PRIVATE not defined */
    sym = NSLookupAndBindSymbol(fp->name);
#endif /* NSLINKMODULE_OPTION_PRIVATE not defined */
    /* XXX check return?? */

    fp->entry = (loadable_func_t *) NSAddressOfSymbol(func);
    if (fp->entry == NULL) {
	int opt;
#ifdef TRY_UNDERSCORE
	char name2[1024];		/* XXX */

	name2[0] = '_';
	strncpy(name2+1, fp->name, sizeof(name2)-2);
	name2[sizeof(name2)-1] = '\0';

#ifdef NSLINKMODULE_OPTION_PRIVATE
	sym = NSLookupSymbolInModule(fp->handle, name2);
#else  /* NSLINKMODULE_OPTION_PRIVATE not defined */
	sym = NSLookupAndBindSymbol(name2);
#endif /* NSLINKMODULE_OPTION_PRIVATE not defined */
	/* XXX check return?? */

	fp->entry = (loadable_func_t *) NSAddressOfSymbol(sym);
	if (fp->entry != NULL)
	    goto found;
#endif /* TRY_UNDERSCORE defined */
#ifdef NSUNLINKMODULE_OPTION_NONE
	opt = NSUNLINKMODULE_OPTION_NONE;
#else  /* NSUNLINKMODULE_OPTION_NONE not defined */
	opt = FALSE;
#endif /* NSUNLINKMODULE_OPTION_NONE not defined */
	NSUnLinkModule(fp->handle, opt);
	/* XXX NSDestroyObjectFileImage(ofi); ? keep ref count?? */
	free(fp);
	return NULL;
    } /* not found w/o underscore */
#ifdef TRY_UNDERSCORE
 found:
#endif /* TRY_UNDERSCORE defined */

    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    return fp->entry;
} /* os_load */

void
unload(struct spec *sp) {
    struct func *fp, *pp;
    char name[1024];			/* XXX */
    int opt;

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

#ifdef NSUNLINKMODULE_OPTION_NONE
    opt = NSUNLINKMODULE_OPTION_NONE;
#else  /* NSUNLINKMODULE_OPTION_NONE not defined */
    opt = FALSE;
#endif /* NSUNLINKMODULE_OPTION_NONE not defined */
    NSUnLinkModule(fp->handle, FALSE);

    free(fp);				/* free name block */
}
