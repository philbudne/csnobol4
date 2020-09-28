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

loadable_func_t
os_load(char *func, char *file) {
    return NULL;
} /* os_load */

void
unload(struct spec *sp) {
}
