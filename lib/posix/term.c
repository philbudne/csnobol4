/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else  /* HAVE_UNISTD_H not defined */
#define STDERR_FILENO 2
#endif /* HAVE_UNISTD_H not defined */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * return a stdio stream for TERMINAL input variable
 * POSIX.1 version; fdopen() on stderr stream
 */

FILE *
term_input() {
    return fdopen(STDERR_FILENO, "r");
}
