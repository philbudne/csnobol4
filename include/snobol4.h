/*
 * $Id$
 * Interface to experimental snobol4 shared library
 */

#ifdef SHARED	/* building shared library */
#define SNOEXP(X) EXPORT(X)
#else		/* shared library user */
#define SNOEXP(X) IMPORT(X)
#endif

/* main.c */
SNOEXP(int) snobol4_main(int argc, char *argv[]);
/* or */
#define INIT_OK -1
SNOEXP(int) snobol4_init(int argc, char *argv[], int interactive);
SNOEXP(int) snobol4_init_ni(void);	/* non-interactive */
SNOEXP(int) snobol4_run(void);

/* lib/io.c */
SNOEXP(int) io_closeall(int);
SNOEXP(int) io_skip(int);
SNOEXP(void) io_input_file(char *);
SNOEXP(void) io_input_string(char *name, char *);
SNOEXP(int) io_mkfile(int, FILE *, char *);
SNOEXP(int) io_mkfile_noclose(int, FILE *, char *);
SNOEXP(int) io_attached(int);
SNOEXP(int) io_output_string(int unit, char *name, char *buf, int);
SNOEXP(char *)io_fname(int);
SNOEXP(int) io_findunit(void);
SNOEXP(FILE *) io_getfp(int);
