/* $Id$ */

/*
 * Berkeley sockets inet interface using RFC2553 & RFC2292
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

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"
#include "lib.h"			/* own prototypes */
#include "str.h"			/* bcopy() */
#include "bindresvport.h"

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)0xffffffff)	/* want u_int32_t! */
#endif /* INADDR_NONE not defined */

static int
inet_socket( host, service, port, priv, type )
    char *host, *service;
    int type;
    int priv;
    int port;
{
    struct addrinfo hint, *res0, *res;
    int error, s;

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

	if (priv && bindresvport_sa(s, NULL) < 0 ||
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

static FILE *
inet_open( host, service, port, priv, type )
    char *host, *service;
    int port, priv, type;
{
    int s;
    FILE *f;
#ifdef FOLD_HOSTNAMES
    /* WATTCP on DOS requires hostname in upper case?! */
    char *cp = host;
    while ((*cp++ = toupper(*cp)))
	;
#endif /* FOLD_HOSTNAMES defined */

    s = inet_socket(host, service, port, priv, type );
    if (s < 0)
	return NULL;

    f = fdopen(s, "r+");
    if (f == NULL)
	close(s);
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
}
