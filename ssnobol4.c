/*
 * $Id$
 * tiny main program using libsnobol4.so shared library
 */

#include <stdio.h>

#include "h.h"
#include "snobol4.h"			/* shared library API */

int
main(int argc, char *argv[]) {
    return snobol4_main(argc, argv);
}
