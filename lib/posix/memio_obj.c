/*
 * $Id$
 * Another memio implementation
 * using POSIX.1-2008 fmemopen
 * Phil Budne
 * 10/10/2020!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, size_t */
#include <string.h>			/* memcpy */
#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* ssize_t */
#endif

#include "h.h"				/* TRUE */
#include "io_obj.h"
#include "stdio_obj.h"

struct io_obj *
memio_open(char *buf, size_t len, int flags, int dir) {
    char mode[MAXMODE];
    FILE *f;

    if (!buf || len == 0)
	return NULL;

    flags2mode(flags, mode, dir);
    f = fmemopen(buf, len, mode);	/* honors 'b'?! */
    if (!f)
	return NULL;

    return stdio_wrap("mem", f, 0, NULL, flags);
}
