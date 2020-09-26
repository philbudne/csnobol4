/*
 * $Id$
 * base class for I/O that can't be input buffered using stdio/fdopen
 * Phil Budne
 * 9/13/2020
 */

/*
 * NOTE!! Any class using this as a base must define:
 *	io_write, io_flush, io_close, io_read_raw
 */

struct bufio_obj {
    struct io_obj io;

    /*
     * buffer for read_raw to read into (and bufio_getc to fetch out of)
     * owned/set by subclass xxxio_open function or xxxio_read_raw method
     * subclass io_close method MUST free (or not (memio))!!!
     */
    char *buffer;			/* used to reset 'bp' */
    size_t buflen;			/* size of buffer (for read_raw) */

    /* for use by bufio_getc: */
    size_t count;			/* valid characters in buffer */
    char *bp;				/* next valid character */
    int eof;				/* read_raw failed */
};

extern const struct io_ops bufio_ops;
