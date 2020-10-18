/* $Id$ */

typedef struct descr snohandle_t;	/* type of handles returned */
typedef struct handle_table *handle_handle_t;

#define LA_HANDLE(n) (*LA_DESCR(n))	/* returns snohandle_t */
#define OK_HANDLE(h) ((h).v != 0 && (h).a.i >= 0)
#define RETHANDLE(h) do { *retval = h; return TRUE; } while(0)

/*
 * new in 2.2
 * here because new_handle2 needs module extern
 */

struct module {
    struct handle_table *htlist;
    unsigned short api_version;		/* major*100 + minor */
    char threaded;
};

extern struct module module;		/* for loadables */

SNOEXP(void *) lookup_handle(handle_handle_t *, snohandle_t);
SNOEXP(void) remove_handle(handle_handle_t *, snohandle_t);
SNOEXP(snohandle_t) new_handle2(handle_handle_t *table,
				void *value,
				const char *name,
				void (*release)(void*),
				struct module *mp);

/* deprecated: */
SNOEXP(snohandle_t) new_handle(handle_handle_t *, void *, const char *);

/*
 * NOT FOR USER USE!! (called from mod_XXX.c)
 */
#ifdef MODULE_SUPPORT
SNOEXP(void) module_cleanup(struct module *);
#define IS_THREADED 0			/* 1 if TLS is thread-local storage */

#define MODULE_STRUCT_INIT NULL, 100, IS_THREADED
#define MODULE_INIT(MOD) (MOD).htlist = NULL
#define MODULE_CLEANUP(MOD) module_cleanup(&(MOD))
#endif
