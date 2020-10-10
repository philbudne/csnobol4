/*
 * $Id$
 * tiny main program using libsnobol4.so shared library
 */

extern int snobol4_main(int argc, char *argv[]); /* XXX include snobol4.h? */

int
main(int argc, char *argv[]) {
    return snobol4_main(argc, argv);
}
