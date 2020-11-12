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

enum format { GZIP, BZIP2, XZ };

struct compio_obj {
    struct bufio_obj bio;	/* line buffered input */
    struct io_obj *wiop;	/* wrapped I/O pointer */
    char dir;
    char done;			/* decompressor says done */
    /* everything below in subclass? */
    enum format fmt : 8;
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
	/* PLB: read larger chunks (keep buffer in ciop)?! */
	char in[1];
	int ret;

        if (ciop->bio.eof || ioo_read_raw(ciop->wiop, in, 1) < 1) {
	    ciop->bio.eof = 1;
	    break;
	}

	stream->next_in = (unsigned char *)in;
	stream->avail_in = 1;
	ret = inflate(stream, Z_NO_FLUSH);
        if (ret == Z_DATA_ERROR)
            return -1;
        if (ret == Z_STREAM_END) {
            inflateReset(stream);
	    ciop->done = 1;
	}
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
zlib_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    z_stream *stream = (z_stream *)ciop->private;
    char out[16384];

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
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
    free(ciop->private);
    ciop->private = NULL;
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
	/* +16 means accept gzip; +32 means accept gzip + zlib */
	if (inflateInit2(stream, 15 + 16) == Z_OK) {
	    ciop->reader = zlib_reader;
	    return TRUE;
	}
    }
    else {
	/* magic numbers from minigzip.c example */
	if (deflateInit2(stream, -1, 8, 15 + 16, 8, 0) == Z_OK) {
	    ciop->writer = zlib_writer;
	    return TRUE;
	}
    }

    free(ciop->private);
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

    /* from minigzip.c gzread */
    stream->next_out = (void *)buf;
    stream->avail_out = len;
    do {
	/* PLB: read larger chunks (keep buffer in ciop)?! */
	char in[1];
	int ret;

        if (ciop->bio.eof || ioo_read_raw(ciop->wiop, in, 1) < 1) {
	    ciop->bio.eof = 1;
	    break;
	}

	stream->next_in = in;
	stream->avail_in = 1;
	ret = BZ2_bzDecompress(stream);
        if (ret == BZ_DATA_ERROR)
            return -1;
        if (ret == BZ_STREAM_END) {
	    /* XXX reset?? */
	    ciop->done = 1;
	}
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
bzlib_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    bz_stream *stream = (bz_stream *)ciop->private;
    char out[16384];

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
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
    free(ciop->private);
    ciop->private = NULL;
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
    }
    else {
	if (BZ2_bzCompressInit(stream, 5, 0, 0) == BZ_OK) {
	    ciop->writer = bzlib_writer;
	    return TRUE;
	}
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
	/* PLB: read larger chunks (keep buffer in ciop)?! */
	char in[1];
	int ret;

        if (ciop->bio.eof || ioo_read_raw(ciop->wiop, in, 1) < 1) {
	    ciop->bio.eof = 1;
	    break;
	}

	stream->next_in = (void *)in;
	stream->avail_in = 1;
	ret = lzma_code(stream, LZMA_RUN); /* also LZMA_FINISH */
        if (ret == LZMA_DATA_ERROR)
            return -1;
        if (ret == LZMA_STREAM_END) {
	    /* XXX reset?? */
	    ciop->done = 1;
	}
    } while (stream->avail_out);
    return len - stream->avail_out;
}

/* called with NULL to finish */
static ssize_t
lzma_writer(struct compio_obj *iop, const char *buf, size_t len) {
    struct compio_obj *ciop = (struct compio_obj *)iop;
    lzma_stream *stream = (lzma_stream *)ciop->private;
    char out[16384];

    if (!stream)
	return -1;

    stream->next_in = (void *)buf;
    stream->avail_in = len;
    do {
        stream->next_out = (void *)out;
        stream->avail_out = sizeof(out);
	/* gcc 7.4, 7.5 complain ignoring result: */
        (void)lzma_code(stream, buf ? LZMA_RUN : LZMA_FINISH);
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
    free(ciop->private);
    ciop->private = NULL;
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
	/* CRC64 is xz default? */
	if (lzma_easy_encoder(stream, 5, LZMA_CHECK_CRC64) == LZMA_OK) {
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
    return -1;
}

static int
compio_seeko(struct io_obj *iop, io_off_t off, int whence) {
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
compio_open(struct io_obj *iop, int flags, char format, int dir) {
    struct compio_obj *ciop;
    enum format fmt;

    switch (format) {			/* ala tar options */
#ifdef USE_ZLIB
    case 'z':
	fmt = GZIP;
	/* appending is legal in .gz files? */
	break;
#endif /* USE_ZLIB */
#ifdef USE_BZLIB
    case 'j':
	fmt = BZIP2;
	/* is appending is legal as in .gz files?? */
	break;
#endif /* USE_ZLIB */
#ifdef USE_LZMA
    case 'J':
	fmt = XZ;
	/* appending is legal. */
	break;
#endif /* USE_LZMA */
    default:
	return NULL;
    }
    
    ciop = (struct compio_obj *)
	io_alloc(sizeof(struct compio_obj), &compio_ops, flags);

    if (!ciop)
	return NULL;

    ciop->wiop = iop;
    ciop->dir = dir;
    ciop->fmt = fmt;
    switch (fmt) {
#ifdef USE_ZLIB
    case GZIP:
	if (!zlib_open(ciop)) {
	    free(ciop);
	    return NULL;
	}
	break;
#endif
#ifdef USE_BZLIB
    case BZIP2:
	if (!bzlib_open(ciop)) {
	    free(ciop);
	    return NULL;
	}
	break;
#endif
#ifdef USE_LZMA
    case XZ:
	if (!lzma_open(ciop)) {
	    free(ciop);
	    return NULL;
	}
	break;
#endif
    default:
	break;				/* should not happen! */
    }

    ciop->bio.buffer = malloc(ciop->bio.buflen = 1024); /* ugh! */

    return &ciop->bio.io;
}
