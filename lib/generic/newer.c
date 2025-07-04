/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"

#include "lib.h"

/*
 * generic version for system that only has timestamps in seconds
 *
 * return:
 * +1 if p1 newer than p2
 *  0 if p1 not newer, or p1 does not exist
 * -1 if p2 does not exist
 */
int
newer(char *p1, char *p2) {
    struct stat st1, st2;

    if (stat(p2, &st2) < 0)
	return -1;			/* input file missing */

    if (stat(p1, &st1) < 0)
	return 0;			/* output file missing */

    if (st1.st_mtime <= st2.st_mtime)	/* output not newer than input */
	return 0;

    return 1;				/* output file newer */
}
