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
 * use POSIX.1-2001 clock_gettime() call to get
 * better resolution than getrusage on Linux.
 */

real_t
mstime() {
    static clockid_t clockid;
    static int status = 0;
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
	struct rusage ru;

	status = -1;
	if (getrusage( RUSAGE_SELF, &ru ) < 0)
	    return 0.0;
	return ru.ru_utime.tv_sec * 1000.0 + ru.ru_utime.tv_usec / 1000.0;
    }
}
