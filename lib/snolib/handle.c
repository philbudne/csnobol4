
/* $Id$ */

/*
 * manage lists of handles
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
};

struct handle_table {
    long entries;
    const char *name;
    handle_datatype_t datatype;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

/* void pointer to handle_number */
#define VP2HANDLENUM(VP) ((handle_number_t)(VP))

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
	    return (void *)hp->handle_number;
    }
    return NULL;
}

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

    h.f = 0;
    h.v = htp->datatype;
    h.a.i = VP2HANDLENUM(vp);

    /* if it already exists, just return handle */
    if (lookup_handle(hhp, h) != NULL)
	return h;

    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return bad_handle;

    hash = HANDLE_HASH(h.a.i);

    hp->next = htp->hash[hash];
    hp->handle_number = h.a.i;

    htp->hash[hash] = hp;
    htp->entries++;

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
