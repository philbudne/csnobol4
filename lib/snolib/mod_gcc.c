#include "snotypes.h"
#include "h.h"				/* __P() */
#include "load.h"			/* SNOEXP() */
#include "handle.h"

extern struct module module;

/*
 * Once upon a time user code declared _fini but that no longer seems
 * possible (now used to call desctructors?)  create mod_fini.c if
 * it's needed for older platforms.
 */

static void __attribute__((constructor))
module_init(void) {
    module.htlist = NULL;
}

static void __attribute__((destructor))
module_cleanup(void) {
    handle_cleanup(module.htlist);
}
