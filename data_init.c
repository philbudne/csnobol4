/* $Id$ */

/*
 * function to init data segment;
 * * guarantees that data is in same order as in SIL source
 * * avoids initializers on unions (illegal in K&R style compilers)
 */

#include "types.h"
#include "flags.h"
#include "h.h"
#include "macros.h"
#include "units.h"

/* machine generated */
#include "equ.h"
#include "data.h"
#include "proc.h"

#define SIZLIM 0x7fffffff		/* XXX move to types.h? */

#define STRLEN(s) (sizeof(s)-1)		/* XXX move to macros.h? */

/* from charset.c */
extern char AMPST[];
extern char COLSTR[];
extern char QTSTR[];
extern char ALPHA[];

#define ALPHSZ 128			/* XXX */

void
init_data() {
#include "init.h"
} /* init_data */
