/* $Id$ */

#include "h.h"				/* for data.h */
#include "snotypes.h"
#include "macros.h"
#include "data.h"			/* for RETCOD */

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
    exit(D_A(RETCOD));
}

