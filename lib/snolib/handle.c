
/* $Id$ */

/*
 * manage lists of handles for loadable code.
 * entries from each handle table are assigned a different
 * EXTERNAL datatype, validated on lookup.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>                     /* for malloc */
#include <stdio.h>			/* for NULL */

#include "snotypes.h"
#include "h.h"
#include "load.h"			/* SNOLOAD_API */
#include "handle.h"			/* prototypes */
#include "str.h"			/* bzero */

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

typedef unsigned int handle_datatype_t;	/* must fit in vfld */
static VAR handle_datatype_t next_handle_datatype;
static TLS char in_handle_cleanup;

typedef int_t handle_number_t;

struct handle_entry {
    struct handle_entry *next;
    handle_number_t handle_number;
    void *value;
};

struct handle_table {
    long entries;
    const char *name;			/* for debug */
    handle_datatype_t datatype;		/* SNOBOL4 EXTERNAL datatype */
    handle_number_t next;		/* next handle to hand out */
    void (*release)(void *value);
    struct handle_table *next_table;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

static const struct descr bad_handle;

SNOLOAD_API(void *)
lookup_handle(handle_handle_t *hhp, snohandle_t h) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;

    if (!htp)
	return NULL;

    /* printf("lookup_handle %s %#lx\n", htp->name, h.a.i); */
    if (h.v != htp->datatype)
	return NULL;

    for (hp = htp->hash[HANDLE_HASH(h.a.i)]; hp; hp = hp->next) {
	if (hp->handle_number == h.a.i)
	    return hp->value;
    }
    return NULL;
}

/*
 * user passes "handle handle", a pointer to a place for us to store a
 * pointer to a handle table (on the first lookup attempt).  Assign
 * datatype numbers from high to low (DATA types are assigned from low
 * to high)
 */
SNOLOAD_API(snohandle_t)
new_handle2(handle_handle_t *hhp, void *vp,
	    const char *tname, void (*release)(void *),
	    struct module *mp) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;
    struct descr h;
    int hash;

    if (!htp) {
	/* first time thru? create hash table */
	htp = malloc(sizeof(struct handle_table));
	if (!htp)
	    return bad_handle;
	bzero(htp, sizeof(struct handle_table));
	if (next_handle_datatype == 0)	/* first call? */
	    next_handle_datatype = SIZLIM;
	htp->datatype = --next_handle_datatype; /* assign datatype */
	htp->name = tname;
	htp->release = release;
	*hhp = htp;

	/* link into list of tables to pass to cleanup */
	if (mp) {
	    htp->next_table = mp->htlist;
	    mp->htlist = htp;
	}
    }

    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return bad_handle;


    hp->handle_number = htp->next++;
    hash = HANDLE_HASH(hp->handle_number);
    hp->next = htp->hash[hash];
    hp->value = vp;

    htp->hash[hash] = hp;
    htp->entries++;

    h.f = 0;
    h.v = htp->datatype;
    h.a.i = hp->handle_number;

    return h;
}

SNOLOAD_API(snohandle_t)
new_handle(handle_handle_t *hhp, void *vp, const char *tname) {
    static char complained = 0;
    if (!complained) {
	fprintf(stderr, "new_handle is deprecated, call new_handle2\n");
	complained = 1;
    }
    return new_handle2(hhp, vp, tname, NULL, NULL);
}

SNOLOAD_API(void)
remove_handle(handle_handle_t *hhp, snohandle_t h) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h.a.i);

    if (!htp || in_handle_cleanup)
	return;

    pp = NULL;
    for (hp = htp->hash[hash]; hp; pp = hp, hp = hp->next) {
	if (hp->handle_number == h.a.i) {
	    if (pp)
		pp->next = hp->next;
	    else
		htp->hash[hash] = hp->next;
	    free(hp);
	    htp->entries--;
	    return;
	}
    }
}

/* call to cleanup one table */
static void
handle_cleanup_table(struct handle_table *htp) {
    int i;

    if (!htp->release)
	return;

    /* XXX keep per-table? nah */
    in_handle_cleanup = 1;	/* guard against remove_handle! */
    for (i = 0; i < HANDLE_HASH_SIZE; i++) {
	struct handle_entry *hp, *next;
	for (hp = htp->hash[i]; hp; hp = next) {
	    next = hp->next;
	    /* SHOULD NOT call remove_handle! */
	    (htp->release)(hp->value);
	    free(hp);
	}
	htp->hash[i] = NULL;
    }
    htp->entries = 0;
    in_handle_cleanup = 0;
    free(htp);
}

/* called on module unload (initiated by mod_xxx, not user) */
SNOLOAD_API(void)
module_cleanup(struct module *mp) {
    struct handle_table *htp = mp->htlist;
#ifdef DEBUG
    printf("module_cleanup\n");
#endif
    while (htp) {
	struct handle_table *next = htp->next_table;
	handle_cleanup_table(htp);
	htp = next;
    }
}
