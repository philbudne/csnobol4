/* $Id$ */

/*
 * init.c - init data segment
 * 12/28/93
 *
 * * guarantees that data is in same order as in SIL source
 * * avoids initializers on unions (illegal in K&R style compilers)
 */

# include "snotypes.h"
# include "h.h"
# include "macros.h"
# include "units.h"

/* machine generated */
# include "equ.h"
# include "res.h"
# include "data.h"
# include "proc.h"

# include "charset.c"			/* character set-dependancies */

const char AMPST[] = "&";
const char QTSTR[] = "'";
const char COLSTR[] = ": ";

# define STRLEN(s) (sizeof(s)-1)	/* support for STRING */

void
init_data() {
# include "data_init.h"
} /* init_data */
