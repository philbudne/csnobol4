/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifndef SH_PATH
#define SH_PATH "/bin/sh"
#endif

#ifndef SH_PATH2
#define SH_PATH2 "/usr/bin/sh"
#endif

void
execute(buf)
    char *buf;
{
    execl(SH_PATH, "sh", "-c", buf, 0); /* XXX SH_NAME? */

    /* must not have found shell; try alternate */
    execl(SH_PATH2, "sh", "-c", buf, 0); /* XXX SH_NAME2? */
}
