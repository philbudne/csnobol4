/* $Id$ */

/*
 * load and run external functions for systems using dlopen()/dlsym()
 * -plb 4/13/97
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>
#ifdef ANSI_STRINGS
#include <string.h>
#else  /* ANSI_STRINGS not defined */
#include <strings.h>                    /* berkeley strings */
#endif /* ANSI_STRINGS not defined */

/* XXX ifdef HAVE_STDLIB_H? Hasn't been needed, yet. */
#include <stdlib.h>			/* malloc(), getenv() */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"

#if __NetBSD__
/* Necessary on NetBSD 1.2 on a.out platforms */
#define TRY_UNDERSCORE
#endif /* __NetBSD__ defined */

#if __FreeBSD__
/* Necessary on FreeBSD 2.2.1-RELEASE */
#define TRY_UNDERSCORE
#endif /* __FreeBSD__ defined */

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
	char path[PATHLEN*2];		/* room for directory name */
	char *pp;			/* path pointer */
	char *snolib;
	int mode;

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

	/*
	 * SunOS4 (and others) only support LAZY mode.
	 * set RTLD_GLOBAL (if available)?? nah; avoid module collisions
	 */
#ifdef RTLD_LAZY
	mode = RTLD_LAZY;
#else  /* RTLD_LAZY not defined */
	/* Needed on FreeBSD 2.2.1-RELEASE */
	mode = 0;
#endif /* RTLD_LAZY not defined */

	fp->handle = dlopen(pp, mode);
	if (fp->handle == NULL) {
#ifdef DEBUG
	    /* XXX always? to stderr?? */
	    printf("dlopen: %s\n", dlerror());
#endif
	    free(fp);
	    return FALSE;		/* fail */
	}

	fp->entry = (int (*)(LOAD_PROTO)) dlsym(fp->handle, fp->name);
	if (fp->entry == NULL) {
#ifdef TRY_UNDERSCORE
	    char name2[1024];		/* XXX */
	    /*
	     * Ouch; NetBSD 1.2 (on pc532 at least, and probably all
	     * a.out based platforms) wants C functions with a leading
	     * underscore.  Rather than trying to figure out when and
	     * if this is needed at config time, just try it both ways.
	     *
	     * FreeBSD 2.2.1 has the same problem.
	     */

	    name2[0] = '_';
	    strncpy(name2+1, fp->name, sizeof(name2)-2);
	    name2[sizeof(name2)-1] = '\0';

	    fp->entry = (int (*)(LOAD_PROTO)) dlsym(fp->handle, name2);
	    if (fp->entry != NULL)
		goto found;
#endif /* TRY_UNDERSCORE defined */

	    dlclose(fp->handle);
	    free(fp);
	    return FALSE;
	} /* dlsym failed */
    } /* not found by pml */
 found:
    fp->self = fp;			/* make valid */

    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    D_A(addr) = (int_t) fp;
    D_F(addr) = D_V(addr) = 0;		/* clear flags, type */
    return TRUE;			/* success */
}

/* support for SIL "LINK" opcode -- call external function */
int
link(retval, args, nargs, addr)
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

    fp->self = 0;			/* invalidate self pointer!! */
    free(fp);				/* free name block */
}
