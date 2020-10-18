/*
 * $Id$
 * module destructor for Windows native DLL (MSVC and MINGW)
 */

extern "C" {
#include "snotypes.h"			/* struct descr */
#include "h.h"				/* EXPOIRT/IMPORT, TLS */
#include "load.h"			/* SNOEXP */
#define MODULE_SUPPORT
#include "handle.h"			/* struct module, MODULE_xxx */

TLS struct module module = { MODULE_STRUCT_INIT };
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// NOTE! DllMain is a C++ API, or I wouldn't have had to make a
// .cpp file for this!
BOOL WINAPI
DllMain(HINSTANCE dllinst, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
	// "reserved is NULL for dynamic loads
	// and non-NULL for static loads" (!)
#if !IS_THREADED
	 MODULE_INIT(module);
#endif
	// can return FALSE on init failure,
	// system will immediately call back w/ DLL_PROCESS_DETACH
	break;
    case DLL_THREAD_ATTACH:
#if IS_THREADED
	 MODULE_INIT(module);
#endif
	 break;
    case DLL_THREAD_DETACH:
#if IS_THREADED
	 MODULE_CLEANUP(module);
#endif
	 break;
    case DLL_PROCESS_DETACH:
	// "reserved is NULL if FreeLibrary has been called or the DLL
	// load failed and non-NULL if the process is terminating."
#if !IS_THREADED
	 MODULE_CLEANUP(module);
#endif
	 break;
    }
    // only honored for DLL_PROCESS_ATTACH:
    return 1;
}
