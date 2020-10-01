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

void *
os_load_library(const char *lname) {
    /*
     * try loading given path;
     * system will scan various directories
     * (including appl dir, cwd, SYSTEM(32),
     * windows dir, and dirs in PATH var)
     */
    return LoadLibrary(lname);
}

void
os_unload_library(void *handle) {
    FreeLibrary(handle);
}

void *
os_find_symbol(void *handle, const char *symbol) {
    return GetProcAddress(handle, symbol);
}
