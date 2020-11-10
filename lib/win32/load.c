/* $Id$ */

/*
 * load and run external functions for Win32
 * -plb 1/5/98
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

void *
os_load_library(const char *lname) {
    /*
     * try loading given path;
     * system will scan various directories
     * (including appl dir, cwd, SYSTEM(32),
     * windows dir, and dirs in PATH var)
     *
     * after LoadLibrary("foo/bar.dll"),
     * LoadLibrary("bar") finds same library
     */
    return LoadLibrary(lname);
}

void
os_unload_library(void *handle) {
    FreeLibrary(handle);
}

void *
os_find_symbol(void *handle, const char *symbol, void **stash) {
    (void) stash;
    return GetProcAddress(handle, symbol);
}

void
os_unload_function(const char *name, void *stash) {
    (void) name;
    (void) stash;
}
