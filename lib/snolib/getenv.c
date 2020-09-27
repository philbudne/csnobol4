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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for free() */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

extern char *getenv();

int
GETENV( LA_ALIST ) {
    char *var, *val;

    var = mgetstring(LA_PTR(0));
    val = getenv(var);
    free(var);
    RETSTR(val);
}
