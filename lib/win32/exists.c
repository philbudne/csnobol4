/* $Id$ */

#include <windows.h>

int
exists(path)
    char *path;
{
    return GetFileAttributesA(path) != -1;
}
