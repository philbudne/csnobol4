/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <process.h>
#include <stdio.h>			/* NULL */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

void
execute(char *buf) {
    execl("cmd.exe", "cmd", "/c", buf, NULL);
}
