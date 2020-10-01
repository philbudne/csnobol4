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

void *
os_load_library(char *file) {
    NSObjectFileImage ofi;
    NSSymbol sym;
    int opt;
    void *ptr;

    if (NSCreateObjectFileImageFromFile(file, &ofi) != 
	NSObjectFileImageSuccess)
	return NULL;

#ifdef NSLINKMODULE_OPTION_PRIVATE
    /* MacOS X; avoid symbol clashes */
    opt = NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW;
#else  /* NSLINKMODULE_OPTION_PRIVATE not defined */
    opt = TRUE;		 /* old "bindnow" */
#endif /* NSLINKMODULE_OPTION_PRIVATE not defined */
    ptr = NSLinkModule(ofi, file, opt);
    if (!ptr) {
	/* XXX NSDestroyObjectFileImage(ofi); ? keep ref count?? */
	return NULL;			/* fail */
    }
    return ptr;
}

void *
os_find_symbol(void *lib, char *func) {
    NSSymbol sym;

#ifdef NSLINKMODULE_OPTION_PRIVATE
    sym = NSLookupSymbolInModule(lib, func);
#else  /* NSLINKMODULE_OPTION_PRIVATE not defined */
    sym = NSLookupAndBindSymbol(func);
#endif /* NSLINKMODULE_OPTION_PRIVATE not defined */
    /* XXX check return?? */

    return NSAddressOfSymbol(sym);
} /* os_find_symbol */

void
os_unload_library(void *lib) {
#ifdef NSUNLINKMODULE_OPTION_NONE
    NSUnLinkModule(lib, NSUNLINKMODULE_OPTION_NONE);
#else  /* NSUNLINKMODULE_OPTION_NONE not defined */
    NSUnLinkModule(lib, FALSE);
#endif /* NSUNLINKMODULE_OPTION_NONE not defined */
}
