/* $Id$ */

/* get user runtime on Win32 pb 12/22/97 */

#include <windows.h>
#include "snotypes.h"

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif

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
	int new;
	int_t x;
#ifdef NO_STATIC_VARS
	static int haveold, old;
	static FILETIME f[2];
#else
	int haveold;
	struct timevars {
	    int tv_old;
	    FILETIME tv_f[2];
	} *tv;

	if (timeptr == NULL) {
	    haveold = FALSE;
	    tv = (struct timevars *) malloc(sizeof(struct timevars));
	    if (!tv) {
		perror("could not malloc timevars");
		exit(1);
	    }
	    timeptr = tv;
#define old tv->tv_old
#define f tv->tv_f
	    old = 0;
	    haveold = 0;
	}
	else {
	    haveold = 1;
	    tv = timeptr;
	}
#endif /* NO_STATIC_VARS defined */

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

	if (haveold)
	    x = (f[new].dwHighDateTime - f[old].dwHighDateTime) * HIGHMS +
		(f[new].dwLowDateTime - f[old].dwLowDateTime) / TICKSPERMS;
	else
	    x = 0;

	old = new;
	haveold = 1;

	return x;
    }
}
