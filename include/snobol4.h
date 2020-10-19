/*
 * $Id$
 * API for *EXPERIMENTAL* snobol4 shared library
 */

#ifdef SNOBOL4	/* building snobol4 or libsnobol4.so shared library */
#ifdef SHARED
#define SNOBOL4_API(RETTYPE) EXPORT(RETTYPE)
#else  /* not SHARED (building snobol4.exe) */
#define SNOBOL4_API(RETTYPE) RETTYPE
#endif /* not SHARED (building snobol4.exe) */
#else  /* SNOBOL4 not defined (shared library user) */
#define SNOBOL4_API(RETTYPE) IMPORT(RETTYPE)
#endif /* SNOBOL4 not defined (shared library user) */

/* main.c */
SNOBOL4_API(int) snobol4_main(int argc, char *argv[]);

/* or */

#define INIT_OK -1
SNOBOL4_API(int) snobol4_init(int argc, char *argv[], int interactive);
/* or */
SNOBOL4_API(int) snobol4_init_ni(void);	/* simple, non-interactive */
/* (and calls to attach "files") */

/* then */

SNOBOL4_API(int) snobol4_run(void);

/*
 * calls to set input and output files after calling
 * snobol4_init w/ interactive == 0, or snobol4_init_ni
 */

/* lib/io.c */
/* NOTE!!unit number definitions in unit.h */

/*
 * add file named "fname" to UNITI (INPUT) file list
 */
SNOBOL4_API(void) io_input_file(const char *fname);	  /* named file */

/*
 * add a C string "buf" as an input file to UNITI (INPUT) file list
 * "fname" is for diagnostic output
 */
SNOBOL4_API(void) io_input_string(const char *fname, char *bif); /* string */

/*
 * attach fixed size output buffer "buf" to an I/O unit "unit"
 * UNITO for OUTPUT, UNITP for TERMINAL (former PUNCH variable!)
 * fname argument is for diagnostic output
 */
SNOBOL4_API(int) io_output_string(int unit, const char *fname, char *buf, int);
