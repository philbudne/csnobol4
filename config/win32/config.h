/* $Id$ */

/* Windows32 Visual C (and MINGW) config.h */

#ifndef __STDC__
/*
 * by default __STDC__ is undefined!?
 *	-Za defines __STDC__ to 1, but that causes includes to turn
 *	off useful non-ANSI extensions
 */
#define __STDC__	0
#endif /* __STDC__ not defined */

#define ANSI_STRINGS
/*#define NEED_POPEN_DECL*/
#define SIGFUNC_T	void __cdecl
#define SNOLIB_FILE	"snolib.dll"
#define SNOLIB_DIR	"/snobol"
#define USE_WINSOCK_H
#define USE_STDARG_H

#define finite		_finite
#define isnan		_isnan
#define popen		_popen
#define pclose		_pclose
