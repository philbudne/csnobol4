/* $Id$ */

/* header files for snobol4.c; included by SIL "COPY PARMS" macro */

#define S4_EXTERN			/* for cstack */

#include "snotypes.h"
#include "h.h"
#include "libret.h"
#include "syntab.h"
#include "macros.h"
#include "units.h"

/* machine generated */
#include "equ.h"
#include "res.h"
#include "data.h"
#include "syn.h"

struct descr NORET[1];

#ifdef TRACE_DEPTH
extern int cdepth;
extern int tdepth[];
extern int returns[];
#endif /* TRACE_DEPTH defined */

#ifdef INLINING

#ifndef NO_INLINE_PAIR
#define STATIC_PAIR static
#include "lib/pair.c"
#endif /* NO_INLINE_PAIR not defined */

#ifndef NO_INLINE_HASH
#define STATIC_HASH static
#include "lib/hash.c"
#endif /* NO_INLINE_HASH not defined */

#ifndef NO_INLINE_TOP
#define STATIC_TOP static
#include "lib/top.c"
#endif /* NO_INLINE_TOP not defined */

#ifndef NO_INLINE_PAT
#define STATIC_PAT static
#include "lib/pat.c"
#endif /* NO_INLINE_PAT not defined */

#endif /* INLINING defined */
