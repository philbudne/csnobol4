/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>

int
exists(path)
    char *path;
{
    struct stat st;

    return stat(path, &st) >= 0;
}
