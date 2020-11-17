/*
 * $Id$
 * TLS (the protocol formerly known as SSL) I/O for SNOBOL4
 * using OpenSSL (the package once known as SSLEAY) "BIO" API
 * Phil Budne
 * November 2020
 */

/*
 * TODO
 * * take inet_flags to limit (or enable) old protocols, disable cert check?
 * * test with OpenSSL forks such as LibeSSL and BoringSSL.
 * * test on Windows (with socket handles?)?
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>			/* before stdio(?) */
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* close */
#endif

#include <openssl/ssl.h>
#include <openssl/bio.h>

#include "h.h"				/* EXPORT */
#include "snotypes.h"
#include "str.h"			/* strdup */
#include "io_obj.h"			/* struct io_obj, io_ops, MAKEOPS */
#include "inet.h"			/* {tcp,udp}_socket */

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#define TLS_client_method SSLv23_client_method
#endif

/* could be "bio_obj"?! */
struct tlsio_obj {
    struct io_obj io;
    SSL_CTX *ctx;
    BIO *bio;
    char need_flush;
    char eof;
};

static int ssl_initialized;		/* GLOBAL */

static ssize_t
tlsio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;

    if (!tiop->bio)
	return -1;

    if (len == 0)
	return 0;

    if (tiop->need_flush)
	BIO_flush(tiop->bio);
    tiop->need_flush = 0;
    return BIO_read(tiop->bio, buf, len);
}

static ssize_t
tlsio_getline(struct io_obj *iop) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;
    size_t bufsize;
    ssize_t full = 0;
    char *bp;

    if (!tiop->bio)
	return -1;

    if (!iop->linebuf) {
	iop->linebufsize = 1024;
	iop->linebuf = malloc(iop->linebufsize);
    }
    bufsize = iop->linebufsize;
    bp = iop->linebuf;

    if (tiop->need_flush)
	BIO_flush(tiop->bio);
    tiop->need_flush = 0;
    for (;;) {
	int cc = BIO_gets(tiop->bio, bp + full, bufsize - full - 1);
	if (cc <= 0)
	    break;

	full += cc;
	if (full && bp[full-1] == '\n')
	    break;

	if (full >= bufsize-1) {
	    bufsize *= 2;
	    bp = realloc(bp, bufsize);
	    if (!bp)
		return -1;
	    iop->linebufsize = bufsize;
	    iop->linebuf = bp;
	}
    }
    if (full == 0)
	tiop->eof = 1;
    return full;
} 

static ssize_t
tlsio_write(struct io_obj *iop, const char *buf, size_t len) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;

    if (!tiop->bio)
	return -1;

    if (len == 0)
	return 0;

    tiop->need_flush = 1;
    return BIO_write(tiop->bio, buf, len);
}

static int
tlsio_seeko(struct io_obj *iop, io_off_t off, int whence) {
    (void) iop;
    (void) off;
    (void) whence;
    /*
     * "You can tune a filesystem, but you can't tune a fish."
     *			--4.2BSD tunefs(8) BUGS
     */
    return 0;				/* failure */
}

static io_off_t
tlsio_tello(struct io_obj *iop) {
    (void) iop;
    return -1;				/* failure */
}

static int
tlsio_flush(struct io_obj *iop) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;
    if (!tiop->bio)
	return 0;

    /* BIO_flush() returns 1 for success and 0 or -1 for failure. */
    return BIO_flush(tiop->bio) == 1;
}

static int
tlsio_eof(struct io_obj *iop) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;
    if (!tiop->bio)
	return 1;

    /*
     * BIO_eof() returns 1 if the BIO has read EOF,
     * the precise meaning of "EOF" varies according to the BIO type.
     */
    return tiop->eof;
}

static void
tlsio_clearerr(struct io_obj *iop) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;
    tiop->eof = 0;			/* ??? */
}

static int
tlsio_close(struct io_obj *iop) {
    struct tlsio_obj *tiop = (struct tlsio_obj *)iop;

    /* linebuf belongs to io_obj. freed in io.c? */

    if (tiop->io.flags & FL_NOCLOSE)
	return TRUE;

    /* XXX flush?? */
    SSL_CTX_free(tiop->ctx);		/* XXX */
    BIO_free_all(tiop->bio);		/* XXX */
    return TRUE;
}

MAKE_OPS(tlsio, NULL);

struct io_obj *
tlsio_open(const char *path,
	   int flags,
	   int dir) {			/* 'r' or 'w' */
    char *fn2, *host, *service;
    struct tlsio_obj *tiop;
    BIO *socket_bio;
    int inet_flags;
    int s;

    if (strncmp(path, "/tls/", 5) != 0)
	return NOMATCH;

    fn2 = strdup(path+5);		/* make writable copy */
    if (inet_parse(fn2, &host, &service, &inet_flags) < 0) {
	free(fn2);
	return NULL;
    }

    s = tcp_socket(host, service, -1, inet_flags);
    free(fn2);				/* free strdup'ed memory */

    if (s < 0)
	return NULL;

    tiop = (struct tlsio_obj *) io_alloc(sizeof(*tiop), &tlsio_ops, flags);
    if (!tiop) {
	close_socket(s);
	return NULL;
    }

    if (!ssl_initialized) {
	SSL_library_init();		/* never fails */
	ssl_initialized = 1;
    }

    socket_bio = BIO_new_socket(s, BIO_NOCLOSE); /* XXX NOCLOSE?? */

#ifdef TEST_PLAINTEXT			/* for testing BIO w/o SSL */
    tiop->bio = socket_bio;
#else
    /* XXX check return: */
    tiop->ctx = SSL_CTX_new(TLS_client_method());
    /*
     * SSL_CTX_set_options(tiop->ctx, ....)
     * possible flags:
     * SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
     * SSL_OP_NO_SSLv2
     * SSL_OP_NO_SSLv3
     * SSL_OP_NO_COMPRESSION
     *
     * Enable trust chain verification:
     * SSL_CTX_set_verify(tiop->ctx, SSL_VERIFY_PEER, NULL);
     */
    /* XXX check return: */
    tiop->bio = BIO_new_ssl(tiop->ctx, TRUE);	/* TRUE for client */

    /* XXX check return */
    BIO_push(tiop->bio, socket_bio);
#endif

    /* XXX not if FL_BINARY set?! */
    /* XXX check return */
    tiop->bio = BIO_push(BIO_new(BIO_f_buffer()), tiop->bio);

    /* XXX check return */
    return &tiop->io;
}
