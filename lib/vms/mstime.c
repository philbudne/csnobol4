/* $Id$ */

#include "snotypes.h"
#include <time.h>

/*
 * mstime for vms
 * J. Leighter
 */

/*
 * times() is venerable (it was in v6)!!
 * (But it seems to have changed over the years).
 */

int_t
mstime() {
    struct tbuffer tbuff;

    times(&tbuff);
    return(tbuff.proc_user_time * 10);		/* time in 10-ms units	*/
}
