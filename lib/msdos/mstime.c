/* $Id$ */

#include "snotypes.h"
#include <time.h>

static int first_time = -1;
static long start_time;

/*
 * times() is venerable (it was in v6)!!
 */

int_t
mstime() {
    clock_t t;

    if (first_time == -1) {
	start_time = (clock_t) clock();
	first_time = -first_time;
	return 0L;
    }

    t = (clock_t) clock();

    if (t < start_time)
	t += 86400L * CLOCKS_PER_SEC;	/* sec/day * clocks/sec */

    /*
     * Convert to milliseconds.
     *  elapsed clocks * (1000 msec/sec) / (CLOCKS_PER_SEC clocks/sec) =
     *  elapsed clocks * (1000 / CLOCKS_PER_SEC) msec/clocks
     *  elapsed * (1000 / CLOCKS_PER_SEC) msec
     */
    return (t - start_time) * (1000 / CLOCKS_PER_SEC);
}
