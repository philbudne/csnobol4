/* $Id$ */

/* allocate dynamic region on BSD */

#include <sys/vadvise.h>

char *malloc();

/* on SunOS use valloc(3) + madvise(2)?? */

char *
dynamic( size )
    int size;
{
    vadvise(VA_ANOM);			/* warn VM we're random during GC */
    return malloc(size);
}
