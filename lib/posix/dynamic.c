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

/* prefer standard POSIX interface; fall back to BSD */
#if !defined(POSIX_MADV_RANDOM) && defined(MADV_RANDOM)
#define POSIX_MADV_RANDOM MADV_RANDOM
#define POSIX_MADV_NORMAL MADV_NORMAL
#define posix_madvise madvise
#endif /* !defined(POSIX_MADV_RANDOM) && defined(MADV_RANDOM) */

/* for lib.h: */
#include <stdio.h>
#include "h.h"
#include "snotypes.h"
#include "lib.h"			/* own prototypes */

static char *dbase;
static size_t dsize;

char *
dynamic( size )
    size_t size;
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
	posix_madvise(dbase, dsize,  POSIX_MADV_RANDOM); /* random during GC */
    else
	posix_madvise(dbase, dsize,  POSIX_MADV_NORMAL); /* normal */
}
