/* $Id$ */

/* WinSock inet interface */

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

#ifdef INET_IO
/*
 * About the INET_IO crock:
 *
 * On Windows 3.1 and Win9x, file handles and winsock handles are
 * incompatible; winsock handles cannot be read or written with
 * ordinary file I/O calls (e.g. ReadFile, WriteFile), so attempting
 * to wrap a socket in a stdio stream won't work.
 *
 * To work around this "feature", if INET_IO is defined, tcp_open()
 * and udp_open() return a "struct inet_file" pointer.  The code in
 * io.c has been crocked not to try normal I/O on things which have
 * the "NOTAFILE" flag set, instead calls inet_read_{raw,cooked},
 * inet_write and inet_close in inet.c, which merrily cast the "FILE *"
 * back to an "inet_file"
 */

struct inet_file {
    SOCKET s;

    /* buffer for "cooked" I/O */
    int buflen;				/* size of buffer */
    char *buffer;			/* start of buffer */

    int count;				/* valid characters in buffer */
    char *bp;				/* next valid character */
};
#endif /* INET_IO defined */

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

    s = socket( AF_INET, type, 0 );
    if (s == INVALID_SOCKET)
	return -1;

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
    return -1;
} /* inet_socket */

static FILE *
inet_open( host, service, port, flags, type )
    char *host, *service;
    int port, flags, type;
{
    SOCKET s;
#ifdef INET_IO
    struct inet_file *fp;
#else
    FILE *f;
    int fd;
#endif

    s = inet_socket(host, service, port, flags, type );
    if (s < 0)
	return NULL;

#ifdef INET_IO
    fp = (struct inet_file *)malloc(sizeof(struct inet_file));
    if (!fp) {
	closesocket(s);
	return NULL;
    }

    fp->s = s;
    fp->buflen = fp->count = 0;
    fp->buffer = NULL;
    return (FILE *)fp;			/* danger will robinson! */
#else  /* INET_IO not defined */
    /*
     * get fd (C runtime file handle) for Read/Write from socket (OS handle)
     * Only works for NT based systems.
     */
    fd = _open_osfhandle(s, O_RDWR|O_BINARY);
    if (fd < 0) {
	closesocket(s);
	return NULL;
    }

    f = fdopen(fd, "r+");
    if (f == NULL) {
	/* is one of these redundant? */
	close(fd);
	closesocket(s);
    }
    return f;
#endif /* INET_IO not defined */
} /* inet_open */

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

#ifdef INET_IO
int
inet_write(f, cp, len)
    FILE *f;
    char *cp;
    int len;
{
    struct inet_file *fp = (struct inet_file *)f;
    return send(fp->s, cp, len, 0);
}

int
inet_read_raw(f, cp, recl)
    FILE *f;
    char *cp;
    int recl;
{
    struct inet_file *fp = (struct inet_file *)f;
    /* reset fp->count to zero?? */
    return recv(fp->s, cp, recl, 0);
}

/* helper for "cooked" read */
static int
inet_getc(fp, recl)
    struct inet_file *fp;
    int recl;
{
    if (fp->buflen == 0) {
	fp->buffer = malloc(recl);
	if (!fp->buffer)
	    return -1;
	fp->buflen = recl;
    }
    if (fp->count <= 0) {

	/* XXX if recl > buflen, alloc new buffer? */
	fp->count = recv(fp->s, fp->buffer, fp->buflen, 0);
	if (fp->count <= 0)
	    return -1;			/* EOF, or something like it */
	    fp->bp = fp->buffer;	/* reset buffer pointer */
    }
    fp->count--;
    return *fp->bp++ & 0xff;		/* sign extension paranoia */
}

/*
 * On NT consider trying compilation without INET_IO defined,
 * or use cygwin!!
 */
int
inet_read_cooked(f, cp, recl, keepeol)
    FILE *f;
    char *cp;
    int recl;
    int keepeol;
{
    int c;
    int n = 0;				/* characters kept */
    int nread = 0;			/* characters read */
    int eol = FALSE;			/* eol seen */
    struct inet_file *fp = (struct inet_file *)f;

    while (n < recl && !eol && (c = inet_getc(fp, recl)) != -1) {
	/* guard against empty lines looking like read failures! */
	nread++;

	if (c == '\r')
	    continue;			/* always discard CR */

	if (c == '\n') {
	    eol = TRUE;
	    if (!keepeol)
		break;
	}

	*cp++ = c;
	n++;
    } /* while */

    if (nread == 0)			/* did not see ANYTHING? */
	return -1;			/* return error */

    /*
     * maintain record flavoredness; if EOL not seen, the line
     * was longer than our record length.  Discard rest of "record"
     */
    if (!eol) {				/* didn't see EOL */
	while ((c = inet_getc(fp, recl)) != EOF && c != '\n')
	    ;
    }
    return n;
}
#endif /* INET_IO defined */

void
inet_cleanup() {
    if (wsock_init)
	WSACleanup();
}

int
inet_close(f)
    FILE *f;
{
    SOCKET s;
#ifdef INET_IO
    struct inet_file *fp = (struct inet_file *)f;

    s = fp->s;				/* before free()!! */
    if (fp->buffer)
	free(fp->buffer);
    free(fp);
#else  /* INET_IO not defined */
    int fd;

    fflush(f);
    fd = fileno(f);
    s = _get_osfhandle(f);		/* recover socket handle? */

#endif /* INET_IO not defined */

    /*
     * ensure all data has been sent? does not block??
     */
    shutdown(s, SD_BOTH);
    /*
     * need to wait for an FD_CLOSE event??
     * then recv() until socket drained?
     */
#ifndef INET_IO
    close(fd);				/* needed? */
#endif /* INET_IO not defined */
    return closesocket(s) == 0;
} /* inet_close */

