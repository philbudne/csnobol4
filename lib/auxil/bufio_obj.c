//#define BUFIO_DEBUG
/*
 * $Id$
 * base class for line buffered input
 * for things that can't be wrapped using fdopen (winsock)
 * child class MUST define io_read_raw & io_write, but not io_read!!
 * Phil Budne
 * 9/13/2020
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>			/* malloc, abort */
#include <stdio.h>			/* NULL, size_t */
#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* ssize_t */
#endif

#include "h.h"				/* TRUE */
#include "io_obj.h"
#include "bufio_obj.h"

#ifdef BUFIO_DEBUG
#define DPRINTF(X) printf X
#else
#define DPRINTF(X)
#endif

static ssize_t
bufio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    fprintf(stderr, "%s io_read_raw not overridden\n", iop->ops->io_name);
    return -1;
}

/* helper for bufio_getc, a helper for bufio_getline */
static ssize_t
ioo_read_raw(struct bufio_obj *biop, char *buf, size_t len) {
    const struct io_ops *op;
    ssize_t ret = -1;

    for (op = biop->io.ops; op; op = op->io_super) {
	if (op->io_read_raw) {
	    ret = (op->io_read_raw)(&biop->io, biop->buffer, biop->buflen);
	    if (ret < 0)
		biop->eof = 1;
	    return ret;
	}
    }
    return ret;
}

/* helper for bufio_getline */
static int
bufio_getc(struct bufio_obj *biop) {
    if (biop->count == 0) {
	/*
	 * ASSuMEs that read_raw provider is like a socket
	 * and won't block until the entire request is filled.
	 * (if it won't, will have to restrict reads to 1 byte):
	 */
	DPRINTF(("bufio_getc: buffer %p len %zd\n", biop->buffer, biop->buflen));
	ssize_t count = ioo_read_raw(biop, biop->buffer, biop->buflen);
	DPRINTF(("bufio_getc: read_raw returned %zd\n", biop->count));
#ifdef DEBUG_BUFIO_READ_RAW
	{
	    printf("bufio_getc read_raw: ");
	    int i = biop->count;
	    char *cp = biop->buffer;
	    while (i-- > 0) {
		char c = *cp++;
		if (c < ' ') printf(" %#o", c);
		else printf(" %c", c);
	    }
	    putchar('\n');
	}
#endif
	/*
	 * NOTE! inetio read_raw returns -1 for EOF,
	 * so _COULD_ loop here (ie; continue if count == 0)
	 */
	if (count <= 0)
	    return -1;			/* EOF, or something like it */
	biop->bp = biop->buffer;	/* reset buffer pointer */
	biop->count = count;
    }
    biop->count--;
    return *biop->bp++ & 0xff;
}

/* NOTE!!! Similar code appears in lib/auxil/getline.c!!! */
static ssize_t
bufio_getline(struct io_obj *iop) {
    struct bufio_obj *biop = (struct bufio_obj *) iop;
    int count = 0;
    int avail;
    int c;
    char *cp = iop->linebuf;

    if (!cp) {
	cp = iop->linebuf = malloc(iop->linebufsize = 128);
	if (!cp)
	    return EOF;
    }
    avail = iop->linebufsize;
    do {
	if (avail < 2) {		/* need room for newline, NUL */
	    size_t nsize = iop->linebufsize * 2;
	    char *nbuf = realloc(iop->linebuf, nsize);
	    if (!nbuf)
		return EOF;
	    iop->linebuf = nbuf;
	    iop->linebufsize = nsize;
	    cp = iop->linebuf + count;
	    avail = iop->linebufsize - count;
	}
	c = bufio_getc(biop);
	//DPRINTF(("bufio_getc returned %d '%c'\n", c, c));
	if (c == EOF)
	    break;

	*cp++ = c;
	count++;
    } while (c != '\n');
    if (count == 0)
	return EOF;
    *cp = '\0';
#ifdef DEBUG_BUFIO_GETLINE
    {
	printf("bufio_getline: ");
	int i = count;
	char *cp = biop->io.linebuf;
	while (i-- > 0) {
	    char c = *cp++;
	    if (c < ' ') printf(" %#o", c & 0xff);
	    else printf(" %c", c);
	}
	putchar('\n');
    }
#endif
    return count;			/* excluding NUL */
}

static ssize_t
bufio_write(struct io_obj *iop, const char *buf, size_t len) {
    fprintf(stderr, "%s io_write not overridden\n", iop->ops->io_name);
    return -1;
}

static int
bufio_seeko(struct io_obj *iop, io_off_t off, int whence) {
    struct bufio_obj *biop = (struct bufio_obj *) iop;

    biop->count = 0;		     /* invalidate input buffer */
    return TRUE;
}

/* XXX use common dummyio_tello? */
static io_off_t
bufio_tello(struct io_obj *iop) {
    (void) iop;
    // fail silently?
    return -1;
}

static int
bufio_flush(struct io_obj *iop) {
    fprintf(stderr, "%s io_flush not overridden\n", iop->ops->io_name);
    return TRUE;
}

static int
bufio_eof(struct io_obj *iop) {
    struct bufio_obj *biop = (struct bufio_obj *) iop;
    return biop->eof;
}

static void
bufio_clearerr(struct io_obj *iop) {
    struct bufio_obj *biop = (struct bufio_obj *) iop;
    biop->eof = 0;			/* !! */
}

static int
bufio_close(struct io_obj *iop) {
    //struct bufio_obj *biop = (struct bufio_obj *) iop;
    // NOTE! linebuf freed in io.c:ioo_close()
    // child classes allocate & free bio.buffer (or not (memio))
    fprintf(stderr, "%s io_close not overridden\n", iop->ops->io_name);
    return TRUE;
}

MAKE_OPS(bufio, NULL);

/* virtual class: no open function */
