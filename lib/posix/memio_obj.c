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

#include "h.h"				/* TRUE */
#include "io_obj.h"
#include "stdio_obj.h"

struct io_obj *
memio_open(char *buf, size_t len, int flags) {
    char mode[MAXMODE];
    FILE *f;

    if (!buf || len == 0)
	return NULL;

    flags |= FL_UPDATE;
    flags2mode(flags, mode, 'r');
    f = fmemopen(buf, len, mode);	/* honors 'b'?! */
    if (!f)
	return NULL;

    return stdio_wrap("mem", f, 0, NULL, flags);
}
