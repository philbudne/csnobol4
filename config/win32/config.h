/* $Id$ */

/*
 * Windows32 config.h 1/27/2002
 *
 * Used for Borland, MINGW, and MSVC
 */

#ifndef __STDC__
/*
 * MSVC: by default __STDC__ is undefined!?
 *	-Za defines __STDC__ to 1, but turns off useful non-ANSI extensions!!
 */
#define __STDC__	0
#endif /* __STDC__ not defined */

/* datatypes; */
#define SIGFUNC_T	void __cdecl

/* paths; */
#define SNOLIB_FILE	"snolib.dll"
#define SNOLIB_DIR	"/snobol4"

/* includes; */
#define ANSI_STRINGS
#define HAVE_STDARG_H
#define HAVE_STDLIB_H

/* DLL import/export macros */

#if defined(_MSC_VER) || defined(__MINGW32__)
#define EXPORT(TYPE) __declspec(dllexport) TYPE
#elif defined(__BORLANDC__)
#define EXPORT(TYPE) TYPE _export
#endif

/* only define IMPORT when building a loadable DLL?? */
#ifdef DLL
#if defined(_MSC_VER) || defined(__MINGW32__)
#define IMPORT(TYPE) __declspec(dllimport) TYPE
#elif defined(__BORLANDC__)
#define IMPORT(TYPE) TYPE _import	/* ??? */
#endif

/* non-standard functions; */
#define finite		_finite
#define isnan		_isnan
#define popen		_popen
#define pclose		_pclose

