/* $Id$ */

#ifndef STATIC_TOP
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"

#define STATIC_TOP
#endif /* STATIC_TOP not defined */

/*# define DEBUG_TOP*/

STATIC_TOP void
top(struct descr *d1, struct descr *d2, struct descr *d3) {
   int_t a;

   a = D_A(d3);
   while ((D_F(a) & TTL) == 0) {
       a -= DESCR;
   }

#ifdef DEBUG_TOP
   if (D_A(a) != a) {
       /* NOTE! this will trigger after GC has relocated titles
	* but not data (relocation pass uses TOP to determine
	* offsets in pointer relocation pass)!
	*/
       printf("*** descr at %# x has TTL flag, a=%# x\n", a, D_A(a));
   }
#endif /* DEBUG_TOP defined */

   D_A(d1) = a;
   D_F(d1) = D_F(d3);
   D_V(d1) = D_V(d3);

   D_A(d2) = D_A(d3) - a;		/* get offset */
   D_F(d2) = 0;
   D_V(d2) = 0;
} /* top */
