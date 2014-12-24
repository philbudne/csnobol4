/* modified from OpenSSH distribution -plb 5/22/2003 */

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
#endif /* HAVE_CONFIG_H defined */

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: bindresvport.c,v 1.13 2000/01/26 03:43:21 deraadt Exp $";
#endif /* defined(LIBC_SCCS) && !defined(lint) */

/*
 * Copyright (c) 1987 by Sun Microsystems, Inc.
 *
 * Portions Copyright(C) 1996, Jason Downs.  All rights reserved.
 */

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#define HAVE_INCLUDES
#endif /* HAVE_WINSOCK_H defined */

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
#endif /* HAVE_INCLUDES not defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H defined */

#include <errno.h>

#if !defined(IPPORT_RESERVED) && !defined(__GLIBC__)
/*
 * could be using broken musl libc!
 * they refuse to identify themselves:
 * http://wiki.musl-libc.org/wiki/FAQ#Q:_why_is_there_no_MUSL_macro_.3F
 */
#include <netdb.h>
#endif

#include "h.h"				/* __P() */
#include "str.h"			/* bzero() */
#include "bindresvport.h"

#define STARTPORT 600
#define ENDPORT (IPPORT_RESERVED - 1)
#define NPORTS	(ENDPORT - STARTPORT + 1)

#if !defined(EADDRINUSE) && defined(WSAEADDRINUSE)
#define EADDRINUSE WSAEADDRINUSE
#endif
#if !defined(EPFNOSUPPORT) && defined(WSAEPFNOSUPPORT)
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#endif

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
	unsigned short *portp;
	unsigned short port;
	SOCKLEN_T salen;
	int i;

	if (sa == NULL) {
		sa = &myaddr;
		bzero((char *)sa, sizeof(myaddr));

		if (getsockname(sd, sa, &salen) == -1)
			return -1;	/* errno is correctly set */

		af = sa->sa_family;
		bzero((char *)sa, salen);
	} else
		af = sa->sa_family;

	switch (af) {
	case AF_INET:
		salen = sizeof(struct sockaddr_in);
		portp = &((struct sockaddr_in *)sa)->sin_port;
		break;
/* Tru64 4.0 defines AF_INET6, but has nothing else! */
#ifdef HAVE_SOCKADDR_IN6
	case AF_INET6:
		salen = sizeof(struct sockaddr_in6);
		portp = &((struct sockaddr_in6 *)sa)->sin6_port;
		break;
#endif /* HAVE_SOCKADDR_IN6 defined */
	default:
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
#endif /* NEED_BINDRESVPORT defined */
