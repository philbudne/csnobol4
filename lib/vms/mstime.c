/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <time.h>
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * mstime for VMS
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

real_t
mstime() {
    struct tbuffer tbuff;

    times(&tbuff);
    return(tbuff.proc_user_time * 1000.0 / CLK_TCK);
}
