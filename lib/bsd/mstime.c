/* $Id$ */

#include "snotypes.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

/*
 * use Berkeley rusage() call to get runtime
 * usually has better resolution than times()
 *	(and you don't need to know HZ)
 */

int_t
mstime() {
    struct rusage ru;
#ifndef NO_GETRUSAGE_BUG
    unsigned long x;
    static unsigned long last = 0;	/* XXX belongs in vars.h? */
#endif /* NO_GETRUSAGE_BUG not defined */

    getrusage( RUSAGE_SELF, &ru );	/* XXX check return? */
    x = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec / 1000;

#ifndef NO_GETRUSAGE_BUG
    /*
     * Ensure we'll never give negative deltas;
     * BSD/OS 3.1 on 486's can, due to "statistical sampling"
     * Older versions of NetBSD could as well.
     */
    if (x < last)
	x = last;
#endif /* NO_GETRUSAGE_BUG not defined */

    return x;
}
