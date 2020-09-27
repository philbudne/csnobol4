/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * return a stdio stream for TERMINAL input variable
 * Win32 version.
 */

FILE *
term_input(void) {
    return fopen("CONIN$", "r");
}
