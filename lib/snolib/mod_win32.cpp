/*
 * $Id$
 * module destructor for Windows native DLL (MSVC and MINGW)
 */

extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "snotypes.h"			/* struct descr */
#include "h.h"				/* EXPORT/IMPORT, TLS */
#include "load.h"			/* SNOLOAD_API */
#define MODULE_SUPPORT
#include "handle.h"			/* struct module, MODULE_xxx */

TLS struct module module = { MODULE_STRUCT_INIT };
} // extern "C"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef DEBUG
#include <stdio.h>
#define PUT(S) puts(S)
#else
#define PUT(S) (void)0
#endif

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
	 PUT("DLL_PROCESS_ATTACH");
	// can return FALSE on init failure,
	// system will immediately call back w/ DLL_PROCESS_DETACH
	break;
    case DLL_THREAD_ATTACH:
#if IS_THREADED
	 MODULE_INIT(module);
#endif
	 PUT("DLL_THREAD_ATTACH");
	 break;
    case DLL_THREAD_DETACH:
#if IS_THREADED
	 MODULE_CLEANUP(module);
#endif
	 PUT("DLL_THREAD_DETACH");
	 break;
    case DLL_PROCESS_DETACH:
	// "reserved is NULL if FreeLibrary has been called or the DLL
	// load failed and non-NULL if the process is terminating."
#if !IS_THREADED
	 MODULE_CLEANUP(module);
#endif
	 PUT("DLL_PROCESS_DETACH");
	 break;
    }
    // only honored for DLL_PROCESS_ATTACH:
    return 1;
}
