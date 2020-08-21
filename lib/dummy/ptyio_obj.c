/*
 * $Id$
 * forkexecpty -- dummy version
 * Phil Budne
 * 2020-08-20
 */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
forkexecpty(cmd, pio, ppid)
    char *cmd;
    long *pio;
    long *ppid;
{
    return -1;
}

int
waitpty(f, io, ptypid)
    FILE *f;
    long io;
    long ptypid;
{
    return 0;
}
