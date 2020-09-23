/* $Id$ */

/* WinSock v1 inet support */

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
#include "inet.h"			/* {tcp,udp}_socket */
#include "str.h"			/* bcopy */

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

static sock_t
inet_socket( host, service, port, flags, type )
    char *host, *service;
    int type;
    int flags;
    int port;
{
    struct hostent *hp;
    struct sockaddr_in sin;
    struct servent *sp;
    sock_t s;
    int true = 1;

    if (!host || !service)
	return INVALID_SOCKET;

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;

    if (service) {
	sp = getservbyname(service, (type == SOCK_STREAM ? "tcp" : "udp"));
	if (sp != NULL) {
	    sin.sin_port = sp->s_port;
	}
	else if (isdigit((unsigned char)*service)) {
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
    else if (isdigit((unsigned char)*host)) { /* possible host addr? */
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

sock_t
tcp_socket( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_socket( host, service, port, flags, SOCK_STREAM );
}

sock_t
udp_socket( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_socket( host, service, port, flags, SOCK_DGRAM );
}
