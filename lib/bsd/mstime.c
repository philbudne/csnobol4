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

    getrusage( RUSAGE_SELF, &ru );	/* XXX check return? */
    return(ru.ru_utime.tv_sec * 1000 +
	   ru.ru_utime.tv_usec / 1000);
}
