/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"

#ifdef HAVE_UNISTD_H
#include <sys/types.h>			/* pid_t */
#include <unistd.h>			/* fork() */
#endif /* HAVE_UNISTD_H defined */

/*
 * LOAD("FORK()INTEGER")
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
