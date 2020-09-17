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

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>
#else  /* HAVE_STDLIB_H not defined */
/* have HAVE_MALLOC_H? */
extern void *malloc();
void free();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* read, write, SEEK_xxx, xxx_FILENO */
#endif /* HAVE_UNISTD_H defined */

#ifdef HAVE_UNIXIO_H			/* DECC v4 */
#include <unixio.h>			/* read(), lseek(), etc */
#endif /* HAVE_UNIXIO_H defined */

#ifdef HAVE_IO_H
#include <io.h>				/* _dup */
#endif /* HAVE_IO_H */

#include "h.h"				/* __P */
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

static ssize_t
stdio_read(struct io_obj *iop, char *buf, size_t len) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    if (len == 0)
	return 0;

    /*
     * ANSI C requires that a file positioning function intervene
     * between output and input.
     */
    if ((iop->flags & FL_UPDATE) && siop->last == LAST_OUTPUT)
	fseeko(siop->f, (off_t)0, SEEK_CUR); /* seek relative by zero */
    siop->last = LAST_INPUT;

    /*printf("%s siop@%p f@%p fd %d fl %#o\n",
	   iop->fname, siop, siop->f, fileno(siop->f), iop->flags);*/
    if (ISTTY(iop))
	tty_mode(siop->f,
		 (siop->io.flags & FL_BINARY) != 0,
		 (siop->io.flags & FL_NOECHO) != 0,
		 len);

    if (iop->flags & FL_BINARY) {
#ifndef NO_UNBUF_RW
	if (iop->flags & FL_UNBUF)
	    return read(fileno(siop->f), buf, len);
#endif /* NO_UNBUF_RW */

#ifdef TTY_READ_RAW
	if (ISTTY(iop))
	    return tty_read(siop->f, buf, len,
			   TRUE,	/* "raw" */
			   (iop->flags & FL_NOECHO) != 0, /* "noecho" */
			   FALSE,	/* "keepeol" */
			   iop->fname);
#endif /* TTY_READ_RAW defined */

	return fread(buf, 1, len, siop->f);
    } /* binary */

    /*
     * not binary.
     */

#ifdef TTY_READ_COOKED			/* used on VMS */
    if (ISTTY(iop))
	return tty_read(siop->f, buf, len,
			FALSE,	/* "raw" */
			(iop->flags & FL_NOECHO) != 0, /* "noecho" */
			(iop->flags & FL_EOL) == 0, /* "keepeol" */
			iop->fname);
#endif /* TTY_READ_COOKED defined */

    /*
     * fgets() returns at most recl-1 characters + NUL; we
     * want exactly recl characters, and don't want the NUL.
     * But the buffer (passed in from SIL) doesn't have room
     * for recl+1 characters.  Use fgets(), then fudge the
     * last character, since fgets() can be much more
     * efficient than a getc() loop (good implementations can
     * block transfer characters from stdio buffers to ours).
     *
     * Don't treat carriage return as an EOL; just discard it.
     */
    if (fgets(buf, len, siop->f) == NULL)
	return -1;

    len = strlen(buf);

    /* ASSERT(len > 0) ??? */
    if (buf[len-1] == '\n') {		/* saw EOL */
	if (iop->flags & FL_EOL) {	/* hide eol? */
	    len--;			/* yes. */

	    if (len > 0 && buf[len-1] == '\r')
		len--;
	} /* hiding EOL (default) */
	else if (len > 1 && buf[len-2] == '\r') {
	    /*
	     * If not hiding EOL, and saw CRLF
	     * replace CRLF with just NL
	     */
	    len--;
	    buf[len-1] = '\n';
	}
    }
    else {				/* no EOL seen */
	/* ASSERT(len == recl-1) ??? */

	/* read one more character, to fill in for NUL byte */
	int c = getc(siop->f);

	if (c == '\r')			/* CR? */
	    c = getc(siop->f);		/* toss it */

	if (c != EOF) {
	    /* save additional character if not EOL
	     * or if EOL should be returned
	     */
	    if (c != '\n' || !(iop->flags & FL_EOL)) {
		buf[len] = c;
		len++;
	    } /* not EOL or not hiding EOL */

	    /* no EOL, breaking up long lines, and no EOL: */
	    if ((iop->flags & FL_BREAK) && c != '\n' && c != EOF) {
		/* discard EOL characters if they follow */
		c = getc(siop->f);
		if (c == '\r')		/* CR? */
		    c = getc(siop->f);	/* toss it */
		if (c != '\n' && c != EOF) /* other than newline or EOF? */
		    ungetc(c, siop->f);	/* stash for next time */
	    }
	} /* extra char not EOF */
	    
	if (!(iop->flags & FL_BREAK)) {	/* not breaking up lines? */
	    /* if not at EOL or EOF, discard rest of "record" */
	    while (c != EOF && c != '\n')
		c = getc(siop->f);
	}
	/* don't care if line terminated by EOL or EOF? */
    } /* no EOL */
    return len;
}

static ssize_t
stdio_write(struct io_obj *iop, char *buf, size_t len) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    if (len == 0)
	return 0;

#ifndef NO_UNBUF_RW
    if (iop->flags & FL_UNBUF)
	return write(fileno(siop->f), buf, len);
#endif

    /*
     * ANSI C requires that a file positioning function intervene
     * between output and input.
     *
     * This may have been added out of an excess of caution!!
     */
    if ((iop->flags & FL_UPDATE) && siop->last == LAST_INPUT)
	fseeko(siop->f, (off_t)0, SEEK_CUR);	/* seek relative by zero */

    siop->last = LAST_OUTPUT;
    return fwrite(buf, 1, len, siop->f);
}

static int
stdio_seeko(struct io_obj *iop, off_t off, int whence) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

#ifndef NO_UNBUF_RW
    if (iop->flags & FL_UNBUF) {
	/* optimized stdio libraries might try to optimize away
	 * the an fseek[o] if they've seen no read/write ops?!
	 */
	return lseek(fileno(siop->f), off, whence) != (off_t)-1;
    }
#endif /* NO_UNBUF_RW not defined */

    siop->last = LAST_NONE;		/* reset last I/O type */
    return fseeko(siop->f, off, whence) == 0;
}

static off_t
stdio_tello(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

#ifndef NO_UNBUF_RW
    if (iop->flags & FL_UNBUF)
	return lseek(fileno(siop->f), 0, SEEK_CUR);
#endif /* NO_UNBUF_RW not defined */

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

#define stdio_read_raw NULL		/* only for bufio */

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
    if (unbuffer_all)
	setvbuf(f, (char *)NULL, _IONBF, 0);

    siop->io.fname = path;		/* borrowed from fp */
    siop->f = f;
    siop->last = LAST_NONE;
    return &siop->io;
}

struct io_obj *
stdio_open(path, flags, dir)
    char *path;
    int flags;
    int dir;				/* 'r' or 'w' */
{
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

#define pipeio_read NULL
#define pipeio_write NULL
#define pipeio_seeko NULL
#define pipeio_tello NULL
#define pipeio_flush NULL
#define pipeio_eof NULL
#define pipeio_clearerr NULL
#define pipeio_read_raw NULL		/* only for bufio */

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
