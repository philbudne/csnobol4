/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "h.h"
#include "snotypes.h"			/* for int_t */
#include "lib.h"

/*
 * use Berkeley rusage() call to get runtime
 * usually has better resolution than times()
 *	(and you don't need to know HZ)
 */

#ifdef GETRUSAGE_BUG
static real_t last_mstime = 0.0;	/* XXX belongs in vars.h? */
#endif /* GETRUSAGE_BUG defined */

real_t
mstime() {
    struct rusage ru;
    register real_t x;

    getrusage( RUSAGE_SELF, &ru );	/* XXX check return? */
    x = ru.ru_utime.tv_sec * 1000.0 + ru.ru_utime.tv_usec / 1000.0;

#ifdef GETRUSAGE_BUG
    /*
     * Ensure we'll never give negative deltas due to "statistical sampling"
     * in BSD44 systems (problem seen with NetBSD, OpenBSD and BSD/OS).
     *
     * Early in a process lifetime the method used to calculate how
     * much of the current tick should be attributed to user or system
     * usage can cause user time to step backwards!!  user+system will
     * never have this property, but we only want user time!!
     *
     * utime[now]/total[now] * (total[now]-total[last])
     * might yield better results.
     */
    if (x < last_mstime)
	x = last_mstime;
    else
	last_mstime = x;
#endif /* GETRUSAGE_BUG defined */

    return x;
}
