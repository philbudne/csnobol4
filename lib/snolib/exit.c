/* $Id$ */

/*
 * SPARC SPITBOL compatibility;
 * LOAD("EXIT()")
 *
 * Usage;	EXIT("command")
 * Returns;	fails, or passes execution to command string
 *
 * partial simulation of SPITBOL EXIT()
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

int
EXIT( LA_ALIST ) LA_DCL
{
    char buf[512];			/* XXX */

    if (LA_TYPE(0) == S) {		/* EXIT("command") */
	getstring( LA_PTR(0), buf, sizeof(buf));
	execute(buf);			/* should not return */
	/* ~sigh~ */
	RETFAIL;
    }

    /* save files not supported */
    RETFAIL;
}
