/*
 * $Id$
 */

/*
 * Not a public interface, but is part of the module/snobol4 ABI
 * On the loadable side, this file should only be included by module
 * support files.
 */
struct module {
    unsigned short abi_version;		/* major*100 + minor */
    unsigned short sizeof_module;
    unsigned short sizeof_long;
    unsigned short sizeof_ptr;
    char threaded;
    char reserved[7];
    struct handle_table *htlist;
};


/*
 * NOT FOR USER USE!! (called from mod_XXX.c)
 */
SNOLOAD_API(void) module_init(struct module *);
SNOLOAD_API(void) module_cleanup(struct module *);

#ifndef IS_THREADED
#define IS_THREADED 0			/* 1 if TLS is thread-local storage */
#endif

#define MODULE_STRUCT_INIT \
	100, \
	(unsigned short)sizeof(struct module), \
	(unsigned short)sizeof(long), \
	(unsigned short)sizeof(void *), \
	IS_THREADED, 0, 0, 0, \
	0, 0, 0, 0, \
	NULL

#define MODULE_INIT(MOD)  module_init(MOD)
#define MODULE_CLEANUP(MOD) module_cleanup(MOD)
