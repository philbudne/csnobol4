/* $Id$ */

/* support for HOST() on systems with POSIX.1 uname(2) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/utsname.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"
#include "str.h"

void
hwname(cp)
    char *cp;
{
#ifdef HWNAME
    strcpy(cp, HWNAME);
#else  /* HWNAME not defined */
    struct utsname u;

    if (uname(&u) < 0)
	strcpy(cp, "unknown");
    else
	strcpy(cp, u.machine);
#endif /* HWNAME not defined */
}

void
osname(cp)
    char *cp;
{
#ifdef OSNAME
    strcpy(cp, OSNAME);
#else  /* OSNAME not defined */
    struct utsname u;

    if (uname(&u) < 0)
	strcpy(cp, "unknown");
    else
	sprintf(cp, "%s %s", u.sysname, u.release);
#endif /* OSNAME not defined */
}
