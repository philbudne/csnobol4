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
     * info set by subclass _open call or _read_raw method
     * (specific meaning, and ownership rules may vary)
     */
    void *buffer;			/* object to be freed on close */
    size_t buflen;			/* size of buffer */

    /* for use by bufio_getc: */
    size_t count;			/* valid characters in buffer */
    char *bp;				/* next valid character */
    int eof;				/* read_raw failed */
};

extern const struct io_ops bufio_ops;
