/* $Id$ */

/* VMS config.h 1/26/2002 */
#define ANSI_STRINGS

#define NEED_POPEN_DECL
#define NO_OFF_T
#define TTY_READ_COOKED
#define TTY_READ_RAW

/* 
 * define preprocessor aliases for SIL and snolib subroutine names
 * which conflict with C runtime names (all are upper-case names)
 */

#define ANY	XANY
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
#define SUBSTR	XSUBSTR
#define TAN	XTAN
#define TIME	XTIME
#define UNLOAD	XUNLOAD
