/* $Id$ */

/*
 * Loadable module for SNOBOL4 in C on Unix systems
 * Phil Budne <phil@ultimate.com> c. December 1997
 *
 * Allows a SNOBOL4 application to become a multi-fork TCP server
 * with a single call!!
 */

/*
 * On SunOS4/5 (using gcc);
 *	gcc -I/usr/local/lib/snobol4 -shared -fPIC -o serv.so serv.c
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>

/* inet */
#include <netdb.h>
#include <netinet/in.h>
#include <ctype.h>

/* unix domain */
#include <sys/un.h>

static void
fireman() {				/* catch falling babies */
    int w;
    while( wait(&w) > 0 )
	;
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
 *	"inet" or "unix"
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
    char sserv[128];
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
#endif
    else
	RETFAIL;

    if (strcmp(sfam, "inet") == 0) {
	struct servent *sp;
	struct sockaddr_in sin;
	char *proto;
	int on;

	bzero(&sin, sizeof(sin));

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
	    sin.sin_port = sp->s_port;
	else if (isdigit(sserv[0])) {
	    int port;
	    port = atoi(sserv);
	    if (port < 1 || port > 0xffff)
		RETFAIL;
	    sin.sin_port = htons(port);
	}
	else
	    RETFAIL;

	s = socket(PF_INET, type, 0);
	if (s < 0)
	    RETFAIL;

	on = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	    close(s);
	    RETFAIL;
	}
    }
    else if (strcmp(sfam, "unix") == 0) {
	struct sockaddr_un s_un;

	s = socket(PF_UNIX, type, 0);
	if (s < 0)
	    RETFAIL;

	bzero(&s_un, sizeof(s_un));
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
    io_flushall();

    for (;;) {
	int pid;
	int slave;
	struct sockaddr sa;
	int len;

	len = sizeof(sa);
	bzero(&sa, len);

	slave = accept(s, &sa, &len);
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
	    close(s);
	    signal(SIGCHLD, SIG_DFL);
	    RETINT( slave );		/* return fd */
	}
	/* here in parent process; close our copy of data stream
	 * and loop to pick up next request
	 */
	close(slave);
    } /* forever */
    /* NOTREACHED */
} /* SERV_LISTEN */
