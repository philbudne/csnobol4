/* $Id$ */

/* WinSock inet interface */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

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
    SOCKET s;

    if (!host || !service)
	return -1;

    if (!wsock_init) {
	WSADATA wsaData;
	WORD wVersionRequested;
	int opt;
	long ret;

	wVersionRequested = MAKEWORD(VMAJOR,VMINOR);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	    return -1;			/* init failed */
	/*
	 * XXX examine wsaData.wVersion and wsaData.wHighVersion?
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
	    sin.sin_port = htons((short)port);
	} /* no service; saw digit */
	else if (port >= 0 && port <= 0xffff) {
	    sin.sin_port = htons((short)port);
	}
	else
	    return -1;
    } /* have service */
    else if (port >= 0 && port <= 0xffff) {
	sin.sin_port = htons((short)port);
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
    int fd;
    FILE *f;
    SOCKET s;

    s = inet_socket(host, service, port, priv, type );
    if (s < 0)
	return NULL;

#ifdef INET_IO
    return (FILE *)s;
#else  /* INET_IO not defined */
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
#endif /* INET_IO not defined */
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

#ifdef INET_IO
int
inet_write(f, cp, len)
    FILE *f;
    char *cp;
    int len;
{
    return send((SOCKET)f, cp, len, 0);
}

int
inet_read_raw(f, cp, recl)
    FILE *f;
    char *cp;
    int recl;
{
    return recv((SOCKET)f, cp, recl, 0);
}


/*
 * awful, but typical of winsock code I've seen if you're on NT,
 * consider trying compilation without INET_IO, or use cygwin
 */
int
inet_read_cooked(f, cp, recl, keepeol)
    FILE *f;
    char *cp;
    int recl;
    int keepeol;
{
    int n = 0;
    while (n < recl) {
	char c;
	if (recv((SOCKET)f, &c, 1, 0) != 1)
	    break;
	if (c == '\n') {
	    if (keepeol)
		*cp++ = c;
	    break;
	}
	/* XXX if CR && !keepeol, continue?? */
	*cp++ = c;
    }
    /* XXX flush extra stuff past EOL */
    return n;
}

int
inet_close(f)
    FILE *f;
{
    /* shutdown & drain?? */
    closesocket((SOCKET)f);
}
#endif /* INET_IO defined */
