/* $Id$ */

/* Berkeley sockets inet interface */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "h.h"				/* TRUE/FALSE */

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

    if (service) {
	sp = getservbyname(service, (type == SOCK_STREAM ? "tcp" : "udp"));
	if (sp != NULL) {
	    sin.sin_port = sp->s_port;
	}
	else if (isdigit(*service)) {
	    int port;

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

    if (type == SOCK_STREAM && priv) {
	int lport;

	lport = IPPORT_RESERVED - 1;
	s = rresvport(&lport);
    }
    else
	s = socket( AF_INET, type, 0 );
    if (s < 0)
	return -1;

    hp = gethostbyname( host );
    if (hp != NULL) {
	char **ap;

	for (ap = hp->h_addr_list; *ap; ap++) {
	    bcopy( *ap, &sin.sin_addr.s_addr, sizeof(sin.sin_addr.s_addr));
	    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == 0)
		return s;
	}
    } /* have hostname */
    else if (isdigit(*host)) {
	u_long addr;

	addr = inet_addr(host);
	if (addr != 0 && addr != (u_long)-1) { /* XXX use 0xffffffff ?? */
	    sin.sin_addr.s_addr = addr;	/* htonl? */
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
