/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#include <windows.h>
#include "snotypes.h"

/*
 * The FILETIME data structure contains two 32-bit values that combine to
 * form a 64-bit count of 100-nanosecond time units.
 */

/* divisor for low 32 */
#define TICKSPERMS 10000

/* multiplier for high 32; */
#define HIGHMS 429496			/* (2**32)/TICKSPERMS */

int_t
mstime()
{
    FILETIME start, exit, kernel, user;

    /*
     * only implemented on NT?
     * XXX cache system type? process handle??
     */
    if (GetProcessTimes(GetCurrentProcess(), &start, &exit, &kernel, &user)) {
	return (user.dwHighDateTime * HIGHMS +
		user.dwLowDateTime / TICKSPERMS);
    }
    else {
	/* XXX just use clock()? */
	static int haveold, old;
	static FILETIME f[2];
	int new;
	int_t x;


	new = !old;
#if 1
	GetSystemTimeAsFileTime(&f[new]);
#else
	{
	    SYSTEMTIME t;

	    GetSystemTime(&t);
	    SystemTimeToFileTime(&t, &f[new]);
	}
#endif

	if (!haveold) {
	    x = 0;
	    haveold = 1;
	}
	else
	    x = (f[old].dwHighDateTime - f[new].dwHighDateTime) * HIGHMS +
		(f[old].dwLowDateTime - f[new].dwLowDateTime) / TICKSPERMS;
	old = new;
	return x;
    }
}
