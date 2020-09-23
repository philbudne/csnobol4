/* $Id$ */

/*
 * Loadable module for SNOBOL4 in C on Unix systems
 * Phil Budne <phil@ultimate.com> c. December 1997
 *
 * Allows a SNOBOL4 application to become a multi-fork TCP server
 * with a single call!!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* atoi() */
#endif /* HAVE_STDLIB_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* close() */
#endif /* HAVE_UNISTD_H defined */

/* inet */
#include <netdb.h>
#include <netinet/in.h>
#include <ctype.h>

/* unix domain */
#include <sys/un.h>

#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "lib.h"			/* io_flushall() */
#include "str.h"

#ifndef SIGFUNC_T
#define SIGFUNC_T void
#endif /* SIGFUNC_T not defined */

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 104		/* OSX, many BSDs */
#endif /* UNIX_PATH_MAX not defined */

static SIGFUNC_T
fireman(sig)
    int sig;
{				/* catch falling babies */
    int w;
    while( wait(&w) > 0 )
	;
    signal(SIGCHLD, fireman);
}

/*
 * LOAD("SERV_LISTEN(STRING,STRING,STRING)INTEGER")
 *	bind a socket, listens on it, and loops waiting for connections.
 *	SERV_LISTEN returns a new data fd on a new connection
 *	in a freshly forked child process
 *	returns integer fd (for "/dev/fd/N" open)
 *
 * args;
 * FAMILY
 *	"inet", "inet6" or "unix"
 * TYPE
 *	"stream"
 * SERV
 *	inet: service name or port number
 *	unix: pathname
 *
 */
int
SERV_LISTEN( LA_ALIST ) LA_DCL
{
    char sfam[128];
    char stype[128];
    char sserv[UNIX_PATH_MAX];
    int type;
    int s;				/* "master" socket */

    getstring(LA_PTR(0), sfam, sizeof(sfam));
    getstring(LA_PTR(1), stype, sizeof(stype));
    getstring(LA_PTR(2), sserv, sizeof(sserv));

    if (strcmp(stype, "stream") == 0)
	type = SOCK_STREAM;
#if 0
    else if (strcmp(stype, "dgram") == 0) /* not so easy */
	type = SOCK_DGRAM;
#endif /* 0 */
    else
	RETFAIL;

    if (strcmp(sfam, "inet") == 0
#ifdef HAVE_SOCKADDR_IN6
	|| strcmp(sfam, "inet6") == 0
#endif /* HAVE_SOCKADDR_IN6 defined */
	) {
	struct servent *sp;
	struct sockaddr *sap;
	struct sockaddr_in sin;
#ifdef HAVE_SOCKADDR_IN6
	struct sockaddr_in6 sin6;
#endif /* HAVE_SOCKADDR_IN6 defined */
	SOCKLEN_T slen;
	char *proto;
	unsigned short nport;		/* port in network order */
	int on;

	switch (type) {
	case SOCK_STREAM:
	    proto = "tcp";
	    break;
	case SOCK_DGRAM:
	    proto = "udp";
	    break;
	default:
	    RETFAIL;
	}

	sp = getservbyname(sserv, proto);
	if (sp)
	    nport = sp->s_port;
	else if (isdigit((unsigned char)sserv[0])) {
	    int port;
	    port = atoi(sserv);
	    if (port < 1 || port > 0xffff)
		RETFAIL;
	    nport = htons(port);
	}
	else
	    RETFAIL;

	if (strcmp(sfam, "inet") == 0) {
	    s = socket(PF_INET, type, 0);
	    slen = sizeof(sin);
	    bzero((char *)&sin, slen);
	    sin.sin_family = AF_INET;
	    sin.sin_port = nport;
	    sap = (struct sockaddr *)&sin;
	}
#ifdef HAVE_SOCKADDR_IN6
	else if (strcmp(sfam, "inet6") == 0) {
	    s = socket(PF_INET6, type, 0);
	    slen = sizeof(sin6);
	    bzero((char *)&sin6, slen);
	    sin6.sin6_family = AF_INET6;
	    sin6.sin6_port = nport;
	    sap = (struct sockaddr *)&sin6;
	}
#endif /* HAVE_SOCKADDR_IN6 defined */
	else
	    RETFAIL;
	    
	if (s < 0)
	    RETFAIL;

	on = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	if (bind(s, sap, slen) < 0) {
	    close(s);
	    RETFAIL;
	}
    }
    else if (strcmp(sfam, "unix") == 0) {
	struct sockaddr_un s_un;

	s = socket(PF_UNIX, type, 0);
	if (s < 0)
	    RETFAIL;

	bzero((char *)&s_un, sizeof(s_un));
	strncpy(s_un.sun_path, sserv, sizeof(s_un.sun_path));
	if (bind(s, (struct sockaddr *)&s_un, sizeof(s_un)) < 0) {
	    close(s);
	    RETFAIL;
	}
    }
    else
	RETFAIL;

    if (listen(s, 5) < 0) {
	close(s);
	RETFAIL;
    }

    signal(SIGCHLD, fireman);

    /* flush I/O buffers, so we don't see anything twice! */
    io_flushall(0);

    for (;;) {
	int pid;
	int slave;
	struct sockaddr sa;
	SOCKLEN_T salen;

	salen = sizeof(sa);
	bzero((char *)&sa, salen);

	slave = accept(s, &sa, &salen);
	if (slave < 0) {
	    if (errno == EINTR)
		continue;
	    close(s);
	    RETFAIL;
	}

	/*
	 * Unix fork operator create a duplicate process
	 * with a copy of all code, data and stack spaces.
	 *
	 * Parent process loops in this routine, spawning
	 * a new child for each connection.  Each new child
	 * returns from the SERV_LISTEN() call with a file
	 * descriptor for the new connection, and a complete
	 * copy of all data from the master/parent process
	 */

	pid = fork();
	if (pid < 0) {
	    close(s);
	    close(slave);
	    RETFAIL;
	}

	if (pid == 0) {
	    /* here in child process */
	    close(s);			/* close master socket */
	    signal(SIGCHLD, SIG_DFL);
	    RETINT( slave );		/* return fd */
	}
	/*
	 * here in parent process; close our copy of data stream
	 * and loop to pick up next request
	 */
	close(slave);
    } /* forever */
    /* NOTREACHED */
} /* SERV_LISTEN */
