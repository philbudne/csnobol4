/* $Id$ */

/* support for HOST() on Win32 pb 12/22/97 */

#include <windows.h>			/* ??? */

#ifdef __i386__
/* use GetSystemInfo() wProcessorLevel info? */
#define HWNAME "x86"
#endif

#ifdef __PPC__
#define HWNAME "ppc"
#endif

#ifdef __ALPHA__			/* XXX just guessing */
#define HWNAME "alpha"
#endif

#ifndef HWNAME
#define HWNAME "unknown"
#endif

void
hwname(cp)
    char *cp;
{
    strcpy(cp, HWNAME);
}

void
osname(cp)
    char *cp;
{
    DWORD version;
    char *os;

    version = GetVersion();
    if (version < 0x80000000)
	os = "Win/NT";
    else if ((version & 255) >= 4 )
	os = "Win95";
    else
	os = "Win32s";

    sprintf(cp, "%s %d.%d", os, version & 255, (version >> 8) & 255)
}
