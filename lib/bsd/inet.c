/* $Id$ */

/* Berkeley sockets inet interface */

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

#ifdef OLD_UCX_INCLUDES
#include <types.h>
#include <socket.h>
#include <in.h>
#define HAVE_INCLUDES
#endif /* OLD_UCX_INCLUDES defined */

#ifndef HAVE_INCLUDES
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>		/* TCP_NODELAY */
#include <arpa/inet.h>			/* inet_addr() */
#endif /* HAVE_INCLUDES not defined */

#include <netdb.h>

#include "h.h"				/* TRUE/FALSE */
#include "snotypes.h"
#include "lib.h"			/* own prototypes */
#include "str.h"			/* bcopy() */
#include "bindresvport.h"

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)0xffffffff)	/* want u_int32_t! */
#endif /* INADDR_NONE not defined */

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
    int s;
    int true = 1;

    if (!host || !service)
	return -1;

    bzero((char *)&sin, sizeof(sin));
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

    s = socket( AF_INET, type, 0 );
    if (s < 0)
	return -1;

/* set a boolean option: TRUE iff flag set and attempt fails */
#define TRYOPT(FLAG,LAYER,OPT) \
	((flags & FLAG) && setsockopt(s,LAYER,OPT,&true,sizeof(true)) < 0)

    if ((flags & INET_PRIV) && bindresvport(s, NULL) < 0 ||
	TRYOPT(INET_BROADCAST,SOL_SOCKET,SO_BROADCAST) ||
	TRYOPT(INET_REUSEADDR,SOL_SOCKET,SO_REUSEADDR) ||
	TRYOPT(INET_DONTROUTE,SOL_SOCKET,SO_DONTROUTE) ||
	TRYOPT(INET_OOBINLINE,SOL_SOCKET,SO_OOBINLINE) ||
	TRYOPT(INET_KEEPALIVE,SOL_SOCKET,SO_KEEPALIVE) ||
	TRYOPT(INET_NODELAY,IPPROTO_TCP,TCP_NODELAY)) {
	close(s);
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
    close(s);
    return -1;
}

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

FILE *
tcp_open( host, service, port, flags )
    char *host, *service;
    int port, flags;
{
    return inet_open( host, service, port, flags, SOCK_STREAM );
}


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
