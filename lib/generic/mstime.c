/* $Id$ */

#include "types.h"
#include <sys/types.h>
#include <sys/times.h>			/* appeared post-v7?! */

/*
 * times() is venerable (it was in v6)!!
 */

#ifndef HZ
/* HZ define is likely to be in some sys include file,
 * but this is supposed to be the ultra-generic version!!!
 */
#define HZ 60				/* sigh */
#endif /* HZ not defined */

int_t
mstime() {
    struct tms tms;

    times(&tms);
    return(tms.tms_utime*1000/HZ);	/* just user time? */
}
