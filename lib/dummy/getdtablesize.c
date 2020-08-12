/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* close */
#endif

#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
getdtablesize() {
    return 1024;
}
