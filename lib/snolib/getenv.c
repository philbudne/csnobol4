/* $Id$ */

/*
 * Get an environment variable;  must be linked in (via pml.h)
 * (might work if load.c used -A flag).
 *
 * It might be better to implement this as a SNOBOL function;
 *	INPUT(.ENV,99,"|printenv " VAR)
 *	GETENV = ENV
 *	DETACH(.ENV)
 *	ENDFILE(99)
 */

/*
 * LOAD("GETENV(STRING)STRING")
 *
 * Usage;	GETENV(VAR)
 * Returns;	string
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

extern char *getenv();

int
GETENV( LA_ALIST ) LA_DCL
{
    char buf[256];			/* XXX */
    char *env;

    getstring( LA_PTR(0), buf, sizeof(buf) );
    env = getenv(buf);

    if (env)
	RETSTR(env, strlen(env));
    RETNULL;
}
