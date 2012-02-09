/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
exists(path)
    char *path;
{
    // XXX convert UTF-8 to UCS-2/UTF-16???
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
}

int
isdir(path)
    char *path;
{
    // XXX convert UTF-8 to UCS-2/UTF-16???
    DWORD attr = GetFileAttributesA(path);

    return (attr != INVALID_FILE_ATTRIBUTES && 
	    (attr & FILE_ATTRIBUTE_DIRECTORY));
}

int
abspath(path)
    char *path;
{
#define BS = '\\'
    return (*cp == '/' ||
	    *cp == BS ||
	    (isalpha(*cp) && cp[1] == ':' && cp[2] == BS));
}
