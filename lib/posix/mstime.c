/* $Id$ */

#include "snotypes.h"
#include <sys/types.h>
#include <sys/times.h>			/* appeared post-v7?! */
#include <unistd.h>			/* for _SC_CLK_TCK */
/*
 * times() is venerable (it was in v6)!!
 * use POSIX sysconf() to get hz!
 */

int_t
mstime() {
    struct tms tms;
    int hz;

    hz = sysconf(_SC_CLK_TCK);
    if (hz == -1)
	return 0;			/* just use 60?? */

    times(&tms);
    return(tms.tms_utime*1000/hz);	/* just user time? */
}
