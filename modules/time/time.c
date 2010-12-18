/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

#include <time.h>			/* struct tm */
#include <string.h>

#ifndef WIN32				/* XXX */
#include <sys/time.h>
#define HAVE_STRPTIME			/* XXX */
#define HAVE_GETTIMEOFDAY
#endif

#define SETINT(DP,N,VAL) (DP)[N].a.i = (VAL); (DP)[N].f = 0; (DP)[N].v = I

int
GETTIMEOFDAY2( LA_ALIST ) LA_DCL
{
    struct descr *dp = LA_PTR(0);
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
	RETFAIL;
    SETINT(dp,1,tv.tv_sec);
    SETINT(dp,2,tv.tv_usec);
#else
    SETINT(dp,1,time(0));
    SETINT(dp,1,0);
#endif
    RETNULL;
}

static void
tm2sno(struct tm *tmp, struct descr *dp)
{
    SETINT(dp,1,tmp->tm_sec);
    SETINT(dp,2,tmp->tm_min);
    SETINT(dp,3,tmp->tm_hour);
    SETINT(dp,4,tmp->tm_mday);
    SETINT(dp,5,tmp->tm_mon);
    SETINT(dp,6,tmp->tm_year);
    SETINT(dp,7,tmp->tm_wday);
    SETINT(dp,8,tmp->tm_yday);
    SETINT(dp,9,tmp->tm_isdst);
    /* gmtoff is non-standard! */
}

static void
sno2tm(struct descr *dp, struct tm *tmp)
{
    memset(tmp, 0, sizeof(struct tm));
    tmp->tm_sec = dp[1].a.i;
    tmp->tm_min = dp[2].a.i;
    tmp->tm_hour = dp[3].a.i;
    tmp->tm_mday = dp[4].a.i;
    tmp->tm_mon = dp[5].a.i;
    tmp->tm_year = dp[6].a.i;
    tmp->tm_wday = dp[7].a.i;
    tmp->tm_yday = dp[8].a.i;
    tmp->tm_isdst = dp[9].a.i;
    /* gmtoff is non-standard! */
}

int
LOCALTIME2( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = localtime(&t);
    tm2sno(tmp, LA_PTR(1));
    RETNULL;
}

int
GMTIME2( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = gmtime(&t);
    tm2sno(tmp, LA_PTR(1));
    RETNULL;
}

int
STRFTIME( LA_ALIST ) LA_DCL
{
    char format[1024];
    char output[1024];
    struct tm tm;

    getstring(LA_PTR(0), format, sizeof(format));
    sno2tm(LA_PTR(1), &tm);
    strftime(output, sizeof(output), format, &tm);
    RETSTR(output);
}

int
MKTIME( LA_ALIST ) LA_DCL
{
    struct tm tm;
    time_t ret;
    sno2tm(LA_PTR(0), &tm);
    mktime(&tm);
    tm2sno(&tm, LA_PTR(0));		/* normalized?! */
    if (ret < 0)
	RETFAIL;
    RETINT(ret);
}

#ifdef HAVE_STRPTIME
int
STRPTIME( LA_ALIST ) LA_DCL
{
    char input[1024];
    char format[1024];
    struct tm tm;
    char *ret;

    getstring(LA_PTR(0), input, sizeof(input));
    getstring(LA_PTR(1), format, sizeof(format));
    ret = strptime(input, format, &tm);
    tm2sno(LA_PTR(1), &tm);		/* only on success?? */
    if (ret) {
      RETSTR(ret);
    }
    RETFAIL;
}
#endif

