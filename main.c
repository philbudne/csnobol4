/* $Id$ */

/* snobol4 main program (make this mlink.c??) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

# include <stdlib.h>			/* for malloc */
# include <stdio.h>			/* for lib.h */
# include "h.h"
# include "snotypes.h"
# include "lib.h"			/* version() */
# include "gen.h"
# include "macros.h"

# ifdef SHARED
# include <setjmp.h>
VAR jmp_buf endex_jmpbuf;
# endif /* SHARED defined */

/* generated */
# include "proc.h"			/* BEGIN() */
# include "equ.h"			/* for res.h */
# include "data.h"
# include "res.h"			/* BANRCL */

# include "version.h"			/* VERSION, VERSION_DATE */
# include "h.h"				/* IMPORT/EXPORT */

# define SNOBOL4_PROVIDER
# include "snobol4.h"			/* prototypes */

# define GLOBAL_EXTERN
# include "globals.h"

#ifdef BLOCKS
#define SNONAME "CSNOBOL4B"
#else
#define SNONAME "CSNOBOL4"
#endif

const char vers[] = VERSION;
const char vdate[] = VERSION_DATE;
const char snoname[] = SNONAME;

/* returns exit status, or -1 on success!! */
SNOEXP(int)
snobol4_init(int argc, char *argv[], int interactive) {
    init_data();
    init_syntab();
    return init_args( argc, argv, interactive );
}

#ifdef SHARED
static char *ni_argv[] = { "snobol4", "-rb" };
#define NI_ARGC sizeof(ni_argv)/sizeof(ni_argv[0])

EXPORT(int)
snobol4_init_ni(void) {			/* non-interactive init */
    return snobol4_init(NI_ARGC, ni_argv, 0);
}
#endif

SNOEXP(int)
snobol4_run(void) {
# ifdef SHARED
    if (setjmp(endex_jmpbuf))
	return(D_A(RETCOD));
# endif /* SHARED defined */
    BEGIN(NORET);
    /* NOTREACHED */
    return( 0 );
}

#ifdef SHARED
#define main snobol4_main
#endif

SNOEXP(int)
main(int argc, char *argv[]) {
    int ret = snobol4_init(argc, argv, 1);
    if (ret != INIT_OK)
	return ret;

    if( D_A(BANRCL) != 0 ) {
	io_printf(D_A(PUNCH),
"The Macro Implementation of SNOBOL4 in C (%s) Version %s\n", snoname, vers);
	io_printf(D_A(PUNCH), "    by Philip L. Budne, %s\n", vdate);
# ifdef MODIFIED_BANNER
	io_printf(D_A(PUNCH), "%s\n", MODIFIED_BANNER);
# endif /* MODIFIED_BANNER defined */
    }

    return snobol4_run();
}
