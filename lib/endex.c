/* $Id$ */

#include "types.h"

extern struct descr RETCOD;
void
endex( x )
    int x;
{
    /* if &ABEND set, dump core?! */
    if (x) {
	/* XXX perform I/O cleanup?! */
	abort();
    }

    /* else exit w/ &CODE */
    exit(RETCOD.a.i);
}

