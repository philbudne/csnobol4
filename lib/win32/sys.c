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
hwname(char *cp) {
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

#ifdef PROCESSOR_ARCHITECTURE_ARM
    case PROCESSOR_ARCHITECTURE_ARM:
	hw = "arm";
	break;
#endif

    default:
	sprintf(cp, "arch#%d", si.wProcessorArchitecture);
	return;
    }
    strcpy(cp, hw);
}

void
osname(char *cp) {
    char osname[32], *os;
    OSVERSIONINFOEXA osv;
    int server = 0;
    int build = 0;
    int vnum = 0;

    ZeroMemory(&osv, sizeof(osv));
    osv.dwOSVersionInfoSize = sizeof(osv);
    /*
     * https://docs.microsoft.com/en-us/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
     * 5/31/2018:
     * In Windows 8.1 and Windows 10, the GetVersion and GetVersionEx
     * functions have been deprecated. In Windows 10, the
     * VerifyVersionInfo function has also been deprecated. While you
     * can still call the deprecated functions, if your application
     * does not specifically target Windows 8.1 or Windows 10, the
     * functions will return the Windows 8 version (6.2).
     */
    if (!GetVersionExA((LPOSVERSIONINFO)&osv)) {
	strcpy(cp, "Win????");
	return;
    }
#ifdef DEBUG_SYS
    printf("plat %ld %ld.%ld.%ld ptype %d\n",
	   osv.dwPlatformId,
	   osv.dwMajorVersion,
	   osv.dwMinorVersion,
	   osv.dwBuildNumber,
	   osv.wProductType);
#define HERE printf("line %d\n", __LINE__)
#else
#define HERE (void)0
#endif
    build = osv.dwBuildNumber;
    server = (osv.wProductType != VER_NT_WORKSTATION);
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
	build &= 0xffff;
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
		HERE;
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
		os = "WinServer2003";
		break;
	    default:
		vnum = 1;
		HERE;
		break;
	    } // 5
	    break;

	case 6:				/* "Longhorn" family */
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
	    case 2:
		if (server)
		    os = "WinServer2012";
		else
		    os = "Win8";
		break;
	    case 3:	/* not returned unless manifested (see above) */
		if (server)
		    os = "WinServer2012R2";
		else
		    os = "Win8.1";
		break;
	    case 4:	/* prior to build 9926 (only if manifested) */
		if (server)
		    os = "WinServer2016";
		else
		    os = "Win10";
		break;
	    default:
		vnum = 1;
		HERE;
		break;
	    } // 6
	    break;

	case 10: /* not returned unless binary manifested for Win10 (else 6.2) */
	    switch (osv.dwMinorVersion) {
	    case 0:
		if (server)
		    os = "WinServer2016"; /* 2019 if build >=1809? */
		else
		    os = "Win10";
		break;
	    /*
	     * anything added here will not be seen unless snobol4.exe.manifest
	     * updated with guid of new OS
	     */
	    default:
		vnum = 1;
		HERE;
		break;
	    } // 10
	    break;

	default:
	    vnum = 1;
	    HERE;
	    break;
	} // dwMajorVersion
	break;

    default:
	sprintf(osname, "Win#%d", (int)osv.dwPlatformId);
	os = osname;
	vnum = 1;
	HERE;
	break;
    } // dwPlatformId

    if (vnum) {
	sprintf(cp, "%s %d.%d", os, (int)osv.dwMajorVersion, (int)osv.dwMinorVersion);
	if (build) {
	    cp += strlen(cp);
	    sprintf(cp, ".%d", build);
	}
	if (server)
	    strcat(cp, " server");
    }
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
