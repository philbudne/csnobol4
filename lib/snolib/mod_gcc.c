/*
 * $Id$
 * SNOBOL4 loadable module support
 * using GNU C __attribute__
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "snotypes.h"			/* struct descr */
#include "h.h"				/* EXPOIRT/IMPORT, TLS */
#include "load.h"			/* SNOLOAD_API */
#include "module.h"			/* struct module, MODULE_xxx */

static TLS struct module m = { MODULE_STRUCT_INIT };
struct TLS module *const module = &m;	/* read only pointer to mutable struct */

static void __attribute__((constructor))
init(void) {
    MODULE_INIT(module);
}

static void __attribute__((destructor))
cleanup(void) {
    MODULE_CLEANUP(module);
}
