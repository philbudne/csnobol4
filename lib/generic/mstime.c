/* $Id$ */

#include "types.h"
#include <sys/types.h>
#include <sys/times.h>			/* appeared post-v7?! */

/*
 * times() is venerable (it was in v6)!!
 */

#define HZ 60				/* sigh */

int_t
mstime() {
    struct tms tms;

    times(&tms);
    return(tms.tms_utime*1000/HZ);	/* just user time? */
}
