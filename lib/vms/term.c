/* $Id$ */

#include <stdio.h>

/*
 * return a stdio stream for TERMINAL input variable
 * VMS version.
 *
 * XXX VAX C RTL has fdopen()!!!
 */

FILE *
term_input() {
    return fopen("SYS$INPUT:", "r");	/* UNTESTED: this right? */
}
