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

int
exists(char *path) {
    struct stat st;

    return stat(path, &st) >= 0;
}

int
isdir(char *path) {
    struct stat st;

    return stat(path, &st) >= 0 && S_ISDIR(st.st_mode);
}

int
abspath(char *path) {
#ifdef ABSPATH
    return ABSPATH(path);
#else
    return *path == '/';
#endif
}
