/* $Id$ */

/*
 * process suspend
 * BSD4.2 version
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <signal.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"


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
