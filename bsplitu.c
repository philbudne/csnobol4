/* $Id$ */

/* split binary file on stdin, and uuencode pieces! */

#include <stdio.h>

/* approx output size; */
#define OSIZE (64*1024)			/* XXX make an arg */
#define BSIZE ((3*OSIZE)/4)		/* allow for expansion by uuencode */

main( argc, argv )
    int argc;
    char *argv[];
{
    int cc;
    char buffer[BSIZE];			/* XXX malloc */
    int n;

    if (argc < 2) {
	fprintf(stderr, "Usage: bsplit <output prefix>\n" );
	exit(1);
    }

    n = 0;
    while ((cc = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
	FILE *out;
	char cmd[512];

	sprintf( cmd, "uuencode %s.%02d > %s.%02d.uu",
		argv[1], n,
		argv[1], n );
	n++;
	out = popen( cmd, "w");
	if (out == NULL) {
	    perror(cmd);
	    exit(1);
	}
	if (fwrite( buffer, 1, cc, out) != cc) {
	    perror("write error");
	}
	pclose(out);
    }
}

