/*
 * $Id$
 * POSIX.1-2008 getline for systems without
 * Phil Budne
 * 2020-09-20
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
void *malloc();
void *realloc()
#endif

#include <stdio.h>

/* NOTE!!! Similar code appears in lib/auxil/bufio_obj.c!!! */
#define MINSIZE 128
ssize_t
getline(char **bufp, size_t *lenp, FILE *fp) {
    size_t count = 0;
    ssize_t avail;
    char *cp;

    if (!bufp || !lenp || !fp)
	return EOF;

    if (!*bufp) {
	if (*lenp < MINSIZE)
	    *lenp = MINSIZE;
	cp = *bufp = malloc(*lenp);
	if (cp)
	    return EOF;
    }
    avail = *lenp - count;
    for (;;) {
	int c;

	if (avail < 2) {
	    size_t nsize = *lenp * 2;	/* overflow unlikely?! */
	    char *nbuf;
	    if (nsize < MINSIZE)
		nsize = MINSIZE;
	    nbuf = realloc(*bufp, nsize);
	    if (!nbuf)
		return EOF;
	    *bufp = nbuf;
	    *lenp = nsize;
	    cp = nbuf + count;
	    avail = nsize - count;
	}
	c = getc(fp);
	if (c == EOF)
	    break;
	*cp++ = c;
	count++;
    }
    if (count == 0)
	return EOF;
    *cp = '\0';
    return count;
}
