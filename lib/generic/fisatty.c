/*
 * $Id$
 * generic v7 thru POSIX.1-2001 tty test
 * as of 2020 only Windows can't use this
 * Phil Budne
 * 9/13/2020
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
fisatty(FILE *f) {
    return isatty(fileno(f));
}
