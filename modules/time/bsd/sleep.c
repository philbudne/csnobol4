/*
 * test:
 *  cc -o sleep sleep.c -I../../include -DTEST
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* NULL, FILE */

#ifdef HAVE_SELECT_H
#include <sys/select.h>			/* (POSIX.1-2001) */
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>	
#endif

#include "snotypes.h"
#include "h.h"
#include "lib.h"

int
sleepf(real_t t) {
    struct timeval tv;

    tv.tv_sec = (int) t;
    /* 0.0000005 + 0.0000005 < 0.000001! */
    tv.tv_usec = (t - tv.tv_sec + 0.000000501) * 1000000;
    /* printf("%d %d\n", tv.tv_sec, tv.tv_usec); */
    if (!tv.tv_sec && !tv.tv_usec)
	return 0;
    return select(0, NULL, NULL, NULL, &tv);
}

#ifdef TEST
main() {
    sleepf(0.5);
}
#endif
