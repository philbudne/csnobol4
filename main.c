/* $Id$ */

/* snobol4 main program (make this mlink.c??) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

# ifdef HAVE_STDLIB_H
# include <stdlib.h>			/* for malloc */
# endif /* HAVE_STDLIB_H */

# if defined(ENDEX_LONGJMP) || defined(NO_STATIC_VARS)
# include "h.h"
# include "snotypes.h"
# include "macros.h"

# include "equ.h"
# include "res.h"
# endif /* defined(ENDEX_LONGJMP) || defined(NO_STATIC_VARS) */

# ifdef ENDEX_LONGJMP
# include <setjmp.h>
# ifndef NO_STATIC_VARS
jmp_buf endex_jmpbuf;
# endif /* NO_STATIC_VARS not defined */
# endif /* ENDEX_LONGJMP defined */

# ifdef NO_STATIC_VARS
# include "vars.h"
struct vars *varp;
# undef argc
# undef argv
# endif /* NO_STATIC_VARS defined */


int
main(argc, argv)
    int argc;
    char *argv[];
{
# ifdef NO_STATIC_VARS
# ifdef ENDEX_LONGJMP
    jmp_buf _endex_jmpbuf;
# endif /* ENDEX_LONGJMP defined */
    varp = (struct vars *)malloc(sizeof(struct vars));
    if (!varp) {
	perror("could not allocate vars");
	exit(1);
    }
# ifdef ENDEX_LONGJMP
    varp->v_endex_jmpbuf = _endex_jmpbuf;
# endif /* ENDEX_LONGJMP defined */
# endif /* NO_STATIC_VARS defined */
    init_data();
    init_syntab();
    init_args( argc, argv );
    version();
# ifdef ENDEX_LONGJMP
    if (setjmp(endex_jmpbuf))
	return(D_A(RETCOD));
# endif /* ENDEX_LONGJMP defined */
    BEGIN( 0 );
    return( 0 );
}
