/* modified from OpenSSH distribution -pb 5/22/2003 */

/* This file has be modified from the original OpenBSD source */

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: bindresvport.c,v 1.13 2000/01/26 03:43:21 deraadt Exp $";
#endif /* LIBC_SCCS and not lint */

/*
 * Copyright (c) 1987 by Sun Microsystems, Inc.
 *
 * Portions Copyright(C) 1996, Jason Downs.  All rights reserved.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#include "str.h"

#define STARTPORT 600
#define ENDPORT (IPPORT_RESERVED - 1)
#define NPORTS	(ENDPORT - STARTPORT + 1)

/*
 * Bind a socket to a privileged IP port
 */
int
bindresvport_sa(sd, sa)
	int sd;
	struct sockaddr *sa;
{
	int error, af;
	struct sockaddr myaddr;
	struct sockaddr_in *sin;
#ifdef AF_INET6
	struct sockaddr_in6 *sin6;
#endif
	unsigned short *portp;
	unsigned short port;
	int salen;
	int i;

	if (sa == NULL) {
		bzero(&myaddr, sizeof(myaddr));
		sa = (struct sockaddr *)&myaddr;

		if (getsockname(sd, sa, &salen) == -1)
			return -1;	/* errno is correctly set */

		af = sa->sa_family;
		bzero(&myaddr, salen);
	} else
		af = sa->sa_family;

	if (af == AF_INET) {
		sin = (struct sockaddr_in *)sa;
		salen = sizeof(struct sockaddr_in);
		portp = &sin->sin_port;
	}
#ifdef AF_INET6
	else if (af == AF_INET6) {
		sin6 = (struct sockaddr_in6 *)sa;
		salen = sizeof(struct sockaddr_in6);
		portp = &sin6->sin6_port;
	}
#endif
	else {
		errno = EPFNOSUPPORT;
		return (-1);
	}
	sa->sa_family = af;

	port = ntohs(*portp);
	if (port == 0)
		port = (getpid() % NPORTS) + STARTPORT;

	/* Avoid warning */
	error = -1;

	for(i = 0; i < NPORTS; i++) {
		*portp = htons(port);
		
		error = bind(sd, sa, salen);

		/* Terminate on success */
		if (error == 0)
			break;
			
		/* Terminate on errors, except "address already in use" */
		if ((error < 0) && !((errno == EADDRINUSE) || (errno == EINVAL)))
			break;
			
		port++;
		if (port > ENDPORT)
			port = STARTPORT;
	}

	return (error);
}

#ifdef NEED_BINDRESVPORT
int
bindresvport(sd, sin)
	int sd;
	struct sockaddr_in *sin;
{
	return bindresvport_sa(sd, (struct sockaddr *)sin);
}
#endif
