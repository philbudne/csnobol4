/* $Id$ */

#include "h.h"
#include "types.h"
#include "macros.h"

#include <sys/types.h>			/* time_t */
#include <time.h>			/* struct tm */

/*
 * The format of DATE() is (in principal) system dependant,
 * (ie; could use ctime()) this returns what SPITBOL does
 */

void
date( sp )
    struct spec *sp;
{
    time_t t;
    static char str[ 6*3 ];
    struct tm *tm;

    time( &t );
    tm = localtime( &t );
    sprintf( str, "%02d/%02d/%02d %02d:%02d:%02d",
	    tm->tm_mon + 1,
	    tm->tm_mday,
	    tm->tm_year,
	    tm->tm_hour,
	    tm->tm_min,
	    tm->tm_sec );
    S_A(sp) = (int_t) str;
    S_L(sp) = strlen(str);
    S_V(sp) = 0;
    S_F(sp) = 0;
    S_O(sp) = 0;
    CLR_S_UNUSED(sp);
}
