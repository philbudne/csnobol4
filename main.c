/* $Id$ */

/* snobol4 main program */

int
main(argc, argv)
    int argc;
    char *argv[];
{
    init_data();
    init_args( argc, argv );
    BEGIN( 0 );
    return( 0 );
}
