/* $Id$ */

/* (b)string support */

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H defined */

#ifdef HAVE_STRING_H
#include <string.h>

#ifndef HAVE_STRINGS_H
#ifndef index
#define index strchr
#endif /* index not defined */

#ifndef rindex
#define rindex strrchr
#endif /* rindex not defined */

/* ifdef needed? better safe than sorry */
#ifndef bcmp
#define bcmp memcmp
#endif /* bcmp not defined */

#ifdef NEED_BCOPY
/* use lib/aux/bcopy.c (fast, handles overlap correctly) */
void bcopy __P((const void *, void *, int));
#endif /* NEED_BCOPY defined */

#ifdef USE_MEMMOVE
#define bcopy(SRC,DEST,LEN) memmove(DEST,SRC,LEN)
#endif /* USE_MEMMOVE defined */

#ifdef USE_MEMSET
#define bzero(A,B) memset(A,0,B)
#endif

#ifdef NEED_BZERO
/* use lib/aux/bzero.c (faster than memset) */
void bzero __P((char *, unsigned int));
#endif /* NEED_BZERO defined */
#endif /* HAVE_STRINGS_H not defined */

#endif /* HAVE_STRING_H defined */
