/* $Id$ */

/* DJGPP/MS-DOS support for HOST() */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <dos.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

void
hwname(cp)
    char *cp;
{
    strcpy(cp, "x86");
}

void
osname(cp)
    char *cp;
{
    sprintf(cp, "%s %d.%d", _os_flavor, _osmajor, _osminor);
#ifdef HAVE_DXE2
    strcat(cp, "+DXE2");
#endif /* HAVE_DXE2 defined */
#ifdef HAVE_WATTCP
    strcat(cp, "+WATTCP");
#endif /* HAVE_WATTCP defined */
}
