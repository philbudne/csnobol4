/* $Id$ */

/* allocate dynamic region on BSD */

#include <sys/vadvise.h>

char *malloc();

/* on SunOS use valloc(3) + madvise(2)?? */

char *
dynamic( size )
    int size;
{
    return malloc(size);
}

void
vm_gc_advise(gc)
    int gc;
{
    if (gc)
	vadvise(VA_ANOM);		/* warn VM we're random during GC */
    else
	vadvise(VA_NORM);		/* normal */
}
