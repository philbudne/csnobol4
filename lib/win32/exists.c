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
    return GetFileAttributesA(path) != -1;
}
