/* $Id $

/*
 * SITBOL compatibility;
 * LOAD("RENAME(STRING,STRING)STRING")
 *
 * Usage;	RENAME("dest","src)
 * Returns;	null string or failure
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

int
RENAME( LA_ALIST ) LA_DCL
{
    char path1[256];			/* XXX */
    char path2[256];			/* XXX */

    getstring( LA_PTR(0), path1, sizeof(path1) );
    getstring( LA_PTR(1), path2, sizeof(path2) );
    D_A(retval) = 0;			/* null ptr means null string */
    return rename(path2, path1) == 0;	/* BSD systems only */
}
