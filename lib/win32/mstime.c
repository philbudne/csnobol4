/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include "snotypes.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

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
    FILETIME start, texit, kernel, user;

    /*
     * only implemented on NT?
     * XXX cache system type? process handle??
     */
    if (GetProcessTimes(GetCurrentProcess(), &start, &texit, &kernel, &user)) {
	return (user.dwHighDateTime * HIGHMS +
		user.dwLowDateTime / TICKSPERMS);
    }
    else {
	/* Win95/98/ME isn't really an operating system,
	 * so it doesn't track runtime!! Use time of day instead??
	 * XXX just use ANSI clock() function???
	 */
#ifndef NO_STATIC_VARS
	static int have_t0;
	static FILETIME t0;
#else  /* NO_STATIC_VARS defined */
	/* FIXME */
#endif /* NO_STATIC_VARS defined */

	if (have_t0) {
	    FILETIME t;

	    GetSystemTimeAsFileTime(&t);
	    return ((t.dwHighDateTime - t0.dwHighDateTime) * HIGHMS +
		    (t.dwLowDateTime - t0.dwLowDateTime) / TICKSPERMS);
	}
	else {
	    GetSystemTimeAsFileTime(&t0);
	    have_t0 = 1;
	    return 0;
	}
    }
}
