/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("EXIT()")
 *
 * Usage;	varies!
 * Returns;	varies!
 *
 * partial simulation of SPITBOL EXIT()
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

#ifndef SH_PATH
#define SH_PATH "/bin/sh"
#endif

#ifndef SH_PATH2
#define SH_PATH2 "/usr/bin/sh"
#endif

int
EXIT( LA_ALIST ) LA_DCL
{
    char buf[512];			/* XXX */
    char *env;
    int n;

    if (LA_TYPE(0) == S) {		/* EXIT("command") */
	getstring( LA_PTR(0), buf, sizeof(buf));
	execl(SH_PATH, "sh", "-c", buf, 0); /* XXX SH_NAME? */
	/* must not have found shell; try alternate */
	execl(SH_PATH2, "sh", "-c", buf, 0); /* XXX SH_NAME2? */
	/* ~sigh~ */
	RETFAIL;
    }

    /* save files not supported */
    RETFAIL;
}
