/* $Id$ */

/* support for HOST() on Win32 pb 12/22/97 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>

void
hwname(cp)
    char *cp;
{
    char *hw;
    SYSTEM_INFO si;

    GetSystemInfo(&si);			/* no return value */

    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL:
	/*
	 * wProcessorLevel, not used on Windows 95.
	 * dwProcessorType contains mnemonic values, but is obsolete on WinNT!
	 */
	hw = "x86";
	break;

    case PROCESSOR_ARCHITECTURE_MIPS:
	hw = "mips";
	break;

    case PROCESSOR_ARCHITECTURE_ALPHA:
	hw = "alpha";
	break;

    case PROCESSOR_ARCHITECTURE_PPC:
	hw = "ppc";
	break;

    default:
	sprintf(cp, "arch#%d", si.wProcessorArchitecture);
	return;
    }
    strcpy(cp, hw);
}

void
osname(cp)
    char *cp;
{
    char *os;
    OSVERSIONINFO osv;
    int vnum;

    vnum = 1;
    osv.dwOSVersionInfoSize = sizeof(osv);
    if (!GetVersionEx(&osv)) {
	strcpy(cp, "unknown");
	return;
    }

    switch (osv.dwPlatformId) {
    case VER_PLATFORM_WIN32s:
	os = "Win32s";
	break;

    case VER_PLATFORM_WIN32_WINDOWS:
	/*
	 * If the dwPlatformId member of that structure is
	 * VER_PLATFORM_WIN32_WINDOWS, and the low word of the
	 * dwBuildNumber member is "1111," the system is running
	 * Windows 95 OSR 2. For Windows 95 and Windows 95 OSR 2, the
	 * dwMajorVersion element returns "4" and dwMinorVersion
	 * returns "0." Future versions or OSRs of Windows 95 will
	 * have higher build numbers. They may also have higher
	 * major/minor version numbers.
	 */
	os = "Win??";
	if (osv.dwMajorVersion == 4) {
	    switch (osv.dwMinorVersion) {
	    case 0:
		os = "Win95";
		vnum = 0;		/* suppress version number */
		break;
	    case 1:
		os = "Win98";
		vnum = 0;		/* suppress version number */
		break;
	    default:
		break;
	    }
	}
	break;

    case VER_PLATFORM_WIN32_NT:
	os = "WinNT";
	break;

    default:
	os = "Win???";			/* XXX win#id? */
	break;
    }

    if (vnum)
	sprintf(cp, "%s %d.%d", os, osv.dwMajorVersion, osv.dwMinorVersion);
    else
	strcpy(cp, os);

    cp += strlen(cp);

    /*
     * szCSDVersion:
     *
     * Windows NT: Contains a null-terminated string, such as "Service
     * Pack 3", that indicates the latest Service Pack installed on
     * the system. If no Service Pack has been installed, the string
     * is empty.
     *
     * Windows 95: Contains a null-terminated string that provides
     * arbitrary additional information about the operating system.
     */
    if (osv.szCSDVersion[0]) {
	char *tp;

	/* strip leading spaces, if any */
	tp = osv.szCSDVersion;
	while (*tp && *tp == ' ')
	    tp++;

	/* ignore if empty */
	if (*tp) {
	    *cp++ = ' ';
	    while ((*cp++ = *tp++))
		;
	}
    }
}
