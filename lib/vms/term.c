/* $Id$ */

#include <stdio.h>

/*
 * return a stdio stream for TERMINAL input variable
 * VMS version.
 */

FILE *
term_input() {
    return fopen("SYS$COMMAND:", "r");	/* this right? */
}
