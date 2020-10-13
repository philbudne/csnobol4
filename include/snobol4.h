/*
 * $Id$
 * Interface to *EXPERIMENTAL* snobol4 shared library
 */

/* SHAREDEXP??? XXX define as noop if building monolithic snobol4? */
#ifdef SNOBOL4_PROVIDER	/* building snobol4 or libsnobol4.so shared library */
#define SNOEXP(X) EXPORT(X)
#else		/* shared library user */
#define SNOEXP(X) IMPORT(X)
#endif

/* main.c */
SNOEXP(int) snobol4_main(int argc, char *argv[]);
/* or */
#define INIT_OK -1
SNOEXP(int) snobol4_init(int argc, char *argv[], int interactive);
/* or */
SNOEXP(int) snobol4_init_ni(void);	/* non-interactive */
/* then */
SNOEXP(int) snobol4_run(void);

/* lib/io.c */
/* unit number definitions in unit.h */
/* add files to UNITI (INPUT) file list */
SNOEXP(void) io_input_file(char *);	/* named file */
SNOEXP(void) io_input_string(char *name, char *); /* string */

/* attach output buffer to unit UNITO for OUTPUT, UNITP for TERMINAL */
SNOEXP(int) io_output_string(int unit, char *name, char *buf, int);
