/* $Id$ */

/*
 * SITBOL compatibility;
 * LOAD("DELETE(STRING)STRING")
 *
 * Usage;	DELETE("filename")
 * Returns;	null string or failure
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* unlink() */
#endif /* HAVE_UNISTD_H defined */

int
DELETE( LA_ALIST ) LA_DCL
{
    char path[256];			/* XXX */

    getstring( LA_PTR(0), path, sizeof(path) );
    if (unlink(path) < 0) {
	RETFAIL;
    }
    RETNULL;
}
