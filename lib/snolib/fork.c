/* $Id$ */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * LOAD("FORK()INTEGER")
 *
 * Usage;	FORK()
 * Returns;	0 if child, pid if parent, fails if fork() does
 */

FORK( LA_ALIST ) LA_DCL
{
    char cmd[256];			/* XXX */
    int pid;

    pid = fork();
    if (pid == -1)
	RETFAIL;
    RETINT( pid );
}
