/* $Id$ */

/*
 * File existance check for VMS
 * P. Budne Feb 10, 1998
 */

/* XXX use a native call?? */

#include <stat.h>

int
exists(path)
    char *path;
{
    struct stat st;

    return stat(path, &st) >= 0;
}
