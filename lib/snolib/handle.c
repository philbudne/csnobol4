
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
#include "h.h"				/* __P() */
#include "load.h"			/* SNOEXP() */
#include "handle.h"
#include "str.h"

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

typedef unsigned int handle_datatype_t;	/* must fit in vfld */
static handle_datatype_t next_handle_datatype = SIZLIM;

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
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

static const struct descr bad_handle;

SNOEXP(void *)
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
SNOEXP(snohandle_t)
new_handle(handle_handle_t *hhp, void *vp, const char *tname) {
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
	htp->datatype = --next_handle_datatype; /* assign datatype */
	htp->name = tname;
	*hhp = htp;
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

SNOEXP(void)
remove_handle(handle_handle_t *hhp, snohandle_t h) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h.a.i);

    if (!htp)
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
