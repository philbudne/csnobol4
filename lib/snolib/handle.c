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

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

void *
lookup_handle(hlp, h)
    struct handle_list *hlp;
    snohandle_t h;
{
    struct handle_entry *hp;

    for (hp = hlp->hash[HANDLE_HASH(h)]; hp; hp = hp->next) {
	if (hp->handle == h)
	    return (void *)hp->handle;
    }
    return NULL;
}

snohandle_t
new_handle(hlp, vp)
    struct handle_list *hlp;
    void *vp;
{
    struct handle_entry *hp;

    if (lookup_handle(hlp, vp))
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

void
remove_handle(hlp, h)
    struct handle_list *hlp;
    void *h;
{
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h);

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
