/* $Id$ */

/* VMS config.h 1/26/2002 */

#define HAVE_STRING_H

/* use tty_read routines; tty_mode() is a noop */
#define TTY_READ_COOKED
#define TTY_READ_RAW

#ifdef SOCKELEN_INT
#define SOCKLEN_T int
#else  /* SOCKLEN_INT not defined */
#define SOCKLEN_T unsigned int
#endif /* SOCKLEN_INT not defined */

/* have osdep_open() routine */
#define OSDEP_OPEN

/* 
 * define preprocessor aliases for SIL and snolib subroutine names
 * which conflict with C runtime names (all names in the C runtime
 * are upper-case.  All our source references to C runtime functions
 * are lower-case, so this just renames our functions to avoid conflict)
 *
 * **NOTE** Any PML function renamed here needs to use PMLFUNC2 in pml.h
 */

#define ANY	XXANY			/* need XX w/ COMPAQ C 6.5?! */
#define COS	XCOS
#define DATE	XDATE
#define DELETE	XDELETE
#define DIV	XDIV
#define EXIT	XEXIT
#define EXP	XEXP
#define INIT	XINIT
#define IO_FINDUNIT XIO_FINDUNIT
#define LOAD	XLOAD
#define LOG	XLOG
#define RAISE	XRAISE
#define READ	XREAD
#define RENAME	XRENAME
#define REWIND	XREWIND
#define RPLACE	XRPLACE
#define SIN	XSIN
#define SQRT	XSQRT
#define SUBSTR	XXSUBSTR		/* need XX w/ COMPAQ C 6.5?! */
#define TAN	XTAN
#define TIME	XTIME
#define UNLOAD	XUNLOAD

#define PML_NDBM
#define DBM_OPEN XDBM_OPEN
#define DBM_CLOSE XDBM_CLOSE
#define DBM_STORE XDBM_STORE
#define DBM_FETCH XDBM_FETCH
#define DBM_FIRSTKEY XDBM_FIRSTKEY
#define DBM_NEXTKEY XDBM_NEXTKEY
#define DBM_DELETE XDBM_DELETE

#define DIR_SEP ""			/* ASSume brackets or device */
#define PATH_SEP ";"			/* be DOS-ish */

#define SNOLIB_DIR "CSNOBOL4$LIB:"
