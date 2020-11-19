/*
 * system independant wrapper for LOAD()
 * calls os_load_library, os_unload_library, os_find_symbol
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>			/* free() */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"			/* SNOLOAD_API (for module.h) */
#include "module.h"			/* structs */
#include "path.h"
#include "lib.h"			/* mspec2str() */
#include "str.h"

struct lib {
    struct lib *next;			/* libs list */
    void *oslib;			/* object from os_load_library */
    struct module *module;		/* see module.h */
    int refcount;
    char *name;
    char *path;
};

struct func {
    struct func *next;			/* funcs list */
    struct func *self;			/* for validity check */
    loadable_func_t *entry;		/* function entry point */
    struct lib *lib;
    void *stash;			/* for use by os_find_symbol */
    char name[1];			/* for unload (MUST BE LAST)! */
};

/* list of loaded functions (for UNLOAD) */
static VAR struct func *funcs;

/*
 * list of loaded libraries (depends on system ref-counting)
 * COULD keep just one, but would require a lock
 */
static VAR struct lib *libs;

#ifdef SHARED
static void loadx_cleanup(void);
#endif

/* create refcounted lib interface */

static struct lib *
libopen(char *name, char *path) {
    void *oslib;
    struct lib *lp;

    /* check for duplicate by ORIGINAL name/path */
    for (lp = libs; lp; lp = lp->next)
	if (strcmp(name, lp->name) == 0)
	    break;

    if (!lp) {
	oslib = os_load_library(path);
	if (oslib == NULL)
	    return NULL;
	/*
	 * defend against multiple paths to same file
	 * Also defend against Windows, where once foo/bar.dll is loaded,
	 * "bar" seems to suffice
	 */
	for (lp = libs; lp; lp = lp->next) {
	    if (oslib == lp->oslib) {
		os_unload_library(oslib); /* reduce use count */
		goto found;
	    }
	}

	lp = (struct lib *) malloc(sizeof(struct lib));
	lp->oslib = oslib;
	lp->refcount = 0;

	lp->name = strdup(name);	/* save ORIGINAL name/path */
	lp->path = strdup(path);
	lp->module = os_find_symbol(oslib, "module", NULL);
	/* XXX error if lookup fails when SHARED (&& THREADS)? */
	/* XXX complain regardless? */

	lp->next = libs;
	libs = lp;

	if (lp->module)
	    module_instance_init(lp->module); /* XXX check return? */
    }
 found:
    lp->refcount++;
    return lp;
}

static int
libclose(struct lib *lib) {
    struct lib *lp, *pp;

    if (--lib->refcount > 0)
	return 1;

    /* find previous, if any */
    for (lp = libs, pp = NULL; lp && lp != lib; pp = lp, lp = lp->next)
	;

    if (!lp)
	return 0;			/* not found */

    if (lp->module)
	module_instance_cleanup(lp->module);

    /* detach library */
    os_unload_library(lp->oslib);

    /* unlink from list */
    if (pp)
	pp->next = lp->next;
    else
	libs = lp->next;
    free(lp->name);
    free(lp->path);
    free(lp);

    return 1;
}

/* support for SIL "LOAD" opcode -- load external function */
int
load(struct descr *addr,		/* OUT */
     struct spec *sp1,			/* function */
     struct spec *sp2) {		/* library */
    char *fname = mspec2str(sp1);
    char *lname = mspec2str(sp2);
    loadable_func_t *entry;		/* function address */
    struct func *fp = NULL;
    struct lib *lp = NULL;
    int ret = FALSE;
    void *stash = NULL;
#ifdef SHARED
    static VAR char registered;
    if (!registered) {
	reg_cleanup(loadx_cleanup);
	registered = 1;
    }
#endif
    /* always try PML first. Only if lname is empty?? */
    /*if (!lname || !*lname)*/
    entry = pml_find(fname);

    if (!entry) {
	char *l2;

	lp = libopen(lname, lname);
	if (lp)
	    goto found_lib;

#ifdef DL_EXT
	l2 = strjoin(lname, DL_EXT, NULL);
	if (!l2)
	    return FALSE;
	lp = libopen(lname, l2);
	free(l2);
	if (lp)
	    goto found_lib;

	if (!abspath(lname)) {
	    l2 = io_lib_find("shared", lname, DL_EXT);
	    if (l2) {
		lp = libopen(lname, l2);
		free(l2);
		if (lp)
		    goto found_lib;
	    }

	    l2 = io_lib_find("dynload", lname, DL_EXT);
	    if (l2) {
		lp = libopen(lname, l2);
		free(l2);
		if (lp)
		    goto found_lib;
	    }

	    l2 = io_lib_find(NULL, lname, DL_EXT);
	    if (l2) {
		lp = libopen(lname, l2);
		free(l2);
	    }
	    if (!lp)
		goto quit;
	} // !abspath
#endif
    found_lib:
	entry = os_find_symbol(lp->oslib, fname, &stash);
	if (!entry) {
#ifdef TRY_UNDERSCORE
	    l2 = strjoin("_", fname, NULL);
	    if (!l2)
		goto freelib;
	    entry = os_find_symbol(lp->oslib, l2, &stash);
	    free(l2);
	    if (!entry)
#endif
		goto freelib;
	}
    } /* not found by pml_find */

    /* here with entry */
    fp = (struct func *) malloc( sizeof (struct func) + strlen(fname));
    if (fp == NULL) {
    freelib:
	libclose(lp);
	goto quit;
    }
    strcpy(fp->name, fname);
    fp->lib = lp;
    fp->entry = entry;
    fp->stash = stash;
    fp->self = fp;			/* make valid */

    fp->next = funcs;			/* link into list */
    funcs = fp;

    D_A(addr) = (int_t) fp;
    D_F(addr) = D_V(addr) = 0;		/* clear flags, type */
    ret = TRUE;

quit:
    free(fname);
    free(lname);
    return ret;
}

/* support for SIL "LINK" opcode -- call external function */
int
callx(struct descr *retval, struct descr *args,
      struct descr *nargs, struct descr *addr) {
    struct func *fp;

    /* XXX check for zero V & F fields?? */
    fp = (struct func *) D_A(addr);
    if (fp == NULL)
        return FALSE;

    if (fp->self != fp)                 /* validate, in case unloaded */
        return FALSE;                   /* fail (fatal error??) */

    if (!fp->entry)
	return FALSE;			/* fail (fatal error??) */

    /* check fp->lib && fp->lib->abivers */
    return (fp->entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}

static void
funload(char *fname) {
    struct func *fp, *pp;

    if (!fname)
	return;

    for (pp = NULL, fp = funcs; fp != NULL; pp = fp, fp = fp->next) {
	if (strcmp(fp->name, fname) == 0)
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

    os_unload_function(fp->name, fp->stash);

    libclose(fp->lib);

    fp->self = NULL;			/* invalidate self pointer!! */
    fp->entry = NULL;			/* invalidate function pointer */
    fp->lib = NULL;
    free(fp);				/* free name block */
}

void
unload(struct spec *sp) {
    char *fname = mspec2str(sp);
    funload(fname);
    free(fname);
}

#ifdef SHARED
static void
loadx_cleanup(void) {
    while (funcs)
	funload(funcs->name);
}
#endif

#include "equ.h"
#include "handle.h"

pmlret_t
EXTERNAL_DATATYPE( LA_ALIST ) {
    struct descr *dp = LA_DESCR(0);
    struct lib *lp;

    (void) nargs;
    if (!dp)
	RETFAIL;

    for (lp = libs; lp; lp = lp->next) {
	struct module_instance *mip;

	if (!lp->module)
	    continue;
	mip = (lp->module->get_module_instance)();
	if (mip) {
	    const char *type_name = handle_table_name(dp, mip);
	    if (type_name)
		RETSTR(type_name);
	}	    
    }
    RETFAIL;
}

pmlret_t
EXTERNAL_MODULE_NAME( LA_ALIST ) {
    struct descr *dp = LA_DESCR(0);
    struct lib *lp;

    (void) nargs;
    if (!dp)
	RETFAIL;

    for (lp = libs; lp; lp = lp->next) {
	struct module_instance *mip;

	if (!lp->module)
	    continue;
	mip = (lp->module->get_module_instance)();
	if (mip) {
	    if (handle_table_name(dp, mip))
		RETSTR(lp->module->name);
	}	    
    }
    RETFAIL;
}

/* what was used to load; may not be full path */
pmlret_t
EXTERNAL_MODULE_PATH( LA_ALIST ) {
    struct descr *dp = LA_DESCR(0);
    struct lib *lp;

    (void) nargs;
    if (!dp)
	RETFAIL;

    for (lp = libs; lp; lp = lp->next) {
	struct module_instance *mip;

	if (!lp->module)
	    continue;
	mip = (lp->module->get_module_instance)();
	if (mip) {
	    if (handle_table_name(dp, mip))
		RETSTR(lp->path);
	}	    
    }
    RETFAIL;
}
