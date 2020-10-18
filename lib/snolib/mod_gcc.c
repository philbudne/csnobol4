/*
 * $Id$
 * SNOBOL4 loadable module support
 * using GNU C __attribute__
 */

#include "snotypes.h"			/* struct descr */
#include "h.h"				/* EXPOIRT/IMPORT, TLS */
#include "load.h"			/* SNOEXP */
#define MODULE_SUPPORT
#include "handle.h"			/* struct module, MODULE_xxx */

TLS struct module module = { MODULE_STRUCT_INIT };

static void __attribute__((constructor))
init(void) {
    MODULE_INIT(module);
}

static void __attribute__((destructor))
cleanup(void) {
    MODULE_CLEANUP(module);
}
