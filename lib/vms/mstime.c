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
 *
 * VMS "tbuffer" members: {proc,child}_{user,system}_time
 * in CLK_TCK units
 */

#ifndef CLK_TCK
#define CLK_TCK 100
#endif /* CLK_TCK not defined */

int_t
mstime() {
    struct tbuffer tbuff;

    times(&tbuff);
    return(tbuff.proc_user_time * 1000 / CLK_TCK);
}
