/* $Id$ */

/*
 * load and run external functions for Win32
 * -plb 1/5/98
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"

#include <windows.h>
#include <string.h>
#include <stdlib.h>			/* malloc(), getenv() */

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

struct func {
    struct func *next;			/* next in loaded function list */
    struct func *self;			/* for validity check */
    int (*entry)(LOAD_PROTO);		/* function entry point */
    HINSTANCE handle;			/* from LoadLibrary() */
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

#if 1
	/* punt for now */
	spec2str(sp2, path, sizeof(path));
	pp = path;
#else
	/* XXX need to check for both kinds of slash???? */
	char *snolib;
	char sep;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

/* XXX must match dir sep style used in snolib! */
	sep = '/';

	if (sp2 && S_A(sp2) && S_L(sp2)) {
	    char temp[PATHLEN];

	    spec2str(sp2, temp, sizeof(temp));

	    /* XXX just try LoadLibrary() ?? */
	    if (temp[0] != '/' &&
		!(isalpha(temp[0]) && temp[1] == ':') &&
		GetFileAttributesA(temp) == -1) {
		/* not absolute and file does not exist; prepend libdir */
		/* XXX limit length of snolib?? */
/* XXX snolib and temp could have differnt sep styles!!! */
		sprintf( path, "%s%c%s", snolib, sep, temp );
	    }
	    else
		strcpy( path, temp );
	    pp = path;
	}
	else {				/* no path */
	    /* XXX limit length of snolib?? */
	    sprintf( path, "%s%c%s", snolib, sep, SNOLIB_FILE );
	    pp = path;
	}
#endif

	fp->handle = LoadLibrary(pp);
	if (fp->handle == NULL) {
	    free(fp);
	    return FALSE;		/* fail */
	}

	fp->entry = (int (*)(LOAD_PROTO)) GetProcAddress(fp->handle, fp->name);
	if (fp->entry == NULL) {
/* XXX does windows keep a usecount?? */
	    FreeLibrary(fp->handle);
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

    FreeLibrary(fp->handle);

    fp->self = 0;			/* invalidate self pointer!! */
    free(fp);				/* free name block */
}
