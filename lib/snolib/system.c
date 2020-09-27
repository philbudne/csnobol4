/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* system(), free() */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * LOAD("SYSTEM(STRING)INTEGER")
 *
 * Usage;	SYSTEM("shell command")
 * Returns;	exit status
 */

SYSTEM( LA_ALIST ) {
    char *cmd = getstring(LA_PTR(0));
    int ret = system(cmd);
    free(cmd);
    RETINT(ret);
}
