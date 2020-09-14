
/* base class for I/O that can't be input buffered using stdio/fdopen */

/*
 * NOTE!! Any class using this as a base must define:
 *	io_write, io_flush, io_close, io_read_raw
 */

struct bufio_obj {
    struct io_obj io;

    int buflen;				/* size of buffer */
    char *buffer;			/* start of buffer */

    int count;				/* valid characters in buffer */
    char *bp;				/* next valid character */

    int eof;				/* read_raw failed */
};

extern const struct io_ops bufio_ops;
