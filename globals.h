/* $Id$ */

#ifndef GLOBAL_EXTERN
#define GLOBAL_EXTERN extern
#endif

/*
 * NOTE! VAR after storage class (extern) but before type!
 * (C99 rules for TLS)
 */

/* set by init.c, global for access by io.c; */
GLOBAL_EXTERN VAR int rflag;
GLOBAL_EXTERN VAR int lflag;
GLOBAL_EXTERN VAR int unbuffer_all;

/* set by init.c, global for access by host.c; */
GLOBAL_EXTERN VAR size_t ndynamic;
GLOBAL_EXTERN VAR size_t pmstack;
GLOBAL_EXTERN VAR size_t istack;
GLOBAL_EXTERN VAR char *params;
GLOBAL_EXTERN VAR char **argv;
GLOBAL_EXTERN VAR int firstarg;
GLOBAL_EXTERN VAR int argc;
GLOBAL_EXTERN VAR int nfiles;
GLOBAL_EXTERN VAR const char *snolib_base; /* BASE */
GLOBAL_EXTERN VAR char *snolib_local;	/* BASE/local */
GLOBAL_EXTERN VAR char *snolib_vers;	/* BASE/VERSION */
GLOBAL_EXTERN VAR char *snolib_vlib;	/* BASE/VERSION/lib */
GLOBAL_EXTERN VAR char *snolib_vlocal;	/* BASE/VERSION/local */

/* from main.c, for access by host.c; */
extern const char snoname[], vers[], vdate[];

/* from build.c, for access by host.c; */
#ifdef HAVE_BUILD_VARS
extern const char build_files[];
extern const char build_lib[];
extern const char build_date[];
extern const char build_dir[];
#endif /* HAVE_BUILD_VARS defined */
