/* $Id$ */

/*
 * dummy functions for LOAD/LINK/UNLOAD
 * (now a worker for loadx.c)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "lib.h"

void *
os_load_library(char *file) {
    (void) file;
    return NULL;
}

void *
os_find_symbol(void *lib, char *func, void **stash) {
    (void) lib;
    (void) func;
    (void) stash;
    return NULL;
}

void
os_unload_function(const char *name, void *stash) {
    (void) name;
    (void) stash;
}

void
os_unload_library(void *lib) {
    (void) lib;
}
