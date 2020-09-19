/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <windows.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

/*
 * The FILETIME data structure contains two 32-bit values that combine to
 * form a 64-bit count of 100-nanosecond time units.
 */

/* divisor for low 32 */
#define TICKSPERMS 10000.0

/* multiplier for high 32 (PLB 2020-09-19: was 429496.0) */
#define HIGHMS 429496.7296	 /* (2**32)/TICKSPERMS (4.97 days!) */

real_t
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
	static FILETIME t0;

	if (t0.dwHighDateTime || t0.dwLowDateTime) {
	    FILETIME t;

	    GetSystemTimeAsFileTime(&t);
	    return ((t.dwHighDateTime - t0.dwHighDateTime) * HIGHMS +
		    (t.dwLowDateTime - t0.dwLowDateTime) / TICKSPERMS);
	}
	else {
	    GetSystemTimeAsFileTime(&t0);
	    return 0;
	}
#else  /* NO_STATIC_VARS defined */
	FILETIME *t0p;
	if (timevars) {
	    FILETIME t;

	    t0p = timevars;
	    GetSystemTimeAsFileTime(&t);
	    return ((t.dwHighDateTime - t0p->dwHighDateTime) * HIGHMS +
		    (t.dwLowDateTime - t0p->dwLowDateTime) / TICKSPERMS);
	}
	else {
	    timevars = t0p = malloc(sizeof(FILETIME));
	    GetSystemTimeAsFileTime(t0p);
	    return 0;
	}
#endif /* NO_STATIC_VARS defined */
    }
}
