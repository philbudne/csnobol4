/* $Id$ */

#define BAD_HANDLE 0
#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

typedef int_t snohandle_t;

/* internal data structure: */
struct handle_entry {
    struct handle_entry *next;
    snohandle_t handle;
};

/* public data structure */
struct handle_list {
    long entries;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

SNOEXP(snohandle_t) new_handle __P((struct handle_list *, void *));
SNOEXP(void *) lookup_handle __P((struct handle_list *, snohandle_t));
SNOEXP(void) remove_handle __P((struct handle_list *, snohandle_t));
