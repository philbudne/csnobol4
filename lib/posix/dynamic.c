/* $Id$ */

/* allocate dynamic region for POSIX 1003.1b-1993 systems */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/mman.h>

#ifdef HAVE_STDLIB_H                    /* before stdio */
#include <stdlib.h>                     /* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

/* for lib.h: */
#include <stdio.h>
#include "h.h"
#include "snotypes.h"
#include "lib.h"			/* own prototypes */

static char *dbase;
static int dsize;

char *
dynamic( size )
    int size;
{
    dsize = size;
    dbase = malloc(size);
    return dbase;
}

void
vm_gc_advise(gc)
    int gc;
{
    if (gc)
	madvise(dbase, dsize,  MADV_RANDOM); /* random during GC */
    else
	madvise(dbase, dsize,  MADV_NORMAL); /* normal */
}
