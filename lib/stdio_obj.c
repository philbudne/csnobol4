/*
 * $Id$
 * I/O object using stdio
 * Phil Budne
 * 9/11/2020
 *
 * _COULD_ write to POSIX open/close/read/write system call API,
 * and perhaps avoid copying, but trust that stdio libraries are
 * well tuned, and that it's hard to beat them.
 * Also, on non-Un*x systems read/write won't be the native operations.
 *
 * NOTE: written using some ANSI (C89) declarations (gasp!)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>		       /* before stdio(?) */
#include <stdio.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* SEEK_xxx, xxx_FILENO */
#endif /* HAVE_UNISTD_H defined */

#include "h.h"				/* EXPORT */
#include "snotypes.h"

#include "io_obj.h"			/* struct io_obj, io_ops, MAKEOPS */
#include "inet.h"			/* {tcp,udp}_socket */
#include "lib.h"			/* tty_mode */
#include "stdio_obj.h"			/* MAXMODE, our own prototypes! */

#ifndef HAVE_FSEEKO
/*
 * fseeko/ftello arose on ILP 32-bit systems with "LARGE FILE" access,
 * defined in POSIX.1 (IEEE Std 1003.1-2001).  
 *
 * 32-bit systems could implement 64-bit offsets using ANSI (C89) f[sg]etpos()
 * (but the standard also says fpos_t is opaque).
 *
 * fall back to possible truncation
 * (NOTE: win32/config.h defines HAVE_FSEEKO/ftello/fseeko)
 */
#define ftello(FP) ftell(FP)
#define fseeko(FP,OFF,WHENCE) fseek(FP,(long)(OFF),WHENCE)
#endif /* HAVE_FSEEKO not defined */

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif /* STDIN_FILENO not defined */

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif /* STDOUT_FILENO not defined */

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif /* STDERR_FILENO not defined */

#define ISTTY(IOP) ((IOP)->flags & FL_TTY)

extern int unbuffer_all;

/****************************************************************
 * methods
 */

/* helper for stdio_read_raw and stdio_getline */
static void
stdio_read_setup(struct stdio_obj *siop, int len) {
    /*
     * ANSI C requires that a file positioning function intervene
     * between output and input.
     */
    if ((siop->io.flags & FL_UPDATE) && siop->last == LAST_OUTPUT)
	fseeko(siop->f, 0, SEEK_CUR); /* seek relative by zero */
    siop->last = LAST_INPUT;

    /*printf("%s siop@%p f@%p fd %d fl %#o\n",
	   iop->fname, siop, siop->f, fileno(siop->f), iop->flags);*/

    if (ISTTY(&siop->io))
	tty_mode(siop->f,
		 (siop->io.flags & FL_BINARY) != 0,
		 (siop->io.flags & FL_NOECHO) != 0,
		 len);
}

static ssize_t
stdio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    if (len == 0)
	return 0;

    stdio_read_setup(siop, len);

#ifdef TTY_READ_RAW			/* Windows, VMS */
    if (ISTTY(iop))
	return tty_read(siop->f, buf, len,
			TRUE,	/* "cbreak" */
			(iop->flags & FL_NOECHO) != 0, /* "noecho" */
			FALSE,	/* "keepeol" */
			iop->fname);
#endif /* TTY_READ_RAW defined */

    return fread(buf, 1, len, siop->f);
}

#ifdef SIGINT_EOF_CHECK
/*
 * Windows getc() (and thereforer getline) returns (permenant) EOF on
 * caught ^C while Unix doesn't return at ALL.  Make Windows imitate
 * Unix.  Returning transient error on Unix would require a setjmp and
 * a longjmp in sig_catch (if waiting for line from tty).
 */
static ssize_t
mygetline(char **bufp, size_t *lenp, FILE *f) {
    for (;;) {
	int_t user_interrupt_count = D_A(UINTCL); /* ^C count */
	size_t ret = getline(bufp, lenp, f);
	if (ret > 0)
	    return ret;
	/* error return, SIGINT seen, stream showing EOF */
	/* XXX check ISTTY too? */
	if (user_interrupt_count != D_A(UINTCL) && feof(f))
	    clearerr(f);
	else
	    return -1;
    }
    return ret;
}
#define getline my_getline
#endif

static ssize_t
stdio_getline(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    stdio_read_setup(siop, 0);		/* len?? */

#ifdef TTY_READ_COOKED			/* used on VMS */
    if (ISTTY(iop)) {
	/* XXX YUK!!! need tty_getline! */
	if (!iop->linebuf) {
	    iop->linebufsize = 1024;
	    iop->linebuf = malloc(iop->linebufsize);
	    /* XXX check return */
	}
	return tty_read(siop->f, iop->linebuf, iop->linebufsize,
			FALSE,	/* "raw" */
			(iop->flags & FL_NOECHO) != 0, /* "noecho" */
			(iop->flags & FL_KEEPEOL) != 0, /* "keepeol" */
			iop->fname);
    }
#endif /* TTY_READ_COOKED defined */

    return getline(&iop->linebuf, &iop->linebufsize, siop->f);
}

static ssize_t
stdio_write(struct io_obj *iop, char *buf, size_t len) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    if (len == 0)
	return 0;

    /*
     * ANSI C requires that a file positioning function intervene
     * between output and input.
     *
     * This may have been added out of an excess of caution!!
     */
    if ((iop->flags & FL_UPDATE) && siop->last == LAST_INPUT)
	fseeko(siop->f, (io_off_t)0, SEEK_CUR);	/* seek relative by zero */

    siop->last = LAST_OUTPUT;
    return fwrite(buf, 1, len, siop->f);
}

static int
stdio_seeko(struct io_obj *iop, io_off_t off, int whence) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    siop->last = LAST_NONE;		/* reset last I/O type */
    return fseeko(siop->f, off, whence) == 0;
}

static io_off_t
stdio_tello(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    return ftello(siop->f);
}

static int
stdio_flush(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    return fflush(siop->f) == 0;
}

static int
stdio_eof(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    return feof(siop->f);
}

static void
stdio_clearerr(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    clearerr(siop->f);
}

static int
stdio_close(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    if (siop->io.flags & FL_TTY)
	tty_close(siop->f);		/* advisory */

    if (siop->io.flags & FL_NOCLOSE)
	return TRUE;

    return fclose(siop->f) == 0;
}

MAKE_OPS(stdio, NULL);

/* create full mode string for fopen() */

void
flags2mode(int flags, char *mode, char dir) {
    char *mp = mode;

    if (dir == 'w' && (flags & FL_APPEND))
	*mp++ = 'a';
    else
	*mp++ = dir;
    if (flags & FL_UPDATE)
	*mp++ = '+';
#ifndef NO_FOPEN_B
    if (flags & FL_BINARY)
	*mp++ = 'b';
#endif /* NO_FOPEN_B not defined */

    if (flags & FL_EXCL)
	*mp++ = 'x';  /* C11: FreeBSD 10, OpenBSD 5.7, glibc 2.0.94 */

    if (flags & FL_CLOEXEC)
	*mp++ = 'e';	      /* FreeBSD 10, OpenBSD 5.7, glibc 2.7 */
    *mp++ = '\0';
}

/*
 * this is the one only place for creating an io object from a stdio stream
 */
struct io_obj *
stdio_wrap(char *path, FILE *f, size_t size, const struct io_ops *ops, int flags) {
    struct stdio_obj *siop;

    if (!ops)
	ops = &stdio_ops;

    if (!size)
	size = sizeof(struct stdio_obj);
    
    if (fisatty(f))
	flags |= FL_TTY;

    siop = (struct stdio_obj *) io_alloc(size, ops, flags);

    /* Windoze doesn't have line buffer, so go Full Monty */
    if (unbuffer_all || (flags & FL_UNBUF))
	setvbuf(f, (char *)NULL, _IONBF, 0);

    siop->io.fname = path;		/* borrowed from fp */
    siop->f = f;
    siop->last = LAST_NONE;

    return &siop->io;
}

struct io_obj *
stdio_open(char *path,
	   int flags,
	   int dir) {			/* 'r' or 'w' */
    FILE *f;
    char mode[MAXMODE];

    flags2mode(flags, mode, dir);

#ifdef OSDEP_OPEN
    /*
     * Allow interception of /dev/tty, /dev/null, etc on non-unix
     * systems.  Function should return TRUE if filename is being
     * intercepted, REGARDLESS of whether the actual open succeeds
     * (on successs, the function should set the FILE ** to point
     * to the open stream).
     */
    if (osdep_open(path, mode, &f)) {
	if (f == NULL)
	    return NULL;
    }
    else
#endif /* OSDEP_OPEN defined */
    if (strcmp(path,"-") == 0) {
	if (dir == 'r')
	    f = stdin;
	else
	    f = stdout;
	flags |= FL_NOCLOSE;
    }
    else if (strcmp(path,"/dev/stdin") == 0) {
	f = stdin;
	flags |= FL_NOCLOSE;
    }
    else if (strcmp(path,"/dev/stdout") == 0) {
	f = stdout;
	flags |= FL_NOCLOSE;
    }
    else if (strcmp(path,"/dev/stderr") == 0) {
	f = stderr;
	flags |= FL_NOCLOSE;
    }
    else if (strcmp(path, "/dev/tmpfile") == 0) {
	/* ANSI tmpfile() function returns anonymous file open for R/W */
	f = tmpfile();
    }
#ifndef NO_FDOPEN
    else if (strncmp(path, "/dev/fd/", 8) == 0) {
	int fd;

	if (sscanf(path+8, "%d", &fd) == 1) {
	    f = fdopen(fd, mode);
	    switch (fd) {
	    case STDIN_FILENO:
	    case STDOUT_FILENO:
	    case STDERR_FILENO:
		flags |= FL_NOCLOSE;
	    }
	}
#endif
	else
	    return NULL;
    }
#ifndef INET_IO
    else if (strncmp(path, "/tcp/", 5) == 0 ||
	     strncmp(path, "/udp/", 5) == 0) {
	char *fn2, *host, *service;
	int inet_flags;
	int s;

	fn2 = strdup(path+5);		/* make writable copy */
	if (inet_parse(fn2, &host, &service, &inet_flags) < 0) {
	    free(fn2);
	    return NULL;
	}

	if (path[1] == 'u')
	    s = udp_socket( host, service, -1, inet_flags );
	else
	    s = tcp_socket( host, service, -1, inet_flags );

	free(fn2);			/* free strdup'ed memory */

	/* XXX want stdio_wrapfd */
	f = fdopen(s, mode);
	if (!f)
	    close(s);
    }
#endif /* INET_IO not defined */
    else
	f = fopen(path, mode);		/* local file! */

    if (!f)
	return NULL;

    return stdio_wrap(path, f, 0, NULL, flags);
}

/****************************************************************
 * pipe, using popen
 */

static int
pipeio_close(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    return pclose(siop->f) == 0;
}

#define pipeio_read_raw NULL
#define pipeio_getline NULL
#define pipeio_write NULL
#define pipeio_seeko NULL
#define pipeio_tello NULL
#define pipeio_flush NULL
#define pipeio_eof NULL
#define pipeio_clearerr NULL

MAKE_OPS(pipeio, &stdio_ops);

struct io_obj *
pipeio_open(char *path, int flags, int dir) {
    FILE *p;
    char mode[MAXMODE];			/* X+bex<NUL> */

    if (path[0] != '|' || path[1] == '|')
	return NOMATCH;

    flags2mode(flags, mode, dir);
    p = popen(path+1, mode);
    if (!p)
	return NULL;

    return stdio_wrap(path, p, 0, &pipeio_ops, flags);
}
