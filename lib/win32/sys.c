/* $Id$ */

/* support for HOST() on Win32 pb 12/22/97 */

#include <windows.h>

static v
void
hwname(cp)
    char *cp;
{
    char *hw;
    SYSTEM_INFO si;

    GetSystemInfo(&si);			/* XXX check return? */

    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL:
#if 0
	/* sprintf(cp, "i%d", si.dwProcessorType ); */
	sprintf(cp, "i%c86", si.wProcessorLevel + '0');
	return;
#else
	cp = "x86";
	break;
#endif

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
#if 0
	sprintf(cp, "arch %d", si.wProcessorArchitecture);
#else
	/* values seem to be mnemonic */
	sprintf(cp, "unk%d", si.dwProcessorType );
#endif
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

    osv.dwOSVersionInfoSize = sizeof(osv);
    GetVersionEx(&osv);

    switch (osv.dwPlatformId) {
    case VER_PLATFORM_WIN32s:
	os = "Win32s";
	break;

    case VER_PLATFORM_WIN32_WINDOWS:
	/*
	 * From MS Win95 OSR 2 docs;
	 *
	 * If the dwPlatformId member of that structure is
	 * VER_PLATFORM_WIN32_WINDOWS, and the low word of the
	 * dwBuildNumber member is "1111," the system is running
	 * Windows 95 OSR 2. For Windows 95 and Windows 95 OSR 2, the
	 * dwMajorVersion element returns "4" and dwMinorVersion
	 * returns "0." Future versions or OSRs of Windows 95 will
	 * have higher build numbers. They may also have higher
	 * major/minor version numbers.
	 */
	switch (osv.dwMajorVersion) {
	case 4:
	    os = "Win95";
	    break;
	default:
	    os = "Win??";
	}
	break;

    case VER_PLATFORM_WIN32_NT:
	os = "WinNT";
	break;

    default:
	os = "Win???";
	break;
    }

    /* XXX include dwBuildNumber? */
    sprintf(cp, "%s %d.%d", os, osv.dwMajorVersion, osv.dwMinorVersion );
}
