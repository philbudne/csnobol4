/*
 * Phil 9/13/2020 (not yet tested!)
 * base class for line buffered input
 * for things that can't be wrapped using fdopen (winsock)
 * child class MUST define io_read_raw but not io_read
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, size_t */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include "h.h"				/* TRUE */
#include "io.h"
#include "bufio_obj.h"

static ssize_t
ioo_read_raw(struct bufio_obj *biop, char *buf, size_t len) {
    const struct io_ops *op;
    
    for (op = biop->io.ops; op; op = op->io_super)
	if (op->io_read_raw)
	    return (op->io_read_raw)(&biop->io, biop->buffer, biop->buflen);

    return -1;
}

static int
bufio_getc(biop)
    struct bufio_obj *biop;
{
    if (biop->count <= 0) {
	/*
	 * ASSuMEs that read_raw provider is like a socket
	 * and won't block until the entire request is filled.
	 * (if it won't will have to restrict reads to 1 byte):
	 */
	biop->count = ioo_read_raw(biop, biop->buffer, biop->buflen);
	if (biop->count <= 0)
	    return -1;			/* EOF, or something like it */
	biop->bp = biop->buffer;	/* reset buffer pointer */
    }
    biop->count--;
    return *biop->bp++ & 0xff;
}

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
	return ioo_read_raw(biop, buf, len);

    if (biop->buflen == 0) {
	biop->buffer = malloc(len);	/* assumes recl can't change */
	if (!biop->buffer)
	    return -1;
	biop->buflen = len;
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

#define bufio_write NULL
#define bufio_seeko NULL
#define bufio_tello NULL
#define bufio_flush NULL
#define bufio_eof NULL
#define bufio_clearerr NULL
#define bufio_close NULL
#define bufio_read_raw NULL		/* subclass MUST define!! */

/* XXX define helper for cleanup, or define close method,
 * and force subclasses to call superclasses
 * or make io_close a special case!!
 */
MAKE_OPS(bufio, NULL);

/* virtual class: no open function */
