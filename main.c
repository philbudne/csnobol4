/* $Id$ */

/* snobol4 main program (make this mlink.c??) */

int
main(argc, argv)
    int argc;
    char *argv[];
{
    init_data();
    init_args( argc, argv );
    version();
    BEGIN( 0 );
    return( 0 );
}
