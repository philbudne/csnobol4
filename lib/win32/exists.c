/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>

int
exists(path)
    char *path;
{
    return GetFileAttributesA(path) != -1;
}
