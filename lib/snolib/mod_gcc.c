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
#define MODULE_SUPPORT			/* MODULE_{STRUCT_INIT,INIT,CLEANUP} */
#include "handle.h"			/* struct module, MODULE_... */

// EXPORT???
TLS struct module module = { MODULE_STRUCT_INIT };

static void __attribute__((constructor))
init(void) {
    MODULE_INIT(module);
}

static void __attribute__((destructor))
cleanup(void) {
    MODULE_CLEANUP(module);
}
