/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifndef STATIC_HASH
#include <stdio.h>
#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "lib.h"			/* own prototype */

/* machine generated; */
#include "equ.h"			/* for OBSIZ */

#define STATIC_HASH
#endif /* STATIC_HASH not defined */

/*
 * New hash function by Bob Jenkins (published in September 1997 Dr Dobbs)
 * see http://burtleburtle.net/bob/hash/
 *
 * This is more expensive than the the simple sum of the first and
 * last four characters I had been using.  LOCA1 (variable lookup) is
 * a hot-spot, and expanding hash table size with the old sum was
 * pointless, since the range of hash values was small.
 *
 * The new function purports to deliver enough good bits to expand the
 * hash table size, it should also have enough bits left over to use
 * for "assention" (I had been using length, which penalized long
 * strings).
 *
 * Only look at the first 12 (if len >= 12), and last 11.  MAINBOL
 * hashes ALL strings, so the inputs can be long, and you can waste a
 * lot of your time hashing them...
 *
 * It's not that I'm convinced this is the best hash in the universe,
 * but he's clearly spent more time investigating this than I want to
 * at the moment, and he takes into consideration factors like fast
 * execution on non-intel architectures, so I'm inclined to trust
 * him...
 *
 * -phil 1/17/2002
 */

/*
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bits set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
  have at least 1/4 probability of changing.
* If mix() is run forward, every bit of c will change between 1/3 and
  2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a
  structure that could supported 2x parallelism, like so:
      a -= b;
      a -= c; x = (c>>13);
      b -= c; a ^= x;
      b -= a; x = (a<<8);
      c -= a; b ^= x;
      c -= b; x = (b>>13);
      ...
  Unfortunately, superscalar Pentiums and Sparcs can't take advantage
  of that parallelism.  They've also turned some of those single-cycle
  latency instructions into multi-cycle latency instructions.  Still,
  this is the fastest good hash I could find.  There were about 2^^68
  to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/

#define MIX(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8);  \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12); \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5);  \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

typedef unsigned int ub4;

STATIC_HASH void
hash(dp, sp)
    struct descr *dp;
    struct spec *sp;
{
    unsigned char *cp;
    int length, len;
    ub4 a, b, c;

    length = S_L(sp);
    cp = (unsigned char *)S_SP(sp);

    /* Set up the internal state */
    len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = 0x1234abcd;				/* XXX */

    if (len >= 12) {
	a += (cp[0] +((ub4)cp[1]<<8) +((ub4)cp[2]<<16) +((ub4)cp[3]<<24));
	b += (cp[4] +((ub4)cp[5]<<8) +((ub4)cp[6]<<16) +((ub4)cp[7]<<24));
	c += (cp[8] +((ub4)cp[9]<<8) +((ub4)cp[10]<<16)+((ub4)cp[11]<<24));
	MIX(a,b,c);
	cp += 12; len -= 12;
    }

    /* handle last 11 */
    if (len > 11) {
	cp += (len - 11);
	len = 11;
    }

    c += length;
    switch (len) {
    case 11: c+=((ub4)cp[10]<<24);
    case 10: c+=((ub4)cp[9]<<16);
    case 9:  c+=((ub4)cp[8]<<8);
	/* the first byte of c is reserved for the length */
    case 8:  b+=((ub4)cp[7]<<24);
    case 7:  b+=((ub4)cp[6]<<16);
    case 6:  b+=((ub4)cp[5]<<8);
    case 5:  b+=cp[4];
    case 4:  a+=((ub4)cp[3]<<24);
    case 3:  a+=((ub4)cp[2]<<16);
    case 2:  a+=((ub4)cp[1]<<8);
    case 1:  a+=cp[0];
    /* case 0: nothing left to add */
    }
    MIX(a,b,c);

#ifdef OBSFT
    /* OBSIZ a power of two; use mask & shift */

    /*
     * Sun "WorkShop 6 2000/04/07 C 5.1" in LP64 mode (-xarch=v9) w/ -O
     * blows this as a single statement
     */
    D_A(dp) = c;
    D_A(dp) &= (OBSIZ-1);
    D_V(dp) = c >> OBSFT;		/* assention */
#else  /* OBSFT not defined */
    /* OBSIZ not a power of two; use mod and div */
    D_A(dp) = c % OBSIZ;
    D_V(dp) = c / OBSIZ;		/* assention */
#endif /* OBSFT not defined */
    D_A(dp) *= DESCR;			/* make table offset */
}
