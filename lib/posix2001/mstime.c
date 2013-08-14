/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <time.h>

#include "h.h"
#include "snotypes.h"			/* for int_t */
#include "lib.h"

/*
 * use POSIX.-2001 clock_gettime() call to get runtime
 * better resolution than getrusage on Linux??
 */

real_t
mstime() {
    struct timespec ts;
    register real_t x;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) < 0)
	return 0.0;

    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000000.0;
}
