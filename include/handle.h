/* $Id$ */

typedef struct descr snohandle_t;	/* type of handles returned */
typedef struct handle_table *handle_handle_t;

#define LA_HANDLE(n) (*LA_DESCR(n))	/* returns snohandle_t */
#define OK_HANDLE(h) ((h).v != 0 && (h).a.i >= 0)
#define RETHANDLE(h) do { *retval = h; return TRUE; } while(0)

extern struct module_instance *const modinst; /* from SNOBOL4_MODULE */

SNOLOAD_API(void *) lookup_handle(handle_handle_t *, snohandle_t);
SNOLOAD_API(void) remove_handle(handle_handle_t *, snohandle_t);
SNOLOAD_API(snohandle_t) new_handle2(handle_handle_t *table,
				     void *value,
				     const char *name,
				     void (*release)(void*),
				     struct module_instance *mip);

/* for handle.c */
const char *handle_table_name(struct descr *dp, struct module_instance *mip);

