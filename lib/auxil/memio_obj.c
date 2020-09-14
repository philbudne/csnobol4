/*
 * *A* memio implementation
 * for systems without POSIX.1-2008 fmemopen
 * (eg; Windows DLL)
 * Phil Budne
 * 9/13/2020 (not tested!)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, size_t */
#include <string.h>			/* memcpy */

#include "h.h"				/* TRUE */
#include "io_obj.h"

struct memio_obj {
    struct io_obj io;
    char *ptr;                /* pointer to buffer */
    size_t len;		      /* length of buffer */
    off_t pos;                /* current position in buffer */
};

static ssize_t
memio_write(struct io_obj *iop, char *buf, size_t len) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    if (len + miop->pos + 1 <= miop->len) {
	memcpy(miop->ptr + miop->pos, buf, len);
	miop->pos += len;
	miop->ptr[miop->pos] = '\0';
	return len;
    }
    /* XXX copy as much as possible? */
    return -1;
}

static ssize_t
memio_read(struct io_obj *iop, char *buf, size_t len) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    if (iop->flags & FL_BINARY) {
	int rem = miop->len - miop->pos;
	if (rem == 0)
	    return -1;			/* 0? */
	if (len > rem)
	    len = rem;
	memcpy(buf, miop->ptr + miop->pos, len);
	miop->pos += len;
	return len;
    }
    else {				/* line oriented */
	/* XXX could layer on bufio */
	int cc = 0;
	int nread = 0;
	int eol = FALSE;

	while (cc < len && !eol && miop->pos < miop->len) {
	    char c = miop->ptr[miop->pos++];
	    nread++;
	    if (c == '\r')
		continue;
	    if (c == '\n') {
		eol = TRUE;
		if (iop->flags & FL_EOL) /* normal EOL behavior (strip)? */
		    break;
	    }
	    *buf++ = c;
	    cc++;
	}
	if (nread == 0)
	    return -1;
	return cc;
    } /* line oriented */
}

static off_t
memio_tello(struct io_obj *iop) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    return miop->pos;
}

static int
memio_seeko(struct io_obj *iop, off_t off, int whence) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    switch (whence) {
    case SEEK_SET:
	if (off <= miop->len) {
	    miop->pos = off;
	    return TRUE;
	}
	break;
    case SEEK_CUR:
	if (miop->pos + off <= miop->len &&
	    miop->pos + off >= 0) {
	    miop->pos += off;
	    return TRUE;
	}
	break;
    case SEEK_END:
	/* fixed length buffer: cannot extend */
	if (miop->len + off <= miop->len &&
	    miop->len + off >= 0) {
	    miop->pos = miop->len + off;
	    return TRUE;
	}
	break;
    }
    return FALSE;
}

static int
memio_flush(struct io_obj *iop) {
    (void) iop;
    return TRUE;
}

static int
memio_eof(struct io_obj *iop) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    if (miop->len == 0)
	return TRUE;

    return miop->pos == miop->len;	/* ?? */
}

static void
memio_clearerr(struct io_obj *iop) {
    (void) iop;
}

static int
memio_close(struct io_obj *iop) {
    (void) iop;
    return TRUE;
}

/* NOTE! defines all methods, since is used alone */
#define memio_read_raw NULL		/* only w/ bufio */

MAKE_OPS(memio, NULL);

struct io_obj *
memio_open(char *buf, size_t len, int flags) {
    struct memio_obj *miop;

    if (len == 0)
	return NULL;

    miop = (struct memio_obj *)
	io_alloc(sizeof(struct memio_obj), &memio_ops, flags);

    if (!miop)
	return NULL;

    miop->pos = 0;
    miop->ptr = buf;
    miop->len = len;

    return &miop->io;
}

