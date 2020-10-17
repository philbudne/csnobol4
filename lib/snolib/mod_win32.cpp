/*
 * $Id$
 * module destructor for Windows native DLL (MSVC and MINGW)
 */

extern "C" {
#include "snotypes.h"
#include "h.h"
#include "load.h"		// SNOEXP
#include "handle.h"

extern struct module module;
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// NOTE! This is a C++ API, or else I would have burried
// it in MODULE macro!
BOOL WINAPI
DllMain(HINSTANCE dllinst, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
	// "reserved is NULL for dynamic loads
	// and non-NULL for static loads!"
	if (!module.threaded)
	    module.htlist = NULL;
	// can return FALSE on init failure,
	// system will immediately call back w/ DLL_PROCESS_DETACH
	break;
    case DLL_THREAD_ATTACH:
	if (module.threaded)
	    module.htlist = NULL;
	break;
    case DLL_THREAD_DETACH:
	if (module.threaded)
	    handle_cleanup(module.htlist);
	break;
    case DLL_PROCESS_DETACH:
	// "reserved is NULL if FreeLibrary has been called or the DLL
	// load failed and non-NULL if the process is terminating."
	if (!module.threaded)
	    handle_cleanup(module.htlist);
	break;
    }
    // only honored for DLL_PROCESS_ATTACH:
    return 1;
}
