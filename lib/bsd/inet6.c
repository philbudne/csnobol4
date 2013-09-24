/* $Id$ */

/*
 * Berkeley sockets inet interface using RFC2553 & POSIX 1003.1g
 * extensions for IPv6 compatibility
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* atoi() */
#endif /* HAVE_STDLIB_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* close() */
#endif /* HAVE_UNISTD_H defined */

#include <stdio.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>		/* TCP_NODELAY */

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"
#include "lib.h"			/* own prototypes */
#include "str.h"			/* bcopy() */
#include "bindresvport.h"

/* NOTE!! Ignores "port" arg!! */
static int
inet_socket( host, service, port, flags, type )
    char *host, *service;
    int type;
    int flags;
    int port;
{
    struct addrinfo hint, *res0, *res;
    int error, s;
    int true = 1;

    if (!host || !service)
	return -1;

    bzero((char *)&hint, sizeof(hint));
    hint.ai_family = PF_UNSPEC;
    hint.ai_socktype = type;

    error = getaddrinfo(host, service, &hint, &res0);
    if (error)
	return -1;

    s = -1;
    for (res = res0; res; res = res->ai_next) {
	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (s < 0)
	    continue;

/* set a boolean option: TRUE iff flag set and attempt fails */
#define TRYOPT(FLAG,LAYER,OPT) \
	((flags & FLAG) && setsockopt(s,LAYER,OPT,&true,sizeof(true)) < 0)

	if (((flags & INET_PRIV) && bindresvport_sa(s, NULL) < 0) ||
	    TRYOPT(INET_BROADCAST,SOL_SOCKET,SO_BROADCAST) ||
	    TRYOPT(INET_REUSEADDR,SOL_SOCKET,SO_REUSEADDR) ||
	    TRYOPT(INET_DONTROUTE,SOL_SOCKET,SO_DONTROUTE) ||
	    TRYOPT(INET_OOBINLINE,SOL_SOCKET,SO_OOBINLINE) ||
	    TRYOPT(INET_KEEPALIVE,SOL_SOCKET,SO_KEEPALIVE) ||
	    TRYOPT(INET_NODELAY,IPPROTO_TCP,TCP_NODELAY) ||
	    connect(s, res->ai_addr, res->ai_addrlen) < 0) {
	    close(s);
	    s = -1;
	    continue;
	}
	break;				/* got one! */
    }
    freeaddrinfo(res0);
    return s;
}

/* NOTE!! Ignores "port" arg!! */
static FILE *
inet_open( host, service, port, flags, type )
    char *host, *service;
    int port, flags, type;
{
    int s;
    FILE *f;
#ifdef FOLD_HOSTNAMES
    /* WATTCP on DOS requires hostname in upper case?! */
    char *cp = host;
    while ((*cp++ = toupper(*cp)))
	;
#endif /* FOLD_HOSTNAMES defined */

    s = inet_socket(host, service, port, flags, type );
    if (s < 0)
	return NULL;

    f = fdopen(s, "r+");
    if (f == NULL)
	close(s);
    return f;
}

/* NOTE!! Ignores "port" arg!! */
FILE *
tcp_open( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_open( host, service, port, flags, SOCK_STREAM );
}

/* NOTE!! Ignores "port" arg!! */
FILE *
udp_open( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_open( host, service, port, flags, SOCK_DGRAM );
}

void
inet_cleanup() {
}

int
inet_close( f )
    FILE *f;
{
    return fclose(f) == 0;
}
