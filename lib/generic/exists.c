/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>

int
exists(path)
    char *path;
{
    struct stat st;

    return stat(path, &st) >= 0;
}
