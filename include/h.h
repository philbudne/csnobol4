/*
 * $Id$
 *
 * language extensions
 */

#define TRUE	1
#define FALSE	0

#ifndef NULL
#define NULL	0
#endif /* NULL not defined */

#ifdef __STDC__
#define STRING(s) #s
#define CONC(a,b) a##b
#else  /* __STDC__ not defined */
#define STRING(s) "s"
#define CONC(a,b) a/**/b
#endif /* __STDC__ not defined */

/* HP unbundled cc defines __STDC__ as zero; does not implement const! */
#if !defined(__STDC__) || __STDC__ == 0
#define const
#define volatile
#endif /* not ANSI C */

#ifndef __P
#ifdef __STDC__
#define __P(proto) proto
#else  /* __STDC__ not defined */
#define __P(proto) ()
#endif /* __STDC__ not defined */
#endif /* __P not defined */

#ifdef ANSI_STRINGS
#define index(a,b) strchr(a,b)
#define bcmp(a,b,c) memcmp(a,b,c)
/* for bzero() use lib/aux/bzero.c (faster than memset) */
/* for bcopy() use lib/aux/bcopy.c (fast, handles overlap correctly) */
#endif /* ANSI_STRINGS */

/*
 * For Windoze DLL's
 * MSC and Borland want the decl on different sides of the type!!
 */

#ifndef EXPORT
#define EXPORT(TYPE) TYPE
#endif /* EXPORT not defined */

#ifndef IMPORT
#define IMPORT(TYPE) TYPE
#endif /* IMPORT not defined */


