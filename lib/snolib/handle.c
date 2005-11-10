
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

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

struct handle_entry {
    struct handle_entry *next;
    snohandle_t handle;
};

struct handle_list {
    long entries;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

SNOEXP(void *)
lookup_handle(hhp, h)
    handle_handle_t *hhp;
    snohandle_t h;
{
    struct handle_list *hlp = *hhp;
    struct handle_entry *hp;

    if (!hlp)
	return NULL;

    for (hp = hlp->hash[HANDLE_HASH(h)]; hp; hp = hp->next) {
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
    struct handle_list *hlp = *hhp;
    struct handle_entry *hp;

    if (!hlp) {
	hlp = malloc(sizeof(struct handle_list));
	if (!hlp)
	    return BAD_HANDLE;
	*hhp = hlp;
    }

    if (lookup_handle(hhp, (snohandle_t)vp))
	return BAD_HANDLE;

    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return BAD_HANDLE;

    hp->next = hlp->hash[HANDLE_HASH(hp->handle)];
    hp->handle = (snohandle_t)vp;

    hlp->hash[HANDLE_HASH(hp->handle)] = hp;
    hlp->entries++;

    return hp->handle;
}

SNOEXP(void)
remove_handle(hhp, h)
    handle_handle_t *hhp;
    snohandle_t h;
{
    struct handle_list *hlp = *hhp;
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h);

    if (!hlp)
	return;

    pp = NULL;
    for (hp = hlp->hash[hash]; hp; pp = hp, hp = hp->next) {
	if (hp->handle == h) {
	    if (pp)
		pp->next = hp->next;
	    else
		hlp->hash[hash] = hp->next;
	    free(hp);
	    hlp->entries--;
	    return;
	}
    }
}
