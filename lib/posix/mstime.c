/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/times.h>			/* appeared post-v7?! */
#include <unistd.h>			/* for _SC_CLK_TCK */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * times() is venerable (it was in v6)!!
 * use POSIX sysconf() to get hz!
 */

real_t
mstime(void) {
    struct tms tms;
    int hz;

    hz = sysconf(_SC_CLK_TCK);
    if (hz == -1)
	return 0.0;			/* just use 60?? */

    times(&tms);
    return(tms.tms_utime*1000.0/hz);	/* just user time? */
}
