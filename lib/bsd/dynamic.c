/* $Id$ */

/* allocate dynamic region on BSD */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/vadvise.h>

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

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
