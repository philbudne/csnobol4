/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

/*
 * return a stdio stream for TERMINAL input variable
 * VMS version.
 */

FILE *
term_input() {
    return fopen("SYS$COMMAND:", "r");
}
