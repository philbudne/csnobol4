/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <time.h>			/* clock_t, CLK_TCK */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * use ANSI clock() function to get runtime (returns user+system)
 */

#ifndef CLK_TCK
#ifdef CLOCKS_PER_SEC
#define CLK_TCK CLOCKS_PER_SEC
#else  /* CLOCKS_PER_SEC not defined */
#define CLK_TCK 1000000			/* popular value? */
#endif /* CLOCKS_PER_SEC not defined */
#endif /* CLK_TCK not defined */

real_t
mstime(void) {
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
