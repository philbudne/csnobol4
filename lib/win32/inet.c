/* $Id$ */

/* WinSock inet interface */

#include <stdio.h>
#include <ctype.h>

#include <winsock.h>

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"			/* needed on VAX/VMS for macros.h */
#include "macros.h"			/* bcopy */

#ifdef NO_STATIC_VARS
#include "vars.h"
#else  /* NO_STATIC_VARS not defined */
static int wsock_init;
#endif /* NO_STATIC_VARS not defined */

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)0xffffffff)	/* want u_int32_t! */
#endif /* INADDR_NONE not defined */

#define VMAJOR 1
#define VMINOR 1

static int
inet_socket( host, service, port, priv, type )
    char *host, *service;
    int type;
    int priv;
    int port;
{
    struct hostent *hp;
    struct sockaddr_in sin;
    struct servent *sp;
    int s;

    if (!host || !service)
	return -1;

    if (!wsock_init) {
	WSADATA wsaData;
	int opt;

	if (WSAStartup( MAKEWORD(VMAJOR,VMINOR), &wsaData ) != 0)
	    return -1;			/* init failed */

	/* XXX examine wsaData.wVersion and wsaData.wHighVersion?
	 * LOBYTE(ver) is major version, HIBYTE(ver) is minor version
	 */
 	wsock_init = TRUE;

	/*
	 * For WinNT; switch to blocking/non-overlapped I/O
	 * see http://www.telicsolutions.com/techsupport/WinFAQ.htm
	 */
	opt = SO_SYNCHRONOUS_NONALERT;
	setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE,
		   (char *)&opt, sizeof(opt));
    }

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
		return -1;
	    sin.sin_port = htons(port);
	} /* no service; saw digit */
	else if (port >= 0 && port <= 0xffff) {
	    sin.sin_port = htons(port);
	}
	else
	    return -1;
    } /* have service */
    else if (port >= 0 && port <= 0xffff) {
	sin.sin_port = htons(port);
    }
    else
	return -1;

    if (priv) {
	if (type == SOCK_STREAM) {
	    int lport;

	    lport = IPPORT_RESERVED - 1;
	    s = rresvport(&lport);
	}
	else
	    return -1;			/* UDP and "priv" set */
    }
    else
	s = socket( AF_INET, type, 0 );

    if (s < 0)
	return -1;

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
    return -1;
}

static FILE *
inet_open( host, service, port, priv, type )
    char *host, *service;
    int port, priv, type;
{
    int s, fd;
    FILE *f;

    s = inet_socket(host, service, port, priv, type );
    if (s < 0)
	return NULL;

    /*
     * get fd (C runtime file handle) for Read/Write from socket (OS handle)
     * Broken under Win95?
     */
    fd = _open_osfhandle(s, O_RDWR|O_BINARY);
    if (fd < 0) {
	closesocket(s);
	return NULL;
    }

    f = fdopen(fd, "r+");
    if (f == NULL) {
	/* XXX close fd? */
	closesocket(s);
    }
    return f;
}

FILE *
tcp_open( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return inet_open( host, service, port, priv, SOCK_STREAM );
}


FILE *
udp_open( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return inet_open( host, service, port, priv, SOCK_DGRAM );
}

void
inet_cleanup() {
    if (wsock_init)
	WSACleanup();
}
