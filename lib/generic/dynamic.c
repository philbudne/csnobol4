/* $Id$ */

/* allocate dynamic region */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

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
}
