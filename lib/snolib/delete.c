/* $Id $

/*
 * SITBOL compatibility;
 * LOAD("DELETE(STRING)STRING")
 *
 * Usage;	DELETE("filename")
 * Returns;	null string or failure
 */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

int
DELETE( LA_ALIST ) LA_DCL
{
    char path[256];			/* XXX */

    getstring( LA_PTR(0), path, sizeof(path) );
    D_A(retval) = 0;			/* null ptr means null string */
    return unlink(path) == 0;
}
