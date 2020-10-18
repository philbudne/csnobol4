/* $Id$ */

/* allocate dynamic region for POSIX 1003.1b-1993 systems */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/mman.h>

#include <stdlib.h>                     /* for malloc */

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

static VAR char *dbase;
static VAR size_t dsize;

char *
dynamic(size_t size) {
    dsize = size;
    dbase = malloc(size);
    return dbase;
}

void
vm_gc_advise(int gc) {
    if (gc)
	posix_madvise(dbase, dsize,  POSIX_MADV_RANDOM); /* random during GC */
    else
	posix_madvise(dbase, dsize,  POSIX_MADV_NORMAL); /* normal */
}
