/*
 * $Id$
 * compressed I/O
 * one could WELL argue that individual compressions should be subclasses!
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
#include "str.h"			/* bzero */
#include "io_obj.h"
#include "bufio_obj.h"

#define SUPER bufio_ops

struct compio_obj {
    struct bufio_obj bio;	/* line buffered input */
    struct io_obj *wiop;	/* wrapped I/O pointer */
    char dir;			/* 'r' or 'w' */
    char level;			/* compression level '0' to '9' (or 0) */
    char buf[512];		/* for read (malloc it?) */
    /* everything below in subclass? or use a union for private?? */
    void *private;
    ssize_t (*reader)(struct compio_obj *iop, const char *buf, size_t len);
    ssize_t (*writer)(struct compio_obj *iop, const char *buf, size_t len);
    int (*closer)(struct compio_obj *);
};

static ssize_t
ioo_read_raw(struct io_obj *iop, char *buf, size_t len) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_read_raw)
	    return (op->io_read_raw)(iop, buf, len);

    return -1;
}

static ssize_t
ioo_write(struct io_obj *iop, const char *buf, size_t len) {
    const struct io_ops *op;

    if (!iop)
	return -1;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_write)
	    return (op->io_write)(iop, buf, len);

    return -1;
}

#ifdef USE_ZLIB
#include <zlib.h>

static ssize_t
zlib_reader(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    z_stream *stream = (z_stream *)ciop->private;

    if (!stream)
	return -1;

    /* from minigzip.c gzread */
    stream->next_out = (void *)buf;
    stream->avail_out = len;
    do {
        if (ciop->bio.eof)
	    break;

	if (stream->avail_in == 0) {
	    stream->avail_in =
		ioo_read_raw(ciop->wiop, ciop->buf, sizeof(ciop->buf));
	    if (stream->avail_in < 1) {
		ciop->bio.eof = 1;
		break;
	    }
	    stream->next_in = (unsigned char *)ciop->buf;
	}

	switch (inflate(stream, Z_NO_FLUSH)) {
        case Z_DATA_ERROR:
            return -1;
	case Z_STREAM_END:
	    inflateReset(stream);
	    break;
	}
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
zlib_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    z_stream *stream = (z_stream *)ciop->private;

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
	char out[16384];
        stream->next_out = (unsigned char *)out;
        stream->avail_out = sizeof(out);
        (void)deflate(stream, buf ? Z_NO_FLUSH : Z_FINISH);
	/* XXX check return: */
	if (stream->avail_out != sizeof(out))
	    ioo_write(ciop->wiop, out, sizeof(out) - stream->avail_out);
    } while (stream->avail_out == 0);
    return len;
}

static int
zlib_closer(struct compio_obj *ciop) {
    z_stream *stream = (z_stream *)ciop->private;
    if (!stream)
	return FALSE;

    if (ciop->dir == 'r')
	inflateEnd(stream);
    else
        deflateEnd(stream);
    return TRUE;
}

static int
zlib_open(struct compio_obj *ciop) {
    z_stream *stream = (z_stream *) (ciop->private = malloc(sizeof(z_stream)));
    if (!stream)
	return FALSE;

    bzero(stream, sizeof(z_stream));
    ciop->closer = zlib_closer;

    if (ciop->dir == 'r') {
	/* +16 means accept gzip, +32 means accept gzip + zlib */
	int err = inflateInit2(stream, 15 + 16);
	if (err == Z_OK) {
	    ciop->reader = zlib_reader;
	    return TRUE;
	}
	else if (err != Z_MEM_ERROR) {
	    /* per Python 2.7.15 zlibmodule.c */
	    inflateEnd(stream);
	}
    }
    else {
	/*
	 * parameters from minigzip.c;
	 * wbits 15+16 means: 32K, "write gzip wrapper instead of zlib"
	 * (FreeBSD gzip uses -15: 32K, "suppress zlib wrapper")
	 */
	int level = 6;			/* gzip default */
	int err;
	if (ciop->level >= '0' && ciop->level <= '9')
	    level = ciop->level = '0';
	err = deflateInit2(stream, level,
			   8,		/* method: Z_DEFLATED */
			   15 + 16,	/* window bits (see above) */
			   8,		/* mem level */
			   0);		/* Z_DEFAULT_STRATEGY */
	if (err == Z_OK) {
	    ciop->writer = zlib_writer;
	    return TRUE;
	}
	else if (err != Z_MEM_ERROR) {
	    /* per Python 2.7.15 zlibmodule.c */
	    deflateEnd(stream);
	}
    }

    return FALSE;
}

#endif /* USE_ZLIB */

#ifdef USE_BZLIB
#include <bzlib.h>

static ssize_t
bzlib_reader(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    bz_stream *stream = (bz_stream *)ciop->private;

    if (!stream)
	return -1;

    stream->next_out = (void *)buf;
    stream->avail_out = len;
    do {
        if (ciop->bio.eof)
	    break;

	if (stream->avail_in == 0) {
	    stream->avail_in =
		ioo_read_raw(ciop->wiop, ciop->buf, sizeof(ciop->buf));
	    if (stream->avail_in < 1) {
		ciop->bio.eof = 1;
		break;
	    }
	    stream->next_in = ciop->buf;
	}
	if (BZ2_bzDecompress(stream) == BZ_DATA_ERROR)
            return -1;
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
bzlib_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    bz_stream *stream = (bz_stream *)ciop->private;

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
	char out[16384];
        stream->next_out = out;
        stream->avail_out = sizeof(out);
        (void)BZ2_bzCompress(stream, buf ? BZ_RUN : BZ_FINISH);
	/* XXX check return: */
	if (stream->avail_out != sizeof(out))
	    ioo_write(ciop->wiop, out, sizeof(out) - stream->avail_out);
    } while (stream->avail_out == 0);
    return len;
}

static int
bzlib_closer(struct compio_obj *ciop) {
    bz_stream *stream = (bz_stream *)ciop->private;
    if (!stream)
	return FALSE;

    if (ciop->dir == 'r')
	BZ2_bzDecompressEnd(stream);
    else
        BZ2_bzCompressEnd(stream);
    return TRUE;
}

static int
bzlib_open(struct compio_obj *ciop) {
    bz_stream *stream = (bz_stream *) (ciop->private = malloc(sizeof(bz_stream)));
    if (!stream)
	return FALSE;

    bzero(stream, sizeof(bz_stream));
    ciop->closer = bzlib_closer;

    if (ciop->dir == 'r') {
	/* small/slow for now? */
	if (BZ2_bzDecompressInit(stream, 0, 1) == BZ_OK) {
	    ciop->reader = bzlib_reader;
	    return TRUE;
	}
	/* Python 2.7.15 doesn't have any cleanup */
    }
    else {
	/*
	 * -9 (or --best) "merely selects the default behavior"
	 * -1 (or --fast) "isn't much faster"
	 */
	int level = 9;
	if (ciop->level >= '1' && ciop->level <= '9')
	    level = ciop->level - '0';
	if (BZ2_bzCompressInit(stream, level, 0, 0) == BZ_OK) {
	    ciop->writer = bzlib_writer;
	    return TRUE;
	}
	/* Python 2.7.15 doesn't have any cleanup */
    }

    free(ciop->private);
    return FALSE;
}

#endif /* USE_BZLIB */

#ifdef USE_LZMA
#include <lzma.h>

static ssize_t
lzma_reader(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    lzma_stream *stream = (lzma_stream *)ciop->private;

    if (!stream)
	return -1;

    /* from minigzip.c gzread */
    stream->next_out = (void *)buf;
    stream->avail_out = len;
    do {
        if (ciop->bio.eof)
	    break;

	if (stream->avail_in == 0) {
	    stream->avail_in =
		ioo_read_raw(ciop->wiop, ciop->buf, sizeof(ciop->buf));
	    if (stream->avail_in < 1) {
		ciop->bio.eof = 1;
		break;
	    }
	    stream->next_in = (unsigned char *)ciop->buf;
	}

	if (lzma_code(stream, LZMA_RUN) == LZMA_DATA_ERROR)
            return -1;
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
lzma_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    lzma_stream *stream = (lzma_stream *)ciop->private;

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
	char out[16384];
        stream->next_out = (void *)out;
        stream->avail_out = sizeof(out);
        if (lzma_code(stream, buf ? LZMA_RUN : LZMA_FINISH) != LZMA_OK)
	    return -1;
	/* XXX check return: */
	if (stream->avail_out != sizeof(out))
	    ioo_write(ciop->wiop, out, sizeof(out) - stream->avail_out);
    } while (stream->avail_out == 0);
    return len;
}

static int
lzma_closer(struct compio_obj *ciop) {
    lzma_stream *stream = (lzma_stream *)ciop->private;
    if (!stream)
	return FALSE;

    lzma_end(stream);			/* ?? */
    return TRUE;
}

static int
lzma_open(struct compio_obj *ciop) {
    lzma_stream *stream = (lzma_stream *) (ciop->private = malloc(sizeof(lzma_stream)));
    if (!stream)
	return FALSE;

    bzero(stream, sizeof(lzma_stream));
    ciop->closer = lzma_closer;

    if (ciop->dir == 'r') {
	/* fails decompressing tiny file compressed with 'xz -9'
	 * w/ only 50MB mem limit
	 */
	if (lzma_auto_decoder(stream, 100*1024*1024, LZMA_CONCATENATED) == LZMA_OK) {
	    ciop->reader = lzma_reader;
	    return TRUE;
	}
    }
    else {
	/* CRC64 is xz default
	 *
	 * zero:
	 * "sometimes faster than gzip -9 while compressing much better"
	 *
	 * six: "is the default, which is usually a good choice e.g. for
	 * distributing files that need to be decompressible even on
	 * systems with only 16 MiB RAM."
	 *
	 * levels 7, 8, 9: "These are useful only when compressing
	 * files bigger than 8 MiB, 16 MiB, and 32 MiB, respectively.
	 */
	int level = 6;
	if (ciop->level >= '0' && ciop->level <= '9')
	    level = ciop->level - '0';
	if (lzma_easy_encoder(stream, level, LZMA_CHECK_CRC64) == LZMA_OK) {
	    ciop->writer = lzma_writer;
	    return TRUE;
	}
    }

    free(ciop->private);
    return FALSE;
}
#endif /* USE_LZMA */

static int
compio_flush(struct io_obj *iop) {
    (void) iop;
    return TRUE;			/* ??? */
}

static ssize_t
compio_write(struct io_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;

    if (!ciop->writer)
	return -1;
    return (ciop->writer)(ciop, buf, len);
}


static ssize_t
compio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;

    if (!ciop->reader)
	return -1;
    return (ciop->reader)(ciop, buf, len);
}

static io_off_t
compio_tello(struct io_obj *iop) {
    (void) iop;
    return -1;
}

static int
compio_seeko(struct io_obj *iop, io_off_t off, int whence) {
    (void) iop;
    (void) off;
    (void) whence;
    return FALSE;
}

static int
compio_close(struct io_obj *iop) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    int ret;

    if (ciop->dir == 'w')
	(ciop->writer)(ciop, NULL, 0);

    if (!(ciop->closer)(ciop))
	return FALSE;

    if (ciop->private) {
	free(ciop->private);
	ciop->private = NULL;
    }
    ret = ciop->wiop && ioo_close(ciop->wiop);
    if (ciop->bio.buffer) {
	free(ciop->bio.buffer);
	ciop->bio.buffer = NULL;
    }

    return ret;
}

#define compio_getline NULL	/* use bufio */
#define compio_eof NULL		/* use bufio */
#define compio_clearerr NULL	/* use bufio */

MAKE_OPS(compio, &SUPER);

struct io_obj *
compio_open(struct io_obj *iop, int flags, int format, int lvl, int dir) {
    struct compio_obj *ciop;
    int (*opener)(struct compio_obj *) = NULL;

    switch (format) {			/* ala tar options */
#ifdef USE_ZLIB
    case 'z':
	/* appending is legal in .gz files? */
	opener = zlib_open;
	break;
#endif /* USE_ZLIB */
#ifdef USE_BZLIB
    case 'j':
	/* is appending is legal as in .gz files?? */
	opener = bzlib_open;
	break;
#endif /* USE_ZLIB */
#ifdef USE_LZMA
    case 'J':
	/* appending is legal. */
	opener = lzma_open;
	break;
#endif /* USE_LZMA */
    default:
	return NULL;
    }

    if (!opener)			/* paranoia */
	return NULL;

    ciop = (struct compio_obj *)
	io_alloc(sizeof(struct compio_obj), &compio_ops, flags);

    if (!ciop)
	return NULL;

    ciop->wiop = iop;
    ciop->dir = dir;
    /* '0' legal for xz:
     */
    if (lvl >= '0' && lvl <= '9')	/* works for ASCII! */
	ciop->level = lvl - '0';
    if (!(opener)(ciop)) {
	free(ciop);
	return NULL;
    }
    ciop->bio.buffer = malloc(ciop->bio.buflen = 1024); /* ugh! */

    return &ciop->bio.io;
}
