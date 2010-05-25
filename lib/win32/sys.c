/* $Id$ */

/* support for HOST() on Win32 pb 12/22/97 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"
#include "str.h"

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

#ifdef PROCESSOR_ARCHITECTURE_IA64
    case PROCESSOR_ARCHITECTURE_IA64:
	hw = "ia64";
	break;
#endif

#ifdef PROCESSOR_ARCHITECTURE_AMD64
    case PROCESSOR_ARCHITECTURE_AMD64:
	hw = "amd64";
	break;
#endif

#ifdef PROCESSOR_ARCHITECTURE_IA32_ON_WIN64
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
	hw = "WOW64";
	break;
#endif

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
    char osname[32], *os;
#ifdef HAVE_GETVERSIONEX
    OSVERSIONINFOEX osv;
    int server;
#else
    OSVERSIONINFO osv;			/* NOT TESTED */
    const int server = 0;
#endif
    int vnum;

    vnum = 0;
    ZeroMemory(&osv, sizeof(osv));
    osv.dwOSVersionInfoSize = sizeof(osv);
#ifdef HAVE_GETVERSIONEX
    // VS10 expects LPOSVERSIONINFOA
    if (!GetVersionEx(&osv)) {
	strcpy(cp, "Win????");
	return;
    }
#ifdef HAVE_GETVERSIONEX
#ifndef VER_NT_WORKSTATION
#define VER_NT_WORKSTATION 1
#endif
    server = (osv.wProductType != VER_NT_WORKSTATION);
#else
    if (!GetVersion(&osv)) {		/* NOT TESTED */
	strcpy(cp, "Win????");
	return;
    }
#endif
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
		if (osv.szCSDVersion[1] == 'A' ||
		    osv.szCSDVersion[1] == 'B') {
		    os = "Win95 OSR2";
		    osv.szCSDVersion[0] = '\0';
		}
		break;
	    case 10:
		os = "Win98";
		if (osv.szCSDVersion[1] == 'A') {
		    os = "Win98SE";
		    osv.szCSDVersion[0] = '\0';
		}
		break;
	    case 90:
		os = "WinME";
		break;
	    default:
		vnum = 1;
		break;
	    }
	}
	break;

    case VER_PLATFORM_WIN32_NT:
	/* also osv.wProductType, wSuiteMask */
	os = "WinNT";
	switch (osv.dwMajorVersion) {
	case 5:
	    switch (osv.dwMinorVersion) {
	    case 0:
		os = "Win2K";
		break;
	    case 1:
		os = "WinXP";
		break;
	    case 2:
		/*
		 * if (GetSystemMetrics(SM_SERVERR2))
		 *	os = "WinServer2003R2";
		 * else if (osv.wProductType == VER_NT_WORKSTATION &&
		 *	si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		 *	os = "WinXPx64";
		 * else
		 */
		os = "WinServer2003";
		break;
	    default:
		vnum = 1;
		break;
	    }
	    break;

	case 6:				/* "Longhorn" family */
	    /* "GetProductInfo()" returns product type:
	     * http://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx
	     * PRODUCT_{ULTIMATE,
	     *	    HOME_{PREMUIUM,BASIC,ENTERPRISE},
	     *	    BUSINESS,STARTER,
	     *	    CLUSTER_SERVER,
	     *	    DATACENTER_{SERVER,SERVER_CORE},
	     *	    ENTERPRISE_{SERVER,SERVER_CORE,SERVER_IA64},
	     *	    SMALLBUSINESS_{SERVER,SERVER_PREMIUM},
	     *	    STANDARD_{SERVER,SERVER_CORE},
	     *	    WEB_SERVER}
	     */
	    switch (osv.dwMinorVersion) {
	    case 0:
		if (server)
		    os = "WinServer2008";
		else
		    os = "WinVista";
		break;
	    case 1:
		if (server)
		    os = "WinServer2008R2";
		else
		    os = "Win7";
		break;
	    default:
		vnum = 1;
		break;
	    }
	    break;
	}
	break;

    default:
	sprintf(osname, "Win#%d", osv.dwPlatformId);
	os = osname;
	vnum = 1;
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

	/* strip trailing spaces, if any */
	tp = osv.szCSDVersion + strlen(osv.szCSDVersion);
	while (tp > osv.szCSDVersion && tp[-1] == ' ')
	    tp--;
	*tp = '\0';

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
