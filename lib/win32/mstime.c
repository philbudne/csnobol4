/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#include "snotypes.h"
#include <windows.h>			/* ??? */

#define NSPERMS 10000L			/* nanoseconds per millisecond */
#define HIGHMS 429496			/* (2^32)/NSPERMS */

int_t
mstime()
{
    FILETIME start, exit, kernel, user;

    /* only implemented on NT? */
    if !(GetProcessTimes(GetCurrentProcess(), &start, &exit, &kernel, &user)) {
	SYSTEMTIME t;
	static int haveold, old;
	static FILETIME f[2];
	int old;
	int_t t;

	/* XXX just use clock()? */

	new = !old;
	GetSystemTime(&t);
	SystemTimeToFileTime(&t, &f[new]);

	if (!haveold) {
	    t = 0;
	    haveold = 1;
	}
	else
	    t = (f[old].dwHighDateTime - f[new].dwHighDateTime) * HIGHMS +
		(f[old].dwLowDateTime - f[new].dwLowDateTime) / NSPERMS;
	old = new;
	return t;
    }
    return (user.dwHighDateTime * HIGHMS + user.dwLowDateTime / NSPERMS);
}
