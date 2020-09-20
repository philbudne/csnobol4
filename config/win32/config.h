/* $Id$ */

/*
 * Windows32 config.h 1/27/2002
 * Windows64 too! 9/23/2013
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

#define BLOCKS

/* datatypes; */
#define SIGFUNC_T	void __cdecl
#define SOCKLEN_T	int
#define SETSOCKOPT_ARG_CAST (const char *)

/* paths; */
#define SNOLIB_FILE	"snolib.dll"
#define SNOLIB_BASE	"C:\\snobol4"
#define SHARED_OBJ_SUBDIR "shared"
#define DIR_SEP		"\\"
#define PATH_SEP	";"
#define DL_EXT		".dll"

/* includes; */
#define HAVE_STRING_H
#define HAVE_STDARG_H
#define HAVE_STDLIB_H
#define HAVE_SDBM_H
#define HAVE_IO_H		/* _read,.... */

#define WIN32_LEAN_AND_MEAN

#define UNLINK_IN_STDIO_H
#define OSDEP_OPEN
#define TTY_READ_RAW
#define HAVE_GETVERSIONEX
#define GETPID_IN_PROCESS_H

#ifdef HAVE_WINSOCK2_H
#define NEED_BINDRESVPORT_SA
#else
#define NEED_BINDRESVPORT
#endif

/* DLL import/export macros */

#if defined(_MSC_VER) || defined(__MINGW32__)
#define EXPORT(TYPE) __declspec(dllexport) TYPE
#elif defined(__BORLANDC__)
#define EXPORT(TYPE) TYPE _export
#endif /* defined(__BORLANDC__) */

/*
 * IMPORT (external) symbols when building a DLL
 * load.h defines SNOEXP(X) as IMPORT(X) when DLL defined
 */
#ifdef DLL
#if defined(_MSC_VER) || defined(__MINGW32__)
#define IMPORT(TYPE) __declspec(dllimport) TYPE
#elif defined(__BORLANDC__)
#define IMPORT(TYPE) TYPE _import	/* ??? */
#endif /* defined(__BORLANDC__) */
#endif /* DLL defined */

/* non-standard functions; */
#define finite		_finite
//#define isnan		_isnan		/* 2020: not needed w/ VSC or MINGW */
#define popen		_popen
#define pclose		_pclose
#define dup		_dup
#define read		_read
#define write		_write

/*
 * C90 & POSIX.1-2001:
 */
#define HAVE_FSEEKO			/* now we do! */
#define io_off_t __int64
#define ftello(FP) _ftelli64(FP)
#define fseeko(FP,OFF,WHENCE) _fseeki64(FP,OFF,WHENCE)

/****
 * for time module
 */
#define HAVE_SLEEP

/*
 * in MINGW 1.0.19 time.h, which says:
 * "introduced in MSVCR80.DLL, and they subsequently
 *  appeared in MSVCRT.DLL, from Windows-Vista onwards."
 */
#define HAVE_TIMEGM			
#define timegm		_mkgmtime

/*
 * end for time module
 ****/

#ifdef _WIN64
/*
 * 64-bit binary cross-compiled with VS2012 worked (the first time!)
 * It looks like it's an "LLP64" evironment:
 *	".... a 32-bit model with 64-bit addresses ...."
 *	http://www.unix.org/version2/whatsnew/lp64_wp.html
 */
#define NO_BITFIELDS
#define SIZLIM (~(VFLD_T)0)
#define INT_T long long
#define REAL_T double
#define REALST_FORMAT "%.15lg"
#define SIZEOF_INT_T 8
#define SIZEOF_REAL_T 8
#else
// 32-bits:
#define SIZEOF_INT_T 4
#define SIZEOF_REAL_T 4
#endif

/* INT_T should always be large enough to hold a pointer */
#define ssize_t INT_T

#define sock_t unsigned INT_T // SOCKET: unsigned that can hold a pointer
#define close_socket	closesocket

/* use native routines */
#define USE_MEMMOVE
#define USE_MEMSET

#if defined(_MSC_VER)			/* *** Microsoft C */

/* ignored if NO_BITFIELDS defined (64-bits) */
#define BITFIELDS_SAME_TYPE		/* or else won't pack them! */

#define OBJECT_EXT ".obj"
#define SETUP_SYS "win.msc"
/* from ntmsvc.mak: CC, COPT, SO_LD, DL_LD, DL_CFLAGS */
#define DL_LDFLAGS "/DLL /NOLOGO"
#define CC_IS "msc"

#elif defined(__GNUC__)			/* *** MINGW */

#define OBJECT_EXT ".o"
#define USE_WCHAR_H	  /* for com.cpp */
#define SETUP_SYS "posix" /* !!! */
/* from mingw.mak: CC, COPT, SO_LD, DL_LD */
#define DL_CFLAGS ""
#define DL_LDFLAGS "-shared -shared-libgcc"
#define CC_IS "gcc"

#elif defined(__BORLANDC__)		/* *** Borland */

#define OBJECT_EXT ".obj"
#define SETUP_SYS "win.borland"
#define CCOMPILER "bcc32"
#define DL_LDFLAGS "-tWD"
#define CC_IS "borland"

#endif /* defined(__BORLANDC__) */

#define SO_CFLAGS DL_CFLAGS
#define SO_DLFLAGS DL_DLFLAGS
