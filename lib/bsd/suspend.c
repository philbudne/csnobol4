/* $Id$ */

/*
 * process suspend
 * BSD4.2 version
 */

#include <signal.h>

void
proc_suspend()
{
    /* let default action occur */
    signal(SIGTSTP, SIG_DFL);

    /* Unblock SIGTSTP */
    sigsetmask(0);			/* new in 4.2 (test for sigmask?) */

    /*
     * Redeliver SIGTSTP to process group.  It's simpler to just
     * deliver a SIGSTOP (no need for the signal mask monkeyshines,
     * (or this system dependant function) but the message that the
     * shell prints is different.
     */
    kill(0, SIGTSTP);
}
