/* $Id$ */

/* VMS config.h 1/26/2002 */

#define HAVE_STRING_H

/* other include files defines?? HAVE_xxx_H */

/* use tty_read routines; tty_mode() is a noop */
#define TTY_READ_COOKED
#define TTY_READ_RAW

/* 
 * define preprocessor aliases for SIL and snolib subroutine names
 * which conflict with C runtime names (all names in the C runtime
 * are upper-case.  All our source references to C runtime functions
 * are lower-case, so this just renames our functions to avoid conflict)
 *
 * **NOTE** Any PML function renamed here needs special treatment in pml.h
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
