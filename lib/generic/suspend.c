/* $Id$ */

/*
 * process suspend
 * generic version
 */

/* not needed (obviously), but forces remake when re-configured */
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
    kill(getpid(), SIGSTOP);		/* use 0 for process group? */
}
