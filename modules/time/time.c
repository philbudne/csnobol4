/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#if defined(HAVE_GLIBC) || defined(linux) /* linux test for safety */
#define _XOPEN_SOURCE			/* glibc: enable strptime() */
#define _BSD_SOURCE			/* glibc: keep tm_gmtoff */
#define _DEFAULT_SOURCE	/* replaces above in glibc 2.20, quashes warnings */
#endif

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#include <stdio.h>			/* for lib.h */
#include <time.h>			/* time_t, strptime() */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "lib.h"			/* sleepf() */
#include "str.h"

#define SETINT(DP,N,VAL) (DP)[N].a.i = (VAL); (DP)[N].f = 0; (DP)[N].v = I
#define SETNULL(DP,N) (DP)[N].a.i = 0; (DP)[N].f = 0; (DP)[N].v = S

enum tv_member {
    TV_DESCR,
    TV_SEC,
    TV_USEC,
    TV_COUNT				/* must be last */
};

enum tm_member {
    TM_DESCR,
    TM_SEC,
    TM_MIN,
    TM_HOUR,
    TM_MDAY,
    TM_MON,
    TM_YEAR,
    TM_WDAY,
    TM_YDAY,
    TM_ISDST,
    TM_GMTOFF,
    TM_COUNT				/* must be last */
};

#define COUNT(DP) ((DP)->v/DESCR+1)

/*
 * LOAD("GETTIMEOFDAY_(TIMEVAL)", TIME_DL)
 */
lret_t
GETTIMEOFDAY_( LA_ALIST ) LA_DCL
{
    struct descr *dp = LA_PTR(0);
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif

    if (!dp || LA_TYPE(0) < DATSTA || COUNT(dp) != TV_COUNT)
	RETNULL;
    /* validate dp[TV_DESCR] */
#ifdef HAVE_GETTIMEOFDAY
    if (gettimeofday(&tv, NULL) < 0)
	RETFAIL;
    SETINT(dp,TV_SEC,tv.tv_sec);
    SETINT(dp,TV_USEC,tv.tv_usec);
#else
    SETINT(dp,TV_SEC,time(0));
    SETINT(dp,TV_USEC,0);
#endif
    RETNULL;
}

static int
tm2sno(struct tm *tmp, struct descr *dp)
{
    if (!dp || COUNT(dp) != TM_COUNT)
	return 0;
    SETINT(dp,TM_SEC,tmp->tm_sec);
    SETINT(dp,TM_MIN,tmp->tm_min);
    SETINT(dp,TM_HOUR,tmp->tm_hour);
    SETINT(dp,TM_MDAY,tmp->tm_mday);
    SETINT(dp,TM_MON,tmp->tm_mon);
    SETINT(dp,TM_YEAR,tmp->tm_year);
    SETINT(dp,TM_WDAY,tmp->tm_wday);
    SETINT(dp,TM_YDAY,tmp->tm_yday);
    SETINT(dp,TM_ISDST,tmp->tm_isdst);
#ifdef HAVE_TM_GMTOFF
    SETINT(dp,TM_GMTOFF,tmp->tm_gmtoff);
#else
    SETNULL(dp,TM_GMTOFF);
#endif
    return 1;
}

static int
sno2tm(struct descr *dp, struct tm *tmp)
{
    if (!dp || COUNT(dp) != TM_COUNT)
	return 0;
    memset(tmp, 0, sizeof(struct tm));
    /* accept int or empty string */
#define FETCH(X,Y) \
    if (dp[Y].v == I) tmp->X = dp[Y].a.i; \
    else if (dp[Y].v == S && dp[Y].a.ptr == 0) tmp->X = 0; \
    else return 0
    FETCH(tm_sec, TM_SEC);
    FETCH(tm_min, TM_MIN);
    FETCH(tm_hour, TM_HOUR);
    FETCH(tm_mday, TM_MDAY);
    FETCH(tm_mon, TM_MON);
    FETCH(tm_year, TM_YEAR);
    FETCH(tm_wday, TM_WDAY);
    FETCH(tm_yday, TM_YDAY);
    FETCH(tm_isdst, TM_ISDST);
#ifdef HAVE_TM_GMTOFF
    FETCH(tm_gmtoff, TM_GMTOFF);
#endif
    return 1;
}

/*
 * LOAD("LOCALTIME_(INTEGER,TM)", TIME_DL)
 */
lret_t
LOCALTIME_( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = localtime(&t);
    if (LA_TYPE(1) < DATSTA || !tm2sno(tmp, LA_PTR(1)))
	RETFAIL;
    RETNULL;
}

/*
 * LOAD("GMTIME_(INTEGER,TM)", TIME_DL)
 */
lret_t
GMTIME_( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = gmtime(&t);
    if (!tm2sno(tmp, LA_PTR(1)))
	RETFAIL;
    RETNULL;
}

#ifdef HAVE_STRFTIME
/*
 * LOAD("STRFTIME(STRING,TM)STRING", TIME_DL)
 */
lret_t
STRFTIME( LA_ALIST ) LA_DCL
{
    char format[1024];
    char output[1024];
    struct tm tm;

    getstring(LA_PTR(0), format, sizeof(format));
    if (!sno2tm(LA_PTR(1), &tm))
	RETFAIL;
    strftime(output, sizeof(output), format, &tm);
    RETSTR(output);
}
#endif

/*
 * LOAD("MKTIME(TM)INTEGER", TIME_DL)
 */

lret_t
MKTIME( LA_ALIST ) LA_DCL
{
    struct tm tm;
    time_t ret;

    if (!sno2tm(LA_PTR(0), &tm))
	RETFAIL;
    ret = mktime(&tm);
    if (ret == -1 || !tm2sno(&tm, LA_PTR(0)))
	RETFAIL;
    RETINT(ret);
}

#ifdef HAVE_SLEEP
/*
 * LOAD("SLEEP(REAL)", TIME_DL)
 */
lret_t
SLEEP( LA_ALIST ) LA_DCL
{
    if (sleepf(LA_REAL(0)) < 0)
	RETFAIL;			/* did not sleep full period */
    RETNULL;
}
#endif

#ifdef HAVE_STRPTIME
/*
 * LOAD("STRPTIME_(STRING,STRING,TM)", TIME_DL)
 */
lret_t
STRPTIME_( LA_ALIST ) LA_DCL
{
    char input[1024];
    char format[1024];
    struct tm tm;
    char *ret;

    getstring(LA_PTR(0), input, sizeof(input));
    getstring(LA_PTR(1), format, sizeof(format));
    memset(&tm, 0, sizeof(tm));		/* sno2tm(LA_PTR(2), &tm); ? */
    ret = strptime(input, format, &tm);
    if (ret && tm2sno(&tm, LA_PTR(2)))
	RETSTR(ret);			/* return remaining string */
    RETFAIL;
}
#endif

#ifdef HAVE_TIMEGM
/*
 * LOAD("TIMEGM(TM)INTEGER", TIME_DL)
 */

lret_t
TIMEGM( LA_ALIST ) LA_DCL
{
    struct tm tm;
    time_t ret;

    if (!sno2tm(LA_PTR(0), &tm))
	RETFAIL;
    ret = timegm(&tm);
    if (ret == -1 || !tm2sno(&tm, LA_PTR(0)))
	RETFAIL;
    RETINT(ret);
}
#endif
