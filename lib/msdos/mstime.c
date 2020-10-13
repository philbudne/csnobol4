/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <time.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

static VAR long start_time;

/*
 * sigh; use ansi-ish clock() defined as user + system,
 *	but wall clock on on MS-DOS?
 */

real_t
mstime(void) {
    clock_t t;

    if (start_time == 0) {
	start_time = clock();
	if (start_time == 0)
	    start_time = 1;		/* must be non-zero */
	return 0.0;
    }

    t = clock();
    if (t == 0 && start_time == 1)
	t = 1;
    if (t < start_time)
	t += 86400L * CLOCKS_PER_SEC;	/* sec/day * clocks/sec */

    /*
     * Convert to milliseconds.
     *  elapsed clocks * (1000 msec/sec) / (CLOCKS_PER_SEC clocks/sec) =
     *  elapsed clocks * (1000 / CLOCKS_PER_SEC) msec/clocks
     *  elapsed * (1000 / CLOCKS_PER_SEC) msec
     */
    return (t - start_time) * (1000.0 / CLOCKS_PER_SEC);
}
