/* $Id$ */

/* snobol4 main program (make this mlink.c??) */

# ifdef NO_STATIC_VARS
# include "h.h"
# include "snotypes.h"
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
    varp = (struct vars *)malloc(sizeof(struct vars));
    if (!varp) {
	perror("could not allocate vars");
	exit(1);
    }
# endif /* NO_STATIC_VARS defined */
    init_data();
    init_syntab();
    init_args( argc, argv );
    version();
    BEGIN( 0 );
    return( 0 );
}
