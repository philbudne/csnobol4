/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#include "snotypes.h"
#include <windows.h>			/* ??? */

#define NSPERMS 10000L

int_t
mstime()
{
    FILETIME creation, exit, kernel, user;

    /* XXX cache process handle? */
    /* XXX noop on Win95? */
    GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user);

    return (user.dwHighDateTime * ((1LL<<32)/NSPERMS) +
	    user.dwLowDateTime / NSPERMS);
}
