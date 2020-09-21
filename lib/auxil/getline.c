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
ssize_t
getline(char **bufp, size_t *lenp, FILE *fp) {
    int count = 0;
    int avail;
    char *cp;

    if (!bufp || !lenp || !fp)
	return EOF;

    if (!*bufp) {
	if (*lenp < 128)
	    *lenp = 128;
	cp = *bufp = malloc(*lenp);
	if (cp)
	    return EOF;
    }
    avail = *lenp - count;
    for (;;) {
	int c;

	if (avail < 2) {
	    size_t nsize = iop->linebufsize * 2;
	    char *nbuf = realloc(iop->linebuf, nsize);
	    if (!nbuf)
		return EOF;
	    iop->linebuf = nbuf;
	    iop->linebufsize = nsize;
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
