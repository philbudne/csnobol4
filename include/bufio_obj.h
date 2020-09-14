
/* base class for I/O that can't be input buffered using stdio/fdopen */

/*
 * NOTE!! Any class using this as a base must define all io_ methods
 * (including io_read_raw) EXCEPT io_read.
 */

struct bufio_obj {
    struct io_obj io;

    /* buffer for "cooked" I/O */
    int buflen;				/* size of buffer */
    char *buffer;			/* start of buffer */

    int count;				/* valid characters in buffer */
    char *bp;				/* next valid character */
};

extern const struct io_ops bufio_ops;
