
/* $Id$ */

/*
 * manage lists of handles
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H                    /* before stdio */
#include <stdlib.h>                     /* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>			/* for NULL */

#include "snotypes.h"
#include "h.h"				/* __P() */
#include "load.h"			/* SNOEXP() */
#include "handle.h"
#include "str.h"

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

struct handle_entry {
    struct handle_entry *next;
    snohandle_t handle;
};

struct handle_table {
    long entries;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

#define VP2HANDLE(VP) ((snohandle_t)(VP))

SNOEXP(void *)
lookup_handle(hhp, h)
    handle_handle_t *hhp;
    snohandle_t h;
{
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;

    if (!htp)
	return NULL;

    for (hp = htp->hash[HANDLE_HASH(h)]; hp; hp = hp->next) {
	if (hp->handle == h)
	    return (void *)hp->handle;
    }
    return NULL;
}

SNOEXP(snohandle_t)
new_handle(hhp, vp)
    handle_handle_t *hhp;
    void *vp;
{
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;

    if (!htp) {
	/* first time thru? create hash table */
	htp = malloc(sizeof(struct handle_table));
	if (!htp)
	    return BAD_HANDLE;
	bzero(htp, sizeof(struct handle_table));
	*hhp = htp;
    }

    /* if it already exists, just return handle */
    if (lookup_handle(hhp, VP2HANDLE(vp)) != NULL)
	return VP2HANDLE(vp);

    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return BAD_HANDLE;

    hp->next = htp->hash[HANDLE_HASH(hp->handle)];
    hp->handle = VP2HANDLE(vp);

    htp->hash[HANDLE_HASH(hp->handle)] = hp;
    htp->entries++;

    return hp->handle;
}

SNOEXP(void)
remove_handle(hhp, h)
    handle_handle_t *hhp;
    snohandle_t h;
{
    struct handle_table *htp = *hhp;
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h);

    if (!htp)
	return;

    pp = NULL;
    for (hp = htp->hash[hash]; hp; pp = hp, hp = hp->next) {
	if (hp->handle == h) {
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
