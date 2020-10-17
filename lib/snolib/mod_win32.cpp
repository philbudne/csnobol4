/*
 * $Id$
 * module destructor for Visual Studio
 */

extern "C" {
#include "snotypes.h"
#include "h.h"				/* __P() */
#include "load.h"			/* SNOEXP() */
#include "handle.h"

extern struct module module;
}

#include <process.h>

BOOL WINAPI
DllMain(HINSTANCE dllinst, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
	if (!module.threaded)
	    module.hhlist = NULL;
	break;
    case DLL_THREAD_ATTACH:
	if (module.threaded)
	    module.hhlist = NULL;
	break;
    case DLL_THREAD_DETACH:
	if (module.threaded)
	    handle_cleanup(&module);
	break;
    case DLL_PROCESS_DETACH:
	if (!module.threaded)
	    handle_cleanup(&module);
	break;
    }
    return 1;
}
