/* $Id$ */

#include <stdio.h>

/*
 * return a stdio stream for TERMINAL input variable
 * VMS version.
 */

/* for tty.c; */
char *term_file = "SYS$COMMAND:";
FILE *term_fd;

FILE *
term_input() {
    term_fd = fopen(term_file, "r");
    return term_fd;
}
