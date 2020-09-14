/* $Id$ */

/* WinSock/inetio_obj interface */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>			/* malloc */
#include <winsock.h>

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"			/* needed on VAX/VMS for macros.h */
#include "lib.h"
#include "str.h"			/* bcopy */
#include "io_obj.h"
#include "bufio_obj.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
static int wsock_init;
#endif /* NO_STATIC_VARS not defined */

/*
 * fcntl.h and io.h included for borland BCC32 v5.5
 * Greg White <glwhite@netconnect.com.au> 8/30/2000
 * needed for MINGW too! -phil 2/14/2002
 */
#include <fcntl.h>
#include <io.h>

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)0xffffffff)	/* want u_int32_t! */
#endif /* INADDR_NONE not defined */

#ifndef SD_BOTH
#define SD_BOTH 2
#endif

/*
 * Request API version 1.1 (first official release); shipped with Win95
 */
#define VMAJOR 1
#define VMINOR 1

struct inetio_obj {
    struct bufio_obj bio;

    SOCKET s;
    int eof;
};

static int
inet_socket( host, service, port, flags, type )
    char *host, *service;
    int type;
    int flags;
    int port;
{
    struct hostent *hp;
    struct sockaddr_in sin;
    struct servent *sp;
    SOCKET s;
    int true = 1;

    if (!host || !service)
	return INVALID_SOCKET;

    if (wsock_init == 0) {
	WSADATA wsaData;
	WORD wVersionRequested;
	int opt;
	long ret;

	wVersionRequested = MAKEWORD(VMAJOR,VMINOR);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret == 0) {
	    /*
	     * XXX examine wsaData.wVersion and wsaData.wHighVersion?
	     * LOBYTE(ver) is major version, HIBYTE(ver) is minor version
	     */
	    wsock_init = 1;

	    /*
	     * For WinNT; switch to blocking/non-overlapped I/O
	     * see http://www.telicsolutions.com/techsupport/WinFAQ.htm
	     */
	    opt = SO_SYNCHRONOUS_NONALERT;
	    setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE,
		       (char *)&opt, sizeof(opt));
	}
	else
	    wsock_init = -1;
    }
    if (wsock_init < 0)
	return INVALID_SOCKET;

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;

    if (service) {
	sp = getservbyname(service, (type == SOCK_STREAM ? "tcp" : "udp"));
	if (sp != NULL) {
	    sin.sin_port = sp->s_port;
	}
	else if (isdigit(*service)) {
	    port = atoi(service);
	    if (port < 0 || port > 0xffff)
		return INVALID_SOCKET;
	    sin.sin_port = htons((short)port);
	} /* no service; saw digit */
	else if (port >= 0 && port <= 0xffff) {
	    sin.sin_port = htons((short)port);
	}
	else
	    return INVALID_SOCKET;
    } /* have service */
    else if (port >= 0 && port <= 0xffff) {
	sin.sin_port = htons((short)port);
    }
    else
	return INVALID_SOCKET;

    /*
     * need winsock2.h WSASocketA call with
     * WSA_FLAG_NO_HANDLE_INHERIT to implement INET_CLOEXEC?
     */
    s = socket( AF_INET, type, 0 );
    if (s == INVALID_SOCKET)
	return s;

/* set a boolean option: TRUE iff flag set and attempt fails */
#define TRYOPT(FLAG,LAYER,OPT) \
	((flags & FLAG) && setsockopt(s,LAYER,OPT,(const void *)&true,sizeof(true)) < 0)

    if ((flags & INET_PRIV) && bindresvport(s) < 0 ||
	TRYOPT(INET_BROADCAST,SOL_SOCKET,SO_BROADCAST) ||
	TRYOPT(INET_REUSEADDR,SOL_SOCKET,SO_REUSEADDR) ||
	TRYOPT(INET_DONTROUTE,SOL_SOCKET,SO_DONTROUTE) ||
	TRYOPT(INET_OOBINLINE,SOL_SOCKET,SO_OOBINLINE) ||
	TRYOPT(INET_KEEPALIVE,SOL_SOCKET,SO_KEEPALIVE) ||
	TRYOPT(INET_NODELAY,IPPROTO_TCP,TCP_NODELAY)) {
	closesocket(s);
	return -1;
    }

    hp = gethostbyname( host );
    if (hp != NULL) {
	char **ap;

	/* try each addr in turn */
	for (ap = hp->h_addr_list; *ap; ap++) {
	    bcopy( *ap, &sin.sin_addr.s_addr, sizeof(sin.sin_addr.s_addr));
	    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == 0)
		return s;
	}
    } /* have hostname */
    else if (isdigit(*host)) {		/* possible host addr? */
	u_long addr;

	/* XXX use inet_aton() if available?? */
	addr = inet_addr(host);
	if (addr != 0 && addr != INADDR_NONE) {
	    sin.sin_addr.s_addr = addr;
	    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == 0)
		return s;
	} /* good inet_addr */
    } /* saw digit */
    closesocket(s);
    return INVALID_SOCKET;
} /* inet_socket */

static SOCKET
tcp_socket( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_socket( host, service, port, flags, SOCK_STREAM );
}

static SOCKET
udp_socket( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_socket( host, service, port, flags, SOCK_DGRAM );
}

void
inet_cleanup() {
    if (wsock_init > 0)
	WSACleanup();
}

/****************************************************************
 * inetio_obj methods
 */

#define inetio_read NULL		/* use bufio */

static ssize_t
inetio_write(struct io_obj *iop, char *buf, size_t len) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    return send(iiop->s, buf, len, 0);
}

static ssize_t
inet_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    int ret = recv(iiop->s, cp, recl, 0);
    if (ret <= 0)
	iiop->eof = 1;
    return ret;
}

static int
inet_close(struct io_obj *iop) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;

    /* ensure all data has been sent? does not block?? */
    shutdown(iiop->s, SD_BOTH);

    /*
     * need to wait for an FD_CLOSE event??
     * then recv() until socket drained?
     */
    return closesocket(iiop->s) == 0;
} /* inet_close */

static off_t
memio_tello(struct io_obj *iop) {
    (void) iop;
    return -1;
}

static int
memio_seeko(struct io_obj *iop, off_t off, int whence) {
    (void) iop;
    return FALSE;
}

static int
memio_flush(struct io_obj *iop) {
    (void) iop;
    return TRUE;
}

static int
inetio_eof(struct io_obj *iop) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    return iiop->eof;
}

static void
inetio_clearerr(struct io_obj *iop) {
    struct inetio_obj *iiop = (struct inetio_obj *) iop;
    iiop->eof = 0;			/* !! */
}

MAKE_OPS(inetio, &bufio_ops);

struct io_obj *
inetio_open(char *path, int flags, int dir) {
    char *fn2, *host, *service;
    struct inetio_obj *iiop;
    int inet_flags;
    SOCKET s;

    if (strncmp(path, "/tcp/, 5) != 0 &&
	strncmp(path, "/udp/, 5) != 0)
	return NOMATCH;


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
	closesocket(s);
	return NULL;
    }
    iiop->s = s;

    return &iiop->bio.io;
} /* inet_open */

