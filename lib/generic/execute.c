/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* execl() */
#endif /* HAVE_UNISTD_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

#ifndef SH_PATH
#define SH_PATH "/bin/sh"
#endif /* SH_PATH not defined */

#ifndef SH_PATH2
#define SH_PATH2 "/usr/bin/sh"
#endif /* SH_PATH2 not defined */

void
execute(char *) {
    execl(SH_PATH, "sh", "-c", buf, NULL); /* XXX SH_NAME? */

    /* must not have found shell; try alternate */
    execl(SH_PATH2, "sh", "-c", buf, NULL); /* XXX SH_NAME2? */
}
