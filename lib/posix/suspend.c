/* $Id$ */

/*
 * process suspend
 * POSIX.1 version
 */

/* not needed (obviously), but forces remake when re-configured */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>			/* NULL */
#include <signal.h>

void
proc_suspend()
{
    sigset_t set;
    
    /* let default action occur */
    signal(SIGTSTP, SIG_DFL);

    /* Unblock SIGTSTP */
    sigemptyset(&set);
    sigaddset(&set, SIGTSTP);
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    /*
     * Redeliver SIGTSTP to process group.  It's simpler to just
     * deliver a SIGSTOP (no need for the signal mask monkeyshines,
     * (or this system dependant function) but the message that the
     * shell prints is different.
     */
    kill(0, SIGTSTP);
}
