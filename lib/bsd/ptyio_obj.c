/*
 * $Id$
 * forkexecpty -- using BSD forkpty
 * Phil Budne
 * 2020-08-20
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/wait.h>			/* waitpid() */
#include <stdio.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* _exit */
#endif /* HAVE_UNISTD_H defined */

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>			/* FreeBSD: forkpty */
#endif
#ifdef HAVE_UTIL_H
#include <util.h>			/* NetBSD: forkpty */
#endif
#ifdef HAVE_PTY_H
#include <pty.h>			/* linux: forkpty() */
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>			/* _PATH_BSHELL */
#endif

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
forkexecpty(cmd, pio, ppid)
    char *cmd;
    long *pio;
    long *ppid;
{
    int master;
    pid_t pid = forkpty(&master, NULL, NULL, NULL);
    if (pid == 0) {
	closefrom(3);
	execl(_PATH_BSHELL, "sh", "-c", cmd, NULL);
	_exit(1);
    }
    else if (pid > 0) {
	*pio = master;			/* fd */
	*ppid = pid;
	return 0;
    }
    else
	return -1;
}

int
waitpty(f, io, ptypid)
    FILE *f;
    long io;
    long ptypid;
{
    pid_t pid;
    int pstat;

    fclose(f);
    (void) io;

    // issue kill() if needed????
    do {
	pid = waitpid((pid_t)ptypid, &pstat, 0);
    } while (pid == -1 && errno == EINTR);

    if (pid == -1)
	return 1;			/* assume OK */
    else
	return (pstat == 0);
}
