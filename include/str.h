/* $Id$ */

/* (b)string support */

#ifdef ANSI_STRINGS
#include <string.h>

#ifndef index
#define index strchr
#endif /* index not defined */
#ifndef rindex
#define rindex strrchr
#endif /* rindex not defined */

#ifdef USE_MEMCMP
#define bcmp memcmp
#endif /* USE_MEMCMP defined */

#ifdef USE_OWN_BCOPY
/* use lib/aux/bcopy.c (fast, handles overlap correctly) */
void bcopy __P((const void *, void *, int));
#endif /* USE_OWN_BCOPY defined */

#ifdef USE_MEMMOVE
#define bcopy(SRC,DEST,LEN) memmove(DEST,SRC,LEN)
#endif /* USE_MEMMOVE defined */

#ifdef USE_OWN_BZERO
/* use lib/aux/bzero.c (faster than memset) */
void bzero __P((char *, unsigned int));
#endif /* USE_OWN_BZERO defined */

#else  /* ANSI_STRINGS not defined */

/* there's no place like home! */
#include <strings.h>

#endif /* ANSI_STRINGS not defined */
