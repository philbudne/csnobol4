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
#include <sys/stat.h>

/* NS: /NextDeveloper/Headers/mach-o/dyld.h */
#include <mach-o/dyld.h>

#ifdef ANSI_STRINGS
#include <string.h>
#else  /* ANSI_STRINGS not defined */
#include <strings.h>                    /* berkeley strings */
#endif /* ANSI_STRINGS not defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* malloc(), getenv() */
#endif /* HAVE_STDLIB_H */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

struct func {
    struct func *next;			/* next in loaded function list */
    struct func *self;			/* for validity check */
    int (*entry)(LOAD_PROTO);		/* function entry point */
    NSModule handle;			/* from NSLinkModule */
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

    l1 = S_L(sp1);
    fp = (struct func *) malloc( sizeof (struct func) + l1 );
    if (fp == NULL)
	return FALSE;			/* fail */

    strncpy( fp->name, S_SP(sp1), l1 );
    fp->name[l1] = '\0';
    fp->handle = NULL;			/* assume internal! */

    /* try "poor mans load" first!!! */
    fp->entry = pml_find(fp->name);
    if (fp->entry == NULL) {		/* not found by pml */
	NSObjectFileImage ofi;
	NSSymbol sym;
	char path[PATHLEN*2];		/* room for directory name */
	char *pp;			/* path pointer */
	char *snolib;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

	if (sp2 && S_A(sp2) && S_L(sp2)) {
	    struct stat st;
	    char temp[PATHLEN];

	    spec2str(sp2, temp, sizeof(temp));

	    /* XXX just try dlopen() ?? */
	    if (temp[0] != '/' && stat(temp, &st) < 0) {
		/* not absolute and file does not exist; prepend libdir */
		/* XXX limit length of snolib?? */
		sprintf( path, "%s/%s", snolib, temp );
	    }
	    else
		strcpy( path, temp );
	    pp = path;
	}
	else {				/* no path */
	    /* XXX limit length of snolib?? */
	    sprintf( path, "%s/%s", snolib, SNOLIB_FILE );
	    /* XXX just pass NULL pathname to dlopen (search main program)? */
	    pp = path;
	}

	if (NSCreateObjectFileImageFromFile(pp, &ofi) ==
	    NSObjectFileImageSuccess) {
	    int opt;
#ifdef NSLINKMODULE_OPTION_PRIVATE
	    /* MacOS X; avoid symbol clashes */
	    opt = NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW;
#else
	    opt = TRUE;			/* old "bindnow" */
#endif
	    fp->handle = NSLinkModule(ofi, pp, opt);
	    if (!fp->handle) {
		/* XXX NSDestroyObjectFileImage(ofi); ? keep ref count?? */
		free(fp);
		return FALSE;		/* fail */
	    }
	}
	else {
	    free(fp);
	    return FALSE;		/* fail */
	}

#ifdef NSLINKMODULE_OPTION_PRIVATE
	sym = NSLookupSymbolInModule(fp->handle, fp->name);
#else
	sym = NSLookupAndBindSymbol(fp->name);
#endif
	/* XXX check return?? */

	fp->entry = (int (*)(LOAD_PROTO)) NSAddressOfSymbol(sym);
	if (fp->entry == NULL) {
	    int opt;
#ifdef TRY_UNDERSCORE
	    char name2[1024];		/* XXX */

	    name2[0] = '_';
	    strncpy(name2+1, fp->name, sizeof(name2)-2);
	    name2[sizeof(name2)-1] = '\0';

#ifdef NSLINKMODULE_OPTION_PRIVATE
	    sym = NSLookupSymbolInModule(fp->handle, name2);
#else
	    sym = NSLookupAndBindSymbol(name2);
#endif
	/* XXX check return?? */

	    fp->entry = (int (*)(LOAD_PROTO)) NSAddressOfSymbol(sym);
	    if (fp->entry != NULL)
		goto found;
#endif /* TRY_UNDERSCORE defined */
#ifdef NSUNLINKMODULE_OPTION_NONE
	    opt = NSUNLINKMODULE_OPTION_NONE;
#else
	    opt = FALSE;
#endif
	    NSUnLinkModule(fp->handle, opt);
	    /* XXX NSDestroyObjectFileImage(ofi); ? keep ref count?? */
	    free(fp);
	    return FALSE;
	} /* dlsym failed */
    } /* not found by pml */
#ifdef TRY_UNDERSCORE
 found:
#endif
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

    /* XXX check for zero V & F fields?? */
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
    char name[1024];			/* XXX */
    int opt;

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

#ifdef NSUNLINKMODULE_OPTION_NONE
    opt = NSUNLINKMODULE_OPTION_NONE;
#else
    opt = FALSE;
#endif
    NSUnLinkModule(fp->handle, FALSE);

    fp->self = 0;			/* invalidate self pointer!! */
    free(fp);				/* free name block */
}
