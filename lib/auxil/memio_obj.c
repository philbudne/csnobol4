/*
 * $Id$
 * *A* memio implementation
 * for systems without POSIX.1-2008 fmemopen (eg; Windows DLL)
 * Phil Budne
 * 9/13/2020 (not tested!)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, size_t */
#include <stdlib.h>			/* malloc */
#include <string.h>			/* memcpy */
#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* ssize_t */
#endif

#include "h.h"				/* TRUE */
#include "io_obj.h"
#include "bufio_obj.h"

#define SUPER bufio_ops

struct memio_obj {
    struct bufio_obj bio;	/* line buffered input */
    char *ptr;			/* pointer to buffer */
    size_t len;			/* length of buffer */
    io_off_t pos;		/* current position in buffer */
};

static ssize_t
memio_write(struct io_obj *iop, const char *buf, size_t len) {
    struct memio_obj *miop = (struct memio_obj *)iop;
    size_t avail = miop->len - miop->pos;
    if (len > avail - 1)	/* need to truncate? */
	len = avail - 1;	/* leave room for NUL */
    if (len > 0) {
	memcpy(miop->ptr + miop->pos, buf, len);
	miop->pos += len;
	miop->ptr[miop->pos] = '\0';
	return len;
    }
    return -1;
}

/*
 * sadly, copies data, but avoids ANOTHER implementation of
 * line-oriented I/O.
 *
 * XXX jam our pointer into iop->bio.buffer & bp on first read,
 *	and if called again, return -1??
 */
static ssize_t
memio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    int rem = miop->len - miop->pos;
    if (rem == 0)
 	return -1;
    if (len > rem)
 	len = rem;
    memcpy(buf, miop->ptr + miop->pos, len);
    miop->pos += len;

    return len;
}

static io_off_t
memio_tello(struct io_obj *iop) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    return miop->pos;
}

static int
memio_seeko(struct io_obj *iop, io_off_t off, int whence) {
    struct memio_obj *miop = (struct memio_obj *)iop;
    int ret = FALSE;

    switch (whence) {
    case SEEK_SET:
	if (off <= miop->len) {
	    miop->pos = off;
	    ret = TRUE;
	}
	break;
    case SEEK_CUR:
	if (miop->pos + off <= miop->len &&
	    miop->pos + off >= 0) {
	    miop->pos += off;
	    ret = TRUE;
	}
	break;
    case SEEK_END:
	/* fixed length buffer: cannot extend */
	if (miop->len + off <= miop->len &&
	    miop->len + off >= 0) {
	    miop->pos = miop->len + off;
	    ret = TRUE;
	}
	break;
    }
    if (ret)
	SUPER.io_seeko(iop, off, whence); /* invalidate line buffer */
    return ret;
}

static int
memio_flush(struct io_obj *iop) {
    (void) iop;
    return TRUE;
}

static int
memio_close(struct io_obj *iop) {
    struct memio_obj *miop = (struct memio_obj *)iop;

    if (miop->bio.buffer) {
	free(miop->bio.buffer);
	miop->bio.buffer = NULL;
    }

    return TRUE;
}

#define memio_getline NULL	/* use bufio */
#define memio_eof NULL		/* use bufio */
#define memio_clearerr NULL	/* use bufio */

MAKE_OPS(memio, &SUPER);

struct io_obj *
memio_open(char *buf, size_t len, int flags, int dir) {
    struct memio_obj *miop;

    (void) dir;
    if (len == 0)
	return NULL;

    miop = (struct memio_obj *)
	io_alloc(sizeof(struct memio_obj), &memio_ops, flags);

    if (!miop)
	return NULL;

    miop->bio.buffer = malloc(miop->bio.buflen = 1024); /* ugh! */
    
    if (flags & FL_APPEND)		/* unlikely!! */
	miop->pos = len;		/* for initial "tell" */
    else
	miop->pos = 0;
    miop->ptr = buf;
    miop->len = len;

    return &miop->bio.io;
}
