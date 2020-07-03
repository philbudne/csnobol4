/* $Id$ */

/* header files for snobol4.c; included by SIL "COPY PARMS" macro */
#define S4_EXTERN			/* for cstack */

#include <stdio.h>			/* for lib.h */
#include <math.h>			/* finite(), isnan() */

#ifdef HAVE_CONFIG_H
/* after math.h; isnan defined in VS 2019 math.h */
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "snotypes.h"
#include "h.h"
#include "syntab.h"
#include "libret.h"
#include "macros.h"
#include "units.h"
#include "str.h"

/* machine generated */
#include "equ.h"
#include "res.h"
#include "data.h"
#include "syn.h"
#include "proc.h"			/* global procs */
#include "static.h"			/* static procs */

#ifdef NO_STATIC_VARS
#include "vars.h"
#endif /* NO_STATIC_VARS defined */

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

#include "lib.h"			/* after all STATIC_xxx defines */
