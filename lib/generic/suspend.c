/* $Id$ */

/*
 * process suspend
 * generic version
 */

#include <signal.h>

void
proc_suspend()
{
    kill(getpid(), SIGSTOP);		/* use 0 for process group? */
}
