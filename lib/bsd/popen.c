/* $Id$ */

/*	$NetBSD: popen.c,v 1.25 2000/01/22 22:19:11 mycroft Exp $	*/

/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software written by Ken Arnold and
 * published in UNIX Review, Vol. 6, No. 8.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <errno.h>
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif /* HAVE_PATHS_H */
#include <signal.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H defined */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H defined */
#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif /* HAVE_VFORK_H defined */
#include <stdio.h>

#include "str.h"

#ifndef _PATH_BSHELL
#define _PATH_BSHELL "/bin/sh"
#endif /* _PATH_BSHELL not defined */

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif /* STDIN_FILENO not defined */

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif /* STDOUT_FILENO not defined */

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif /* STDERR_FILENO not defined */

static struct pid {
	struct pid *next;
	FILE *fp;
	pid_t pid;
} *pidlist; 
	
FILE *
popen(command, type)
	const char *command, *type;
{
	struct pid *cur, *old;
	FILE *iop;
	int pdes[2], pid, twoway, serrno;

#ifdef __GNUC__
	/* This outrageous construct just to shut up a GCC warning. */
	(void) &cur; (void) &twoway; (void) &type;
#endif

	if (index(type, '+')) {
		twoway = 1;
		type = "r+";
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, pdes) < 0)
			return (NULL);
	} else  {
		twoway = 0;
		if ((*type != 'r' && *type != 'w') || type[1] ||
		    (pipe(pdes) < 0)) {
			errno = EINVAL;
			return (NULL);
		}
	}

	if ((cur = (struct pid *)malloc(sizeof(struct pid))) == NULL) {
		(void)close(pdes[0]);
		(void)close(pdes[1]);
		errno = ENOMEM;
		return (NULL);
	}

	switch (pid = vfork()) {
	case -1:			/* Error. */
		serrno = errno;
		free(cur);
		(void)close(pdes[0]);
		(void)close(pdes[1]);
		errno = serrno;
		return (NULL);
		/* NOTREACHED */
	case 0:				/* Child. */
		/* POSIX.2 B.3.2.2 "popen() shall ensure that any streams
		   from previous popen() calls that remain open in the 
		   parent process are closed in the new child process. */
		for (old = pidlist; old; old = old->next)
			close(fileno(old->fp)); /* don't allow a flush */

		if (*type == 'r') {
			(void)close(pdes[0]);
			if (pdes[1] != STDOUT_FILENO) {
				(void)dup2(pdes[1], STDOUT_FILENO);
				(void)close(pdes[1]);
			}
			if (twoway)
				(void)dup2(STDOUT_FILENO, STDIN_FILENO);
		} else {
			(void)close(pdes[1]);
			if (pdes[0] != STDIN_FILENO) {
				(void)dup2(pdes[0], STDIN_FILENO);
				(void)close(pdes[0]);
			}
		}

		execl(_PATH_BSHELL, "sh", "-c", command, NULL);
		_exit(127);
		/* NOTREACHED */
	}

	/* Parent; assume fdopen can't fail. */
	if (*type == 'r') {
		iop = fdopen(pdes[0], type);
		(void)close(pdes[1]);
	} else {
		iop = fdopen(pdes[1], type);
		(void)close(pdes[0]);
	}

	/* Link into list of file descriptors. */
	cur->fp = iop;
	cur->pid =  pid;
	cur->next = pidlist;
	pidlist = cur;

	return (iop);
}

/*
 * pclose --
 *	Pclose returns -1 if stream is not associated with a `popened' command,
 *	if already `pclosed', or waitpid returns an error.
 */
int
pclose(iop)
	FILE *iop;
{
	struct pid *cur, *last;
	int pstat;
	pid_t pid;

	/* Find the appropriate file pointer. */
	for (last = NULL, cur = pidlist; cur; last = cur, cur = cur->next)
		if (cur->fp == iop)
			break;
	if (cur == NULL)
		return (-1);

	(void)fclose(iop);

	do {
		pid = waitpid(cur->pid, &pstat, 0);
	} while (pid == -1 && errno == EINTR);

	/* Remove the entry from the linked list. */
	if (last == NULL)
		pidlist = cur->next;
	else
		last->next = cur->next;
	free(cur);
		
	return (pid == -1 ? -1 : pstat);
}
