/*
 * $Id$
 *
 * macros for data access and implementation of SIL ops
 */

/* descriptor at address x */
#define D(x)	(*(struct descr *)(x))

/* access descr fields */
#define D_A(x)	(D(x).a.i)
#define D_F(x)	(D(x).f)
#define D_V(x)	(D(x).v)

#define D_RV(x) (D(x).a.f)

#ifdef USE_MEMCMP
#define bcmp memcmp
#endif /* USE_MEMCMP defined */

#ifdef USE_MEMMOVE
#define bcopy(SRC,DEST,LEN) memmove(DEST,SRC,LEN)
#endif /* USE_MEMMOVE defined */

/* compare two descrs (returns boolean) */
#ifdef DCMP_BYTES
/* this has never been tested! */
/* here if sizeof(float) > sizeof(long) */
#define DCMP(A,B) (bcmp(A, B, DESCR) == 0)
#else  /* DCMP_BYTES not defined */
/* here if sizeof(float) <= sizeof(long) */
#define DCMP(A,B) (D_A(A) == D_A(B) && D_F(A) == D_F(B) && D_V(A) == D_V(B))
#endif /* DCMP_BYTES not defined */

/* clear B+1 descriptor block */
#define ZERBLK(A,B) bzero((void *)(A), (long)((B)+DESCR)) /* XXX SIZE_T */

/*
 * copy descriptor block
 *
 * NOTE: may overlap!!
 * (bcopy deals with this but some memcpy's do not)!!!
 */
/* XXX SIZE_T */
#define MOVBLK(A,B,C) bcopy((void *)((B)+DESCR),(void *)((A)+DESCR),(long)(C) )

/****************
 * string specifiers (qualifiers)
 */

/* specifier at address x */
#define _SPEC(x) (*(struct spec *)(x))

/* access spec fields */
#define S_A(x) (_SPEC(x).a.i)
#define S_F(x) (_SPEC(x).f)
#define S_V(x) (_SPEC(x).v)
#define S_L(x) (_SPEC(x).l.i)
#define S_O(x) (_SPEC(x).o)

#define S_SP(x) ((char *)S_A(x) + S_O(x))

#if 0
#define CLR_S_UNUSED(x) (_SPEC(x).unused = 0)
#else  /* not 0 */
#define CLR_S_UNUSED(x)
#endif /* not 0 */

/*
 * zero is most common case?!
 * one is next most common!?
 * bcopy handles anything else as well as simple loop!!
 *	should NEVER need to check for overlap!
 */
#define APDSP(BASE,STR) if (S_L(STR) > 0) { apdsp(BASE, STR); }

/* must deal with A == C
 * 10/28/93
 */
#define X_REMSP(A,B,C) \
    S_A(A) = S_A(B); \
    S_F(A) = S_F(B); \
    S_V(A) = S_V(B); \
    S_O(A) = S_O(B) + S_L(C); \
    S_L(A) = S_L(B) - S_L(C); \
    CLR_S_UNUSED(A)

#define X_LOCSP(A,B) \
    if (D_A(B) == 0) S_L(A) = 0; \
    else { \
       S_A(A) = D_A(B); S_F(A) = D_F(B); S_V(A) = D_V(B); \
       S_O(A) = BCDFLD; S_L(A) = D_V(D_A(B)); CLR_S_UNUSED(A); \
    }

/* fast compare for equality */
/* XXX SIZE_T */
#define LEXEQ(A,B) (S_L(A) == S_L(B) && \
		    (S_L(A) == 0 || \
		     *S_SP(A) == *S_SP(B) && \
		     (S_L(A) == 1 || \
		      S_SP(A)[1] == S_SP(B)[1] && \
		      (S_L(A) == 2 || \
		      (bcmp(S_SP(A)+2,S_SP(B)+2,(long)S_L(A)-2) == 0)))))

/* 11/4/97 - get length for string structure */
#define X_GETLTH(A) (DESCR * (3 + ((S_L(A) - 1) / CPD + 1)))

/* 11/4/97 - get block size for GC */
#define X_BKSIZE(A) \
    ((D_F(A) & STTL) ? (DESCR*(4+((D_V(A)-1)/CPD+1))) : D_V(A) + DESCR)

/****************
 * system stack
 */

#ifndef NO_STATIC_VARS
#ifndef S4_EXTERN
#define S4_EXTERN extern
#endif

S4_EXTERN struct descr *cstack;
S4_EXTERN struct descr ostack[1];	/* old stack pointer */
#endif

/* RCALL support */
/* by the book, no C local ostack */

#define SAVSTK() START_CALL(); PUSH(ostack); D_A(ostack) = (int_t)cstack
#define RSTSTK() cstack = (struct descr *)D_A(ostack); POP(ostack)

/* overflow check */
#define OFCHK()	\
	{ if ((int_t)cstack > (int_t)(STACK+STSIZE*DESCR)) OVER(NORET); }

#ifdef DO_UFCHK
/* for debug only (internal error); */
#define UFCHK()	{ if ((int_t)cstack < (int_t)STACK) INTR10(NORET); }
#else  /* DO_UFCHK not defined */
#define UFCHK()
#endif /* DO_UFCHK not defined */

#define PUSH(x)	D(cstack+1) = D(x); cstack++; OFCHK()
#define POP(x)	cstack--; UFCHK(); D(x) = D(cstack+1)

#define SPUSH(x) _SPEC(cstack+1) = _SPEC(x); cstack += SPEC/DESCR; OFCHK()
#define SPOP(x)	 cstack -= SPEC/DESCR; UFCHK(); _SPEC(x) = _SPEC(cstack+1)

#define ISTACK() cstack = (struct descr *)STACK
#define PSTACK(x) D_A(x) = (int_t)(cstack-1); D_F(x) = D_V(x) = 0

/****************/

#ifdef PANIC_PUTS
#define PANIC(S) puts(S)
#else  /* PANIC_PUTS not defined */
#define PANIC(S)
#endif /* PANIC_PUTS not defined */

/****************/

#ifndef NO_STATIC_VARS
extern volatile int math_error;
#endif

#define CLR_MATH_ERROR() math_error = FALSE
#define MATH_ERROR() math_error
#define RMATH_ERROR(RESULT) (MATH_ERROR() || !finite((RESULT)))

/****************/

#define ENTRY(NAME) CONC(L_,NAME):

#ifdef TRACE_DEPTH
/* on real call; increment call depth; clear tail call depth for this level */
#define START_CALL() cdepth++; tdepth[cdepth]=0;
/* on tail call; inrement tail calls for this level */
#define BRANCH(NAME) {tdepth[cdepth]++; return (NAME (retval));}
/* on real return; record returns from this tail call depth; decrement level */
#define RETURN(VALUE) {returns[tdepth[cdepth--]]++; RSTSTK(); return (VALUE);}
#else  /* TRACE_DEPTH not defined */
#define START_CALL()
#define BRANCH(NAME) return (NAME (retval));
#define RETURN(VALUE) {RSTSTK(); return (VALUE);}
#endif /* TRACE_DEPTH not defined */
