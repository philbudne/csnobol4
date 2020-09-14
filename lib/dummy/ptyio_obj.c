/*
 * $Id$
 * ptyio_open -- dummy version
 * Phil Budne
 * 2020-08-20
 */

#include <stdio.h>

#include "io.h"

struct io_obj *
ptyio_open(char *path, int flags, int dir) {
    if (path[0] != '|' || path[1] != '|')
	return NOMATCH;

    return NULL;
}
