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

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#define SETINT(DP,N,VAL) (DP)[N].a.i = (VAL); (DP)[N].f = 0; (DP)[N].v = I

/*
 * GETTIMEOFDAY_(TIMEVAL)
 */
int
GETTIMEOFDAY_( LA_ALIST ) LA_DCL
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
#ifdef HAVE_TM_GMTOFF
    SETINT(dp,10,tmp->tm_gmtoff);
#else
    SETINT(dp,10,-1);
#endif
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
#ifdef HAVE_TM_GMTOFF
    tmp->tm_gmtoff = dp[10].a.i;
#endif
}

/*
 * LOCALTIME_(INTEGER,TM)
 */
int
LOCALTIME_( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = localtime(&t);
    tm2sno(tmp, LA_PTR(1));
    RETNULL;
}

/*
 * GMTIME_(INTEGER,TM)
 */
int
GMTIME_( LA_ALIST ) LA_DCL
{
    time_t t = LA_INT(0);
    struct tm *tmp = gmtime(&t);
    tm2sno(tmp, LA_PTR(1));
    RETNULL;
}

/*
 * STRFTIME(STRING,TM)STRING
 */
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

/*
 * MKTIME(TM)INTEGER
 */

int
MKTIME( LA_ALIST ) LA_DCL
{
    struct tm tm;
    time_t ret;
    sno2tm(LA_PTR(0), &tm);
    ret = mktime(&tm);
    tm2sno(&tm, LA_PTR(0));		/* normalized?! */
    if (ret == -1)
	RETFAIL;
    RETINT(ret);
}

#ifdef HAVE_STRPTIME
/*
 * STRPTIME(STRING,STRING,TM)
 */
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
    tm2sno(LA_PTR(2), &tm);		/* only on success?? */
    if (ret) {
      RETSTR(ret);
    }
    RETFAIL;
}
#endif

