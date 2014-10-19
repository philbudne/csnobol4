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
 * LOAD("FORK()INTEGER", FORK_DL)
 *
 * Usage;	FORK()
 * Returns;	0 if child, pid if parent, fails if fork() does
 */

int
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
int
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
	case 'c': case 'C': options |= WCONTINUED; break;;
	case 'h': case 'H': options |= WNOHANG; break;
	case 'u': case 'U': case 's': case 'S': 
	    /* synonyms: */
#ifdef WUNTRACED
	    options |= WUNTRACED;
#endif
#ifdef WSTOPPED
	    options |= WSTOPPED;
#endif
	    break;
	case 'x': case 'X': options |= WEXITED; break; /* implicit! */
	case 'w': case 'W': options |= WNOWAIT; break;
	default: RETFAIL;
	}
    }

    pid = waitpid(wpid, &status, options);
    if (pid == -1)
	RETFAIL;

    if (WIFCONTINUED(status))
	sprintf(str, "%u continued", (unsigned)pid);
    else if (WIFEXITED(status))
	sprintf(str, "%u exit %d", (unsigned)pid, WEXITSTATUS(status));
    else if (WIFSTOPPED(status))
	sprintf(str, "%u stopped %d", (unsigned)pid, WSTOPSIG(status));
    else if (WIFSIGNALED(status)) {
	sprintf(str, "%u killed %d%s", (unsigned)pid, WTERMSIG(status), 
#ifdef WCOREDUMP
		(WCOREDUMP(status) ? " core" : "")
#else
		""
#endif
		);
    }
    RETSTR(str);
}
