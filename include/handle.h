/* $Id$ */

typedef struct descr snohandle_t;	/* type of handles returned */
typedef void *handle_handle_t;		/* handle for handle.c internal use */

#define LA_HANDLE(n) (*LA_DESCR(n))	/* returns snohandle_t */
#define OK_HANDLE(h) ((h).v != 0 && (h).a.i != 0)
#define RETHANDLE(h) do { *retval = h; return TRUE; } while(0)

SNOEXP(snohandle_t) new_handle(handle_handle_t *, void *, const char *);
SNOEXP(void *) lookup_handle(handle_handle_t *, snohandle_t);
SNOEXP(void) remove_handle(handle_handle_t *, snohandle_t);
