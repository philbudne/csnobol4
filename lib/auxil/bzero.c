/* $Id$ */

/*
 * fancy generic bzero
 * Phil Budne
 * 7/25/97
 */

typedef unsigned int size_t;
typedef long word_t;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#define WSIZE sizeof(word_t)
#define WMASK (WSIZE-1)			/* WSIZE must be power of two! */

/*
 * XXX check for addr & length multiples of 8???
 *  (((int)cp)|length)&WMASK == 0 OR
 *  (((int)cp) & WMASK) == 0 && (length & WMASK) == 0
 *
 * in practice, snobol's use of bzero means input will 
 * ALWAYS be aligned (pointer to struct descr), and
 * ALWAYS be a multiple of sizeof (struct descr)
 */

void
bzero(register char *cp, size_t length) {
    if (length >= 2*WSIZE) {		/* enough to bother? */
	register word_t *wp;
	register size_t t;

	/* ensure cp is "word" aligned */
	t = WSIZE - (((int)cp) & WMASK);
	if (t) {
	    length -= t;
	    do {
		*cp++ = '\0';
	    } while (--t);
	}

	/* get count of words; non-zero due to above length check */
	t = length / WSIZE;
	wp = (word_t *)cp;
	do {
	    *wp++ = 0;
	} while (--t);
	cp = (char *)cp;
	length &= WMASK;		/* get straggler count */
    }

    for (; length; length--)
	*cp++ = '\0';
}
