/* $Id$ */

# include "h.h"
# include "snotypes.h"
# include "macros.h"

# ifdef VAXC
# include <types.h>			/* time_t */
# else  /* VAXC not defined */
# include <sys/types.h>			/* time_t */
# endif /* VAXC not defined */
# include <time.h>			/* struct tm */

/*
 * The format of DATE() is (in principle) system dependant,
 * (ie; could use ctime()) this returns what SPITBOL does,
 * which is what '360 MAINBOL plus the time!
 *
 * localtime() exists in v6, but "struct tm" doesn't!
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
	    tm->tm_year % 100,		/* ?! */
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
