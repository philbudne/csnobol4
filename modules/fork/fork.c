/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_UNISTD_H
#include <sys/types.h>			/* pid_t */
#include <sys/wait.h>			/* wait(), waitpid() */
#include <unistd.h>			/* fork() */
#endif /* HAVE_UNISTD_H defined */

#include <stdio.h>			/* sprintf */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "str.h"			/* for strlen */

/*
**=pea
**=sect NAME
**snobol4fork - POSIX fork for SNOBOL4
**=sect SYNOPSYS
**=code
**-INCLUDE 'fork.sno'
**
**        pid = B<FORK()>
**
**        status = B<WAITPID(>[I<pid>][,I<options>]B<)>
**=ecode
**=sect DESCRIPTION
**B<FORK()> creates a process: returns 0 in child, process ID in parent.
**
**B<WAITPID()> takes an optional process id to wait for, and returns a string
**with the process id, status, and additional status about the child.
**I<options> is a string of optional flags (see the B<waitpid>(2)
**man page for details):
**=item c
**WCONTINUED
**=item h
**WNOHANG
**=item t
**WTRAPPED
**=item u
**WUNTRACED
**=item w
**NOWAIT
**=cut
*/

/*
 * LOAD("FORK()INTEGER", FORK_DL)
 *
 * Usage;	FORK()
 * Returns;	0 if child, pid if parent, fails if fork() does
 */

lret_t
FORK( LA_ALIST ) LA_DCL
{
    int pid;

    pid = fork();
    if (pid == -1)
	RETFAIL;
    RETINT( pid );
}

/*
 * LOAD("WAITPID(,STRING)STRING", FORK_DL)
 */
lret_t
WAITPID( LA_ALIST) LA_DCL
{
    pid_t pid, wpid;
    int options = 0;
    int status = 0;
    char str[512], *cp;

    wpid = -1;				/* default to any process (wait) */
    if (LA_TYPE(0) == I)
	wpid = LA_INT(0);
    else if (LA_TYPE(0) == R)
	wpid = LA_REAL(0);		/* allow real in integer context! */
    else if (LA_TYPE(0) != S || LA_PTR(0))
	RETFAIL;

    getstring(LA_PTR(1), str, sizeof(str));
    for (cp = str; *cp; cp++) {
	switch (*cp) {
#ifdef WCONTINUED
	case 'c': case 'C': options |= WCONTINUED; break;
#endif
	case 'h': case 'H': options |= WNOHANG; break;
#ifdef WTRAPPED				/* FreeBSD */
	case 't': case 'T': options |= WTRAPPED; break;
#endif
	case 'u': case 'U': options |= WUNTRACED; break;
#ifdef WNOTWAIT
	case 'w': case 'W': options |= WNOWAIT; break;
#endif
	default: RETFAIL;
	}
    }

    pid = waitpid(wpid, &status, options);
    if (pid == -1)
	RETFAIL;

    if (WIFEXITED(status))
	sprintf(str, "%u exit %d", (unsigned)pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status)) {
	sprintf(str, "%u killed %d%s", (unsigned)pid, WTERMSIG(status), 
#ifdef WCOREDUMP
		(WCOREDUMP(status) ? " core" : "")
#else
		""
#endif
		);
    }
    else if (WIFSTOPPED(status))
	sprintf(str, "%u stopped %d", (unsigned)pid, WSTOPSIG(status));
#ifdef WIFCONTINUED
    else if (WIFCONTINUED(status))
	sprintf(str, "%u continued", (unsigned)pid);
#endif
    RETSTR(str);
}

/*
**=pea
**=sect SEE ALSO
**B<snobol4>(1), B<fork>(2), B<waitpid>(2)
**=sect AUTHOR
**Philip L. Budne
**=cut
*/
