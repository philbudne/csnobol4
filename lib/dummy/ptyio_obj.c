/*
 * $Id$
 * ptyio_open -- dummy version
 * Phil Budne
 * 2020-08-20
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "h.h"				/* __P */
#include "io_obj.h"

struct io_obj *
ptyio_open(char *path, int flags, int dir) {
    if (path[0] != '|' || path[1] != '|')
	return NOMATCH;

    return NULL;
}
