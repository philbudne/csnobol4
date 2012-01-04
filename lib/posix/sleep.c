/*
 * test:
 *  cc -o sleep sleep.c -I../../include -DTEST
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, FILE */
#include <time.h>

#include "snotypes.h"
#include "h.h"
#include "lib.h"

int
sleepf(real_t t) {
    struct timespec ts;

    ts.tv_sec = (int) t;
    /* 5e-10 + 5e-10 < 1e-9 !! */
    ts.tv_nsec = (t - ts.tv_sec + 0.000000000501) * 1000000000;
#ifdef TEST
    printf("%e => %d %d\n", t, ts.tv_sec, ts.tv_nsec);
#endif
    if (!ts.tv_sec && !ts.tv_nsec)
	return 0;
    return nanosleep(&ts, NULL);
}

#ifdef TEST
main() {
    sleepf(0.0000000005);
    sleepf(0.5);
    sleepf(10);
}
#endif
