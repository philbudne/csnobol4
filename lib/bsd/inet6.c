/* $Id$ */

/*
 * Berkeley sockets inet interface using RFC2553 & POSIX 1003.1g
 * extensions for IPv6 compatibility
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* close() */
#endif /* HAVE_UNISTD_H defined */

#include <stdio.h>
#include <fcntl.h>

#ifdef HAVE_WINSOCK2_H
#include <ws2tcpip.h>			/* getaddrinfo */
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>		/* TCP_NODELAY */
#endif

#include "h.h"				/* TRUE/FALSE, __P */
#include "inet.h"			/* own prototypes */
#include "str.h"			/* bzero */
#include "bindresvport.h"

/* NOTE!! Ignores "port" arg!! */
static sock_t
inet_socket(char *host, char *service, int type, int flags, int port) {
    struct addrinfo hint, *res0, *res;
    int yes = 1;
    int error;
    sock_t s;

    if (!host || !service)
	return -1;

    bzero((char *)&hint, sizeof(hint));
    hint.ai_family = PF_UNSPEC;
    hint.ai_socktype = type;

#ifdef FOLD_HOSTNAMES
    /* WATTCP on DOS requires hostname in upper case?! */
    char *cp = host;
    while ((*cp++ = toupper(*cp)))
	;
#endif /* FOLD_HOSTNAMES defined */

    error = getaddrinfo(host, service, &hint, &res0);
    if (error)
	return -1;

    s = -1;
    for (res = res0; res; res = res->ai_next) {
	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (s < 0)
	    continue;

#ifdef F_SETFD
	if (flags & INET_CLOEXEC) {
	    int ff = fcntl(s, F_GETFD, 0);
	    if (ff != -1 || fcntl(s, F_SETFD, ff|FD_CLOEXEC) < 0) {
		close_socket(s);
		return -1;
	    }
	}
#endif

/* set a boolean option: TRUE iff flag set and attempt fails */
#define TRYOPT(FLAG,LAYER,OPT) \
	((flags & FLAG) && \
	 setsockopt(s,LAYER,OPT,SETSOCKOPT_ARG_CAST &yes,sizeof(yes)) < 0)

	if (((flags & INET_PRIV) && bindresvport_sa(s, NULL) < 0) ||
	    TRYOPT(INET_BROADCAST,SOL_SOCKET,SO_BROADCAST) ||
	    TRYOPT(INET_REUSEADDR,SOL_SOCKET,SO_REUSEADDR) ||
	    TRYOPT(INET_DONTROUTE,SOL_SOCKET,SO_DONTROUTE) ||
	    TRYOPT(INET_OOBINLINE,SOL_SOCKET,SO_OOBINLINE) ||
	    TRYOPT(INET_KEEPALIVE,SOL_SOCKET,SO_KEEPALIVE) ||
	    TRYOPT(INET_NODELAY,IPPROTO_TCP,TCP_NODELAY) ||
	    connect(s, res->ai_addr, res->ai_addrlen) < 0) {
	    close_socket(s);
	    s = -1;
	    continue;
	}
	break;				/* got one! */
    }
    freeaddrinfo(res0);

    return s;
}

/* NOTE!! Ignores "port" arg!! */
sock_t
tcp_socket(char *host, char *service, int port, int flags) {
    return inet_socket( host, service, port, flags, SOCK_STREAM );
}

/* NOTE!! Ignores "port" arg!! */
sock_t
udp_socket(char *host, char *service, int port, int flags) {
    return inet_socket( host, service, port, flags, SOCK_DGRAM );
}

#ifndef INET_IO
void
inet_cleanup(void) {
}
#endif
