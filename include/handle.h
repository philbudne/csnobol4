/* $Id$ */

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */
#define HANDLE_HASH(H) ((H) & (HANDLE_HASH_SIZE-1))

#define BAD_HANDLE -1

typedef int_t handle_t;

struct handle_list {
    handle_t next;
    long entries;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

struct handle_entry {
    struct handle_entry *next;
    handle_t handle;
    void *value;
};

SNOEXP(handle_t) new_handle __P((struct handle_list *, void *));
SNOEXP(void *) lookup_handle __P((struct handle_list *, handle_t));
SNOEXP(void) remove_handle __P((struct handle_list *, handle_t));
