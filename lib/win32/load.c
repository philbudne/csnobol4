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

#ifndef SNOLIB_FILE
#define SNOLIB_FILE "snolib.dll"
#endif

#ifndef SNOLIB_DIR
#define SNOLIB_DIR "."			/* XXX get from command line */
#endif

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

#define PATHLEN 256			/* XXX */
#define BS '\\'				/* exactly */

#define ABSPATH(CP) \
    ((CP)[0] == '/' || \
     (CP)[0] == BS || \
     isalpha((CP)[0]) && (CP)[1] == ':' && (CP)[2] == BS)

#define EXISTS(CP) (GetFileAttributesA(CP) != -1)

/*
 * copy and concatenate path components,
 * second source path may be NULL
 * converting slashes (required by LoadLibrary)
 * remove double /'s
 */

/* XXX return error if out of space? */
static void
pathcpy(dp, cc, sp1, sp2)
    char *dp, *sp1, *sp2;
    int cc;
{
    char sep1, sep2;
    char c, last;

    last = '\0';
    sep1 = sep2 = BS;

    cc--;				/* leave room for NUL */
    while (cc > 0 && (c = *sp1++)) {
	if (c == '/') {
	    /* remove repeated /'s */
	    if (last == '/')
		continue;
	    c = BS;
	}
	*dp++ = c;
	last = c;
	cc--;
    }
    if (sp2 && cc > 0) {
	/* add seperator, if needed */
	if (last != BS) {
	    *dp++ = BS;
	    cc--;
	}
	while (cc > 0 && (c = *sp2++)) {
	    /* remove repeated /'s */
	    if (last == '/')
		continue;
	    c = BS;
	}
	*dp++ = c;
	last = c;
	cc--;
    } /* sp2 && space */
    *dp = '\0';
}

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
	char *snolib;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

	if (sp2 && S_A(sp2) && S_L(sp2)) { /* have filename */
	    char temp[PATHLEN];

	    spec2str(sp2, temp, sizeof(temp));
	    if (!ABSPATH(temp) && !EXISTS(temp)) {
		/* not absolute and file does not exist; prepend libdir */
		pathcpy( path, sizeof(path), snolib, temp );
	    }
	    else {
		/* absolute or exists */
		pathcpy( path, sizeof(path), temp, NULL );
	    }
	}
	else {				/* no path */
	    pathcpy( path, sizeof(path), SNOLIB_FILE, NULL );
	}

	fp->handle = LoadLibrary(path);	/* do the deed */
	if (fp->handle == NULL) {
	    free(fp);
	    return FALSE;		/* fail */
	}

	fp->entry = (int (*)(LOAD_PROTO)) GetProcAddress(fp->handle, fp->name);
	if (fp->entry == NULL) {
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
