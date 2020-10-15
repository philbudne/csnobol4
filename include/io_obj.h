/*
 * $Id$
 * I/O Object defines
 * Phil Budne
 * 9/11/2020
 */

/* base class */
struct io_obj {
    const struct io_ops *ops;
    int flags;
    char *fname;		 /* DO NOT FREE (part of struct file) */

    /* for io_getline: NOTE! no concurrency, could get by with global buffer */
    char *linebuf;
    size_t linebufsize;
};

/*
 * NOTE!! multiple units may refer to same stdio stream
 * (via "-" and /dev/std{in,out,err} magic paths)
 * or the same file descriptors (via the /dev/fd/N magic path)
 * on different stdio streams and have different behaviors.
 */
#define FL_KEEPEOL	01		/* keep EOL on input, none on output */
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
#define FL_INCLUDE	04000		/* only in struct file */

/* XXX reserve some FL_OBJ[123] bits for private per-class use? */

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

#ifndef io_off_t
#define io_off_t off_t
#endif

struct io_ops {
    const char *io_name;
    const struct io_ops *io_super;	/* superclass */
    ssize_t (*io_read_raw)(struct io_obj *, char *, size_t);
    ssize_t (*io_getline)(struct io_obj *);
    ssize_t (*io_write)(struct io_obj *, char *, size_t);
    int (*io_seeko)(struct io_obj *, io_off_t, int); /* bool */
    io_off_t (*io_tello)(struct io_obj *);
    int (*io_flush)(struct io_obj *); /* bool */
    int (*io_eof)(struct io_obj *);   /* bool */
    void (*io_clearerr)(struct io_obj *);
    int (*io_close)(struct io_obj *); /* bool */
};

/*
 * Initializer for io_ops structs.
 *
 * This allows the order of fields to be kept in this file alone.
 * Would perhaps prefer to use C99 named/designated initializers, but
 * MS VSC is so prehistoric that it doesn't support support C99
 * designated initializers in 2020, so it's COMPLETELY out of the question
 * (and 99% of the code is still K&R C compatible).
 *
 * 2020-09-20: Just learned that Visual Studio 16.8 will include
 * support for C11 and C17 (where they admit "for many years Visual
 * Studio has only supported C to the extent of it being required for
 * C++"), but still won't support C99 variable length arrays.
 *
 * The Super pointer allows subclassing (if a method pointer is NULL
 * the superclass is consulted, and so on, and so on).  To make NULL
 * entries just "#define myio_method NULL".
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

#if __STDC_VERSION__ >= 199901L
/* see if diagnostics more helpful */
#define MAKE_OPS(NAME, SUPERPTR) \
const struct io_ops NAME##_ops = { \
    .io_name = #NAME,	\
    .io_super = SUPERPTR, \
    .io_read_raw = NAME##_read_raw, \
    .io_getline = NAME##_getline, \
    .io_write = NAME##_write, \
    .io_seeko = NAME##_seeko, \
    .io_tello = NAME##_tello, \
    .io_flush = NAME##_flush, \
    .io_eof = NAME##_eof, \
    .io_clearerr = NAME##_clearerr, \
    .io_close = NAME##_close \
}
#else
#define MAKE_OPS(NAME, SUPERPTR) \
const struct io_ops NAME##_ops = { \
    #NAME, \
    SUPERPTR, \
    NAME##_read_raw, \
    NAME##_getline, \
    NAME##_write, \
    NAME##_seeko, \
    NAME##_tello, \
    NAME##_flush, \
    NAME##_eof, \
    NAME##_clearerr, \
    NAME##_close \
}
#endif

struct io_obj *io_alloc(int size, const struct io_ops *ops, int flags);

/* flagp: INET_xxx flags (in inet.h) */
int inet_parse(char *path, char **hostp, char **servicep, int *flagp);

/* return value for _open routines (NULL means open attempted and failed) */
extern struct io_obj nomatch;
#define NOMATCH &nomatch

struct io_obj *memio_open(char *buf, size_t len, int flags, int dir);
#ifdef INET_IO
struct io_obj *inetio_open(char *path, int flags, int dir);
#endif
#ifdef OSDEPIO_OBJ
struct io_obj *osdepio_open(char *path, int flags, int dir);
#endif
