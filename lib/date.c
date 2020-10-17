/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>			/* time_t */

#include <time.h>			/* struct tm */
#include <stdio.h>			/* for sprintf() */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "equ.h"			/* for "I" */
#include "lib.h"			/* for own prototype */
#include "str.h"

/*
 * The format of DATE() is (in principle) system dependant,
 * (ie; could use ctime()) this returns what SPITBOL does,
 * which is what '360 MAINBOL returns, plus the time!
 *
 * updated 9/21/96; now returns full julian year.
 * updated 4/2/97; takes optional arg per Catspaw SPITBOL
 *
 * localtime() exists in v6, but "struct tm" doesn't!
 */

static VAR char strbuf[21];

void
date(struct spec *sp, struct descr *dp) {
    time_t t;
    struct tm *tm;
    enum date_format { OLD=0, NEW=1, ISO=2 } format;

    if (D_V(dp) == I)
	format = (enum date_format) D_A(dp);
    else
	format = NEW;			/* default */

    time( &t );
    tm = localtime( &t );

    switch (format) {
    default:				/* out-of-range */
    case NEW:				/* SPITBOL new format */
	/* MM/DD/YYYY HH:MM:SS */
#ifdef HAVE_STRFTIME
	strftime(strbuf, sizeof(strbuf), "%m/%d/%Y %H:%M:%S", tm);
#else
	sprintf( strbuf, "%02d/%02d/%d %02d:%02d:%02d",
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_year + 1900,
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec );
#endif
	break;
    case OLD:				/* SPITBOL default */
	/* MM/DD/YY HH:MM:SS */
#ifdef HAVE_STRFTIME
	strftime(strbuf, sizeof(strbuf), "%m/%d/%y %H:%M:%S", tm);
#else
	sprintf( strbuf, "%02d/%02d/%02d %02d:%02d:%02d",
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_year % 100,
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec );
#endif
	break;
    case ISO:				/* ISO style with 4-digit year */
	/* YYYY-MM-DD HH:MM:SS */
#ifdef HAVE_STRFTIME
	strftime(strbuf, sizeof(strbuf), "%Y-%m-%d %H:%M:%S", tm);
#else
	sprintf( strbuf, "%d-%02d-%02d %02d:%02d:%02d",
		tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec );
#endif
	break;
    }

    S_A(sp) = (int_t) strbuf;
    S_L(sp) = strlen(strbuf);
    S_V(sp) = 0;
    S_F(sp) = 0;
    S_O(sp) = 0;
    CLR_S_UNUSED(sp);
}
