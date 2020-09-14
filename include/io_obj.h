/* $Id$ */

/* superclass */
struct io_obj {
    const struct io_ops *ops;
    int flags;
    char *fname;		 /* DO NOT FREE (part of struct file) */
};

/*
 * Per-file (unit) flags.
 *
 * ** NOTE **
 *
 * multiple units may refer to same stdio stream (via "-" and
 * /dev/std{in,out,err} magic files) or the same file descriptors (via
 * /dev/fd/N magic pathname) and have different behaviors.
 */
#define FL_EOL		01		/* strip EOL on input, add on output */
#define FL_BINARY	02		/* binary: no EOL; use recl */
#define FL_UPDATE	04		/* update: read+write */
#define FL_UNBUF	010		/* unbuffered write */
#define FL_APPEND	020		/* append */
#define FL_NOECHO	040		/* tty: no echo */
#define FL_NOCLOSE	0100		/* don't fclose() */
#define FL_BREAK	0200		/* breaK long lines (EXPERIMENTAL) */
#define FL_EXCL		0400		/* eXclusive open (fail if eXists) */
#define FL_CLOEXEC	01000		/* mark for close on exec */
#define FL_TTY		02000		/* NOTE! only in io_obj.flags!! */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET not defined */

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif /* SEEK_CUR not defined */

#ifndef SEEK_END
#define SEEK_END 2
#endif /* SEEK_END not defined */

#ifdef NEED_OFF_T
typedef long off_t;
#else  /* NEED_OFF_T not defined */
#include <sys/types.h>			/* off_t */
#endif /* NEED_OFF_T not defined */

struct io_ops {
    const struct io_ops *io_super;	/* superclass */
    ssize_t (*io_read) __P((struct io_obj *, char *, size_t));
    ssize_t (*io_write) __P((struct io_obj *, char *, size_t));
    int (*io_seeko) __P((struct io_obj *, off_t, int)); /* bool */
    off_t (*io_tello) __P((struct io_obj *));
    int (*io_flush) __P((struct io_obj *)); /* bool */
    int (*io_eof) __P((struct io_obj *));   /* bool */
    void (*io_clearerr) __P((struct io_obj *));
    int (*io_close) __P((struct io_obj *)); /* bool */
    ssize_t (*io_read_raw) __P((struct io_obj *, char *, size_t)); /* 4bufio */
};

/*
 * Initializer for io_ops structs.
 *
 * This allows the order of fields to be kept in this file alone.
 * Would perhaps prefer to use C99 named/designated initializers, but
 * MS VSC is so prehistoric that it doesn't support support C99 named
 * initializers in 2020 (but they only seem to care about the subset
 * of C that is common with C++).
 *
 * The Super pointer allows subclassing (if a method pointer is NULL
 * the superclass is consulted, and so on, and so on).  To make NULL
 * entries just "#define myio_method NULL"
 *
 * It's currently a goal to keep io_obj (XXXio_obj) struct definitions
 * private to one file.  If it becomes necessary to have subclasses in
 * a different file (different versions depending on the O/S I'll
 * reconsider).
 *
 * If all method functions are declared static, compilers should be
 * able to detect if a method has been removed, and the code is no
 * longer reachable.
 */

#define MAKE_OPS(NAME, SUPER) \
const struct io_ops NAME##_ops = { \
    SUPER, \
    NAME##_read, \
    NAME##_write, \
    NAME##_seeko, \
    NAME##_tello, \
    NAME##_flush, \
    NAME##_eof, \
    NAME##_clearerr, \
    NAME##_close, \
    NAME##_read_raw \
}

struct io_obj *io_alloc __P((int size, const struct io_ops *ops, int flags));
int inet_parse __P((char *path, char **hostp, char **servicep, int *flagp));

// return value for _open routines (NULL means open attempted and failed)
struct io_obj nomatch;
#define NOMATCH &nomatch

struct io_obj *memio_open(char *buf, size_t len, int flags);

