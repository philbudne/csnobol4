/* $Id$ */
 
/*
 * dummy popen()/pclose()
 * September 24, 1997
 */

#include <stdio.h>

FILE *
popen(file, mode)
    char *file, *mode;
{
    return NULL;
}

int
pclose(f)
    FILE *f;
{
    return -1;
}
