/* $Id$ */

/* DJGPP/MS-DOS support for HOST() */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <dos.h>

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
}
