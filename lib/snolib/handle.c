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

void *
lookup_handle(hlp, h)
    struct handle_list *hlp;
    snohandle_t h;
{
    struct handle_entry *hp;

    for (hp = hlp->hash[HANDLE_HASH(h)]; hp; hp = hp->next) {
	if (hp->handle == h)
	    return hp->value;
    }
    return NULL;
}

snohandle_t
new_handle(hlp, vp)
    struct handle_list *hlp;
    void *vp;
{
    snohandle_t h;
    struct handle_entry *hp;

    /* find a free handle */
    /* XXX save initial "next" value, avoid infinite loop! */
    do {
	h = hlp->next++;
    } while (h == BAD_HANDLE || lookup_handle(hlp, h));

    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return -1;

    hp->value = vp;
    hp->next = hlp->hash[HANDLE_HASH(hp->handle)];
    hp->handle = h;

    hlp->hash[HANDLE_HASH(hp->handle)] = hp;
    hlp->entries++;

    return hp->handle;
}

void
remove_handle(hlp, h)
    struct handle_list *hlp;
    snohandle_t h;
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
