/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "snotypes.h"
#include <time.h>			/* clock_t, CLK_TCK */

/*
 * use ANSI clock() function to get runtime (returns user+system)
 */

#ifndef CLK_TCK
#ifdef CLOCKS_PER_SEC
#define CLK_TCK CLOCKS_PER_SEC
#else
#define CLK_TCK 1000000			/* popular value? */
#endif
#endif

real_t
mstime() {
    clock_t t;
    int ticks;

    t = clock();			/* sigh; includes system time */
    if (t == (clock_t)-1)
	return 0.0;

    ticks = CLK_TCK;
    if (ticks == 1000)
	return (real_t)t;

    if (ticks < 1000)			/* ie; BSD4.4 */
	return(t*1000.0/ticks);

    /* XXX check for ticks == 0? */

    return(t/(ticks/1000.0));
}
