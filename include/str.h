/* $Id$ */

/* (b)string support */

#include <string.h>

/*
 * 2020-09-27
 * MOST places still use bcopy/bzero
 */

#if defined(HAVE_BCOPY)

#include <strings.h>

#ifndef memmove
#define memmove(DEST,SRC,LEN) bcopy(SRC,DEST,LEN) 
#endif
#ifndef memset
#define memset(PTR,X,LEN) bzero(PTR, LEN) /* XXX assert X==0?!!! */
#endif

#elif defined(NEED_BCOPY)

/* use lib/aux/bcopy.c (fast, handles overlap correctly) */
void bcopy(const void *, void *, int);
void bzero(void *, int);
#ifndef memmove
#define memmove(DEST,SRC,LEN) bcopy(SRC,DEST,LEN) 
#endif
#ifndef memset
#define memset(PTR,X,LEN) bzero(PTR, LEN) /* XXX assert X==0?!!! */
#endif

#elif defined(USE_MEMMOVE)

#ifndef bcopy
#define bcopy(SRC, DEST, LEN) memmove(DEST, SRC, LEN)
#endif

#ifndef bzero
#define bzero(A,B) memset(A,0,B)
#endif

#endif /* USE_MEMMOVE defined */
