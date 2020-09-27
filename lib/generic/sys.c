/* $Id$ */

/* generic support for HOST() on systems with no uname(2) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"
#include "str.h"

void
hwname(char *cp) {
    strcpy(cp, HWNAME);
}

void
osname(char *cp) {
    strcpy(cp, OSNAME);
}
