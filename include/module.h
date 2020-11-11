/*
 * $Id$
 */

/*
 * The only part of this file that should be considered public
 * is the SNOBOL4_MODULE macro.  Please ignore the rest!
 */

struct module {
    /* NOTE: all new data types start on x8 boundary */
    unsigned short abi_version;		/* major*100 + minor */
    unsigned short sizeof_module;
    unsigned short sizeof_long;
    unsigned short sizeof_ptr;
    unsigned short sizeof_modinst;
    unsigned short reserved_short[3];
    const char *name;
    int flags;
    int refcount;			/* needs locking! */
    struct module_instance *(*get_module_instance)(void);
    /* end of data in abi_version == 100 */
};


struct module_instance_private ;

struct module_instance {
    struct module *mod;
    struct module_instance *next;
    struct module_instance_private *priv;
};

#define MF_INITIALIZED	0x00000001
/*#define MF_THREADED	0x00000002*/
/* XXX flag to prevent unload? */

#ifndef SNOBOL4				/* building module... */

#ifdef IS_THREADED
#define SNOMOD_THREADED MF_THREADED
#else
#define SNOMOD_THREADED 0
#endif

#define MODULE_FLAGS (SNOMOD_THREADED)	/* X|Y|Z */

#define MODULE_STRUCT_INIT(NAME) \
	100, \
	(unsigned short)sizeof(struct module), \
	(unsigned short)sizeof(long), \
	(unsigned short)sizeof(void *), \
	(unsigned short)sizeof(struct module_instance), \
	0, 0, 0, \
        #NAME, \
	MODULE_FLAGS, 0, \
	get_module_instance

#define MODULE_INSTANCE_STRUCT_INIT \
	&module, \
	NULL, \
	NULL

#define MODULE_INSTANCE_FORWARDS \
    static struct module_instance *get_module_instance(void);

/* called via module->call_module_instance_xxx */
#define MODULE_INSTANCE_FUNCTIONS \
    static struct module_instance *get_module_instance(void) { return &mi; }

#define SNOBOL4_MODULE(NAME) \
    MODULE_INSTANCE_FORWARDS \
    EXPORT(struct module) module = { MODULE_STRUCT_INIT(NAME) }; \
    static TLS struct module_instance mi = { MODULE_INSTANCE_STRUCT_INIT }; \
    struct TLS module_instance *const modinst = &mi; \
    MODULE_INSTANCE_FUNCTIONS

/* read only pointer to mutable data */
extern TLS struct module_instance *const modinst;

#endif /* building module */
