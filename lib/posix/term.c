/* $Id$ */

#include <stdio.h>
#include <unistd.h>

/*
 * return a stdio stream for TERMINAL input variable
 * POSIX.1 version; fdopen() on stderr stream
 */

FILE *
term_input() {
    return fdopen(STDERR_FILENO, "r");
}
