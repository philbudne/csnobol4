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
#define const
#define STRING(s) "s"
#define CONC(a,b) a/**/b
#endif /* __STDC__ not defined */

#ifdef ANSI_STRINGS
#define index(a,b) strchr(a,b)
#define bcmp(a,b,c) memcmp(a,b,c)
/* for bzero() use lib/aux/bzero.c (faster than memset) */
/* for bcopy() use lib/aux/bcopy.c (fast, handles overlap correctly) */
#endif /* ANSI_STRINGS */
