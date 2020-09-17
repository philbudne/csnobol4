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

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* malloc, abort */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */
#include <stdio.h>			/* NULL, size_t */

#include "h.h"				/* TRUE */
#include "io_obj.h"
#include "bufio_obj.h"

#define MINBUFLEN 4096

static ssize_t
ioo_read_raw(struct bufio_obj *biop, char *buf, size_t len) {
    const struct io_ops *op;
    ssize_t ret = -1;

    for (op = biop->io.ops; op; op = op->io_super) {
	if (op->io_read_raw) {
	    ret = (op->io_read_raw)(&biop->io, biop->buffer, biop->buflen);
	    if (ret <= 0)		/* zero is socket EOF */
		biop->eof = 1;
	    return ret;
	}
    }
    return ret;
}

static int
bufio_getc(biop)
    struct bufio_obj *biop;
{
    if (biop->count <= 0) {
	/*
	 * ASSuMEs that read_raw provider is like a socket
	 * and won't block until the entire request is filled.
	 * (if it won't, will have to restrict reads to 1 byte):
	 */
	biop->count = ioo_read_raw(biop, biop->buffer, biop->buflen);
	if (biop->count <= 0)
	    return -1;			/* EOF, or something like it */
	biop->bp = biop->buffer;	/* reset buffer pointer */
    }
    biop->count--;
    return *biop->bp++ & 0xff;
}

/****************************************************************
 * I/O methods
 * (child MUST supply io_write, io_read_raw)
 */

static ssize_t
bufio_read(struct io_obj *iop, char *buf, size_t len) {
    struct bufio_obj *biop = (struct bufio_obj *)iop;
    int c;				/* character or -1 */
    int n = 0;				/* characters kept */
    int nread = 0;			/* characters read */
    int eol = FALSE;			/* eol seen */

    if (len == 0)
	return 0;

    if (iop->flags & (FL_BINARY|FL_UNBUF))
	return ioo_read_raw(biop, buf, len); /* uncut */

    if (biop->buflen == 0) {	       /* first time? */
	/* assumes recl can't change */
	if (len < MINBUFLEN)
	    biop->buflen = MINBUFLEN;
	else
	    biop->buflen = len;
	biop->buffer = malloc(biop->buflen); 
	if (!biop->buffer)
	    return -1;
    }

    while (n < len && !eol && (c = bufio_getc(biop)) != -1) {
	/* guard against empty lines looking like read failures! */
	nread++;

	if (c == '\r')
	    continue;			/* always discard CR */

	if (c == '\n') {
	    eol = TRUE;
	    if (iop->flags & FL_EOL)	/* dropping EOL? */
		break;			/* done */
	    /* fall thru to store char, exit at top */
	}

	*buf++ = c;
	n++;
    } /* while */

    if (nread == 0)			/* did not see ANYTHING? */
	return -1;			/* return error */

    /*
     * maintain record flavoredness; if EOL not seen, the line
     * was longer than our record length.  Discard rest of "record"
     */
    if (!eol && !(iop->flags & FL_BREAK)) { /* didn't see EOL */
	while ((c = bufio_getc(biop)) != EOF && c != '\n')
	    ;
    }
    return n;
}

/* XXX use common dummyio_write? */
static ssize_t
bufio_write(struct io_obj *iop, char *buf, size_t len) {
    printf("%s io_write not overridden\n", iop->ops->io_name);
    abort();
}

/* XXX use common dummyio_seeko? */
static int
bufio_seeko(struct io_obj *iop, off_t off, int whence) {
    (void) iop;
    // fail silently?
    return FALSE;
}

/* XXX use common dummyio_tello? */
static off_t
bufio_tello(struct io_obj *iop) {
    (void) iop;
    // fail silently?
    return -1;
}

static int
bufio_flush(struct io_obj *iop) {
    printf("%s io_flush not overridden\n", iop->ops->io_name);
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
    struct bufio_obj *biop = (struct bufio_obj *) iop;
    if (biop->buffer) {
	free(biop->buffer);
	biop->buffer = NULL;
    }
    return TRUE;
}

/* XXX use common dummyio_read_raw? */
static ssize_t
bufio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    printf("%s io_read_raw not overridden\n", iop->ops->io_name);
    abort();
}

MAKE_OPS(bufio, NULL);

/* virtual class: no open function */
