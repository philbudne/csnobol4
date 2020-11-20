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
    size_t full = 0;
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

    BIO_flush(tiop->bio);
    SSL_CTX_free(tiop->ctx);		/* XXX */
    BIO_free_all(tiop->bio);		/* XXX */
    tiop->bio = NULL;
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
    long options;
    SSL *ssl;
    int s;

    (void) dir;
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

    /* XXX have an INET flag to allow old SSL/TLS versions? */
    options = SSL_OP_ALL |	/* "mostly harmless"? */
#ifdef SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
	SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION | /* not in 0.9.6 */
#endif
#ifdef SSL_OP_NO_COMPRESSION
	SSL_OP_NO_COMPRESSION |	/* not in 0.9.8b */
#endif
	SSL_OP_NO_SSLv2;
    SSL_CTX_set_options(tiop->ctx, options);
    /*
     * also:
     * SSL_OP_NO_SSLv3		in 0.9.7c
     * SSL_OP_NO_TLSv1		in 0.9.7c
     * SSL_OP_NO_TLSv1_1	in 1.0.2k
     * SSL_OP_NO_TLSv1_2	in 1.0.2k
     */

#if OPENSSL_VERSION_NUMBER >= 0x10100000L /* new in 1.1.0 */
    /* OpenSSL 1.1.0: August, 25 2016 */
    /* SSLv3/TLS 1.0 EOL: June 30, 2018 */
    /* TLS 1.1/TLS 1.2 EOL: March 31, 2020 */
    SSL_CTX_set_min_proto_version(tiop->ctx, TLS1_3_VERSION);
    /*
     *    SSL3_VERSION, TLS1_VERSION, TLS1_1_VERSION,
     *    TLS1_2_VERSION, TLS1_3_VERSION
     */
#endif

    /* Enable trust chain verification if requested */
    if (inet_flags & INET_VERIFY)
	SSL_CTX_set_verify(tiop->ctx, 
			   SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
			   NULL);

#if 0
    ssl = SSL_new(tiop->ctx);
    SSL_set_tlsext_host_name(ssl, host); /* for SNI? */
    SSL_set_connect_state(ssl);
    tiop->bio = BIO_new(BIO_f_ssl());
    BIO_set_ssl(tiop->bio, ssl, BIO_CLOSE);
    /* X509_check_host handles SAN certs? */
#else
    /* XXX check return: */
    tiop->bio = BIO_new_ssl(tiop->ctx, TRUE);	/* TRUE for client */
#endif

    /* XXX check return */
    BIO_push(tiop->bio, socket_bio);
#endif /* not plaintext test */

    /* XXX not if FL_BINARY set?! */
    /* XXX check return */
    tiop->bio = BIO_push(BIO_new(BIO_f_buffer()), tiop->bio);

    /* XXX check return */
    return &tiop->io;
}
