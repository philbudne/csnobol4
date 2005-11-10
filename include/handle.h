/* $Id$ */

#define BAD_HANDLE 0

typedef int_t snohandle_t;		/* type of handles returned */
typedef void *handle_handle_t;		/* handle for handle.c internal use */

SNOEXP(snohandle_t) new_handle __P((handle_handle_t *, void *));
SNOEXP(void *) lookup_handle __P((handle_handle_t *, snohandle_t));
SNOEXP(void) remove_handle __P((handle_handle_t *, snohandle_t));
