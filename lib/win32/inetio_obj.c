/*
 * $Id$
 * Internet I/O object using WinSockets
 * Phil Budne
 * 2020-09-15
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* NULL, size_t */
#include <stdlib.h>			/* malloc/free */

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#define WS_MAJOR 2
#define WS_MINOR 0
#else
#include <winsock.h>
// first public release?
#define WS_MAJOR 1
#define WS_MINOR 1
#endif

#include "h.h"				/* __P, TRUE */
#include "io_obj.h"
#include "bufio_obj.h"
#include "inet.h"			/* {tc,ud}p_socket */

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
static int wsock_init_done;
#endif /* NO_STATIC_VARS not defined */

#ifndef SD_BOTH
#define SD_BOTH 2
#endif

#define SUPER bufio_ops

struct inetio_obj {
    struct bufio_obj bio;		/* line buffered input */

    SOCKET s;
};

static ssize_t
inetio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    ssize_t ret = recv(iiop->s, buf, len, 0);
    //printf("inetio_read_raw %zd\n", ret);
    return ret;
}

static ssize_t
inetio_write(struct io_obj *iop, char *buf, size_t len) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    return send(iiop->s, buf, len, 0);
}

static int
inetio_flush(struct io_obj *iop) {
    (void) iop;
    return TRUE;
}

static int
inetio_close(struct io_obj *iop) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;

    SUPER.io_close(iop);		/* free buffer */

    /* ensure all data has been sent? does not block?? */
    shutdown(iiop->s, SD_BOTH);

    /*
     * need to wait for an FD_CLOSE event??
     * then recv() until socket drained?
     */
    return closesocket(iiop->s) == 0;
} /* inetio_close */

#define inetio_read NULL		/* use bufio */
#define inetio_seeko NULL		/* use bufio */
#define inetio_tello NULL		/* use bufio */
#define inetio_eof NULL			/* use bufio */
#define inetio_clearerr NULL		/* use bufio */

MAKE_OPS(inetio, &SUPER);

static void
wsock_init(void) {
    if (wsock_init_done != 0)
	return;

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(WS_MAJOR,WS_MINOR);
    long ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
	wsock_init_done = -1;
	return;
    }
    /*
     * XXX examine wsaData.wVersion and wsaData.wHighVersion?
     * LOBYTE(ver) is major version, HIBYTE(ver) is minor version
     */
    wsock_init_done = 1;

#ifdef SO_SYNCHRONOUS_NONALERT
    /*
     * For WS1/WinNT; switch to blocking/non-overlapped I/O
     * see http://www.telicsolutions.com/techsupport/WinFAQ.htm
     */
    int opt = SO_SYNCHRONOUS_NONALERT;
    setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE,
	       (char *)&opt, sizeof(opt));
#endif
}

void
inet_cleanup() {
    if (wsock_init_done > 0)
	WSACleanup();
    wsock_init_done = 0;
}

struct io_obj *
inetio_open(char *path, int flags, int dir) {
    char *fn2, *host, *service;
    struct inetio_obj *iiop;
    int inet_flags;
    SOCKET s;

    if (strncmp(path, "/tcp/", 5) != 0 &&
	strncmp(path, "/udp/", 5) != 0)
	return NOMATCH;

    if (wsock_init_done == 0)
	wsock_init();
    if (wsock_init_done < 0)
	return NULL;

    fn2 = strdup(path+5);		/* make writable copy */
    if (inet_parse(fn2, &host, &service, &inet_flags) < 0) {
	free(fn2);
	return NULL;
    }

    if (path[1] == 'u')
	s = udp_socket( host, service, -1, inet_flags );
    else
	s = tcp_socket( host, service, -1, inet_flags );

    free(fn2);				/* free strdup'ed memory */

    if (s == INVALID_SOCKET)
	return NULL;

    iiop = (struct inetio_obj *) io_alloc(sizeof(*iiop), &inetio_ops, flags);
    if (!iiop) {
	closesocket(s);			/* XXX call into inet(6).c? */
	return NULL;
    }
    iiop->s = s;

    return &iiop->bio.io;
} /* inetio_open */
