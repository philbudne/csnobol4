/* to test: cc -I../../include -o sleep sleep.c -D TEST */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifndef TEST
#include <windows.h>
#endif

#include "snotypes.h"
#include "h.h"
#include "lib.h"

#define DAYSEC (24*60*60)

#ifdef TEST
void
Sleep(unsigned int ms) {
#ifdef VERBOSE
    printf(" Sleep %d\n", ms);
#endif
}
#endif

int
sleepf(real_t t) {
    int sec = (int) t;
    int days = sec / DAYSEC;
    int ms = (t - days*DAYSEC) * 1000 + 0.5;

#ifdef TEST
    printf("%d days, %d ms\n", days, ms);
#endif
    for (; days > 0; days--)
	Sleep(DAYSEC*1000);
    if (ms)
	Sleep(ms);
    return 0;
}

#ifdef TEST
main() {
    sleepf(0.5);
    sleepf(0.05);
    sleepf(0.005);
    sleepf(0.0005);
    sleepf(10.0);
    sleepf(10.5);
    sleepf(DAYSEC + 0.5);
    sleepf(DAYSEC*10 + 0.5);
    sleepf(DAYSEC*100 + 0.5);
    sleepf(DAYSEC*1000 + 0.5);
}
#endif
