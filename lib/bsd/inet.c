/* $Id$ */

/* Berkeley sockets inet interface */

#include <stdio.h>
#include <ctype.h>

/* Ugly, but better than duplicating the whole file! */
#ifdef unix
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#else /* not unix */
#ifdef USE_WINSOCK_H
#include <winsock.h>
#endif /* USE_WINSOCK_H */
#ifdef VMS
#include <types.h>
#include <socket.h>
#include <netdb.h>
#include <in.h>
#endif /* VMS */
#endif /* not unix */

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"			/* needed on VAX/VMS for macros.h */
#include "macros.h"			/* bcopy */

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)0xffffffff)	/* use -1? */
#endif

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

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
#ifdef HAVE_SOCKADDR_LEN
    sin.sin_len = sizeof(sin);
#endif

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
    close(s);
    return -1;
}

static FILE *
inet_open( host, service, port, priv, type )
    char *host, *service;
    int port, priv, type;
{
    int s;
    FILE *f;

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
