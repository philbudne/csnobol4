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
 * use POSIX.-2001 clock_gettime() call to get runtime
 * better resolution than getrusage on Linux.
 */

real_t
mstime() {
    struct timespec ts;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) < 0 ||
	(ts.tv_sec == 0 && ts.tv_nsec == 0)) {
	struct rusage ru;

	if (getrusage( RUSAGE_SELF, &ru ) < 0)
	    return 0.0;
	return ru.ru_utime.tv_sec * 1000.0 + ru.ru_utime.tv_usec / 1000.0;
    }
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}
