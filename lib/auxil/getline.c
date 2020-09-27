/*
 * $Id$
 * POSIX.1-2008 getline for systems without
 * Phil Budne
 * 2020-09-20
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

/* NOTE!!! Similar code appears in lib/auxil/bufio_obj.c!!! */
#define MINSIZE 128
ssize_t
getline(char **bufp, size_t *lenp, FILE *fp) {
    size_t count = 0;
    ssize_t avail;
    char *cp;
    int c;

    if (!bufp || !lenp || !fp)
	return EOF;

    if (!*bufp) {
	*bufp = malloc(*lenp = MINSIZE);
	if (!*bufp)
	    return EOF;
    }
    cp = *bufp;
    avail = *lenp - count;
    do {
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
    } while (c != '\n');
    if (count == 0)
	return EOF;
    *cp = '\0';
    return count;			/* excluding NUL */
}

#ifdef TEST
int
main() {
    char *buf = NULL;
    size_t len = 0;

    printf("getline: %zd\n", getline(&buf, &len, stdin));
    printf("line: %s", buf);
}
#endif

