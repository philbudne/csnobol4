/* $Id$ */

#include "h.h"
#include "types.h"

#include "macros.h"
#include "flags.h"

/*#define DEBUG_TOP*/

void
top( d1, d2, d3 )
    struct descr *d1, *d2, *d3;
{
   int_t a;

   a = D_A(d3);
   while ((D_F(a) & TTL) == 0) {
       a -= DESCR;
   }

#ifdef DEBUG_TOP
   if (D_A(a) != a) {
       printf("*** descr at %#x has TTL flag, a=%#x\n", a, D_A(a));
   }
#endif /* DEBUG_TOP defined */

   D_A(d1) = a;
   D_F(d1) = D_F(d3);
   D_V(d1) = D_V(d3);

   D_A(d2) = D_A(d3) - a;		/* get offset */
   D_F(d2) = 0;
   D_V(d2) = 0;
}
