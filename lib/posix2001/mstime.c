/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <time.h>

#include <sys/time.h>
#include <sys/resource.h>		/* getrusage() */

#include "h.h"
#include "snotypes.h"			/* for int_t */
#include "lib.h"

/*
 * use POSIX.1-2001 clock_gettime() call to get sum of user and system time
 * in nanosecond units (clock_gettime is 1003.1b-1993 ("POSIX.1b")?
 * Prefer getrusage (in usec) because it splits user and system times.
 * 
 * Emscripten 3.1.5 _LOOKS_ it has working/dummy clock_getcpuclockid
 * but clock_gettime fails.
 */

real_t
mstime(void) {
    static VAR clockid_t clockid;
    static VAR int status;
    struct timespec ts;

    if (status == 0) {
	if (clock_getcpuclockid(0, &clockid) == 0)
	    status = 1;
	else
	    status = -1;		/* no suitable clock? */
    }

    if (status > 0 && clock_gettime(clockid, &ts) == 0 && (ts.tv_sec || ts.tv_nsec)) {
	return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
    }
    else {
#ifdef CLK_TCK
	clock_t t = clock();
	if (t != (clock_t)-1) {
	    if (CLK_TCK == 1000)		/* milliseconds? */
		return (real_t) t;		/* yes, just return */
	    else
		return t*(real_t)(1000/CLK_TCK);
	}
#else
	return 0.0;
#endif
    }
}
