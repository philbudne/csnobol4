/*
 * $Id$
 * generic v7 thru POSIX.1-2001 tty test
 * as of 2020 only Windows can't use this
 */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
fisatty(f)
    FILE *f;
{
    return isatty(fileno(f));
}
