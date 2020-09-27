/* $Id$ */

/* (b)string support */

#include <string.h>

#define bzero(A,B) memset(A,0,B)

#if defined(HAVE_BCOPY)
#include <strings.h>
#define memmove(DEST,SRC,LEN) bcopy(SRC,DEST,LEN) 
#elif defined(NEED_BCOPY)
/* use lib/aux/bcopy.c (fast, handles overlap correctly) */
void bcopy(const void *, void *, int);
#define memmove(DEST,SRC,LEN) bcopy(SRC,DEST,LEN) 
#elif defined(USE_MEMMOVE)
#define bcopy(SRC, DEST, LEN) memmove(DEST, SRC, LEN)
#endif /* USE_MEMMOVE defined */
