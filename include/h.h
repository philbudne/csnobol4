/*
 * $Id$
 *
 * language extensions
 */

#define TRUE	1
#define FALSE	0

#define NULL	0

#ifdef __STDC__
#define STRING(s) #s
#define CONC(a,b) a##b
#else  /* __STDC__ not defined */
#define const
#define STRING(s) "s"
#define CONC(a,b) a/**/b
#endif /* __STDC__ not defined */
/* $Id$ */

