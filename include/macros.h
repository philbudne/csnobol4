/*
 * $Id$
 *
 * macros for data access and implementation of SIL ops
 * contains some system dependancies (bcmp, bcopy, bzero)
 */

/* descriptor at address x */
#define D(x)	(*(struct descr *)(x))

/* access descr fields */
#define D_A(x)	(D(x).a.i)
#define D_F(x)	(D(x).f)
#define D_V(x)	(D(x).v)

#define D_RV(x) (D(x).a.f)

/* fetch current value for printf */
#define D_XXX(x) D_A(x)			/* XXX loses for reals!! */

/* compare two descrs (returns boolean) */
#ifdef DCMP_BYTES
/* here if sizeof(float) > sizeof(long) */
#define DCMP(A,B) (bcmp(A, B, DESCR) == 0)
#else  /* DCMP_BYTES not defined */
/* here if sizeof(float) <= sizeof(long) */
#define DCMP(A,B) (D_A(A) == D_A(B) && D_F(A) == D_F(B) && D_V(A) == D_V(B))
#endif /* DCMP_BYTES not defined */

/* clear B+1 descriptor block */
#define ZERBLK(A,B) bzero(A, (B)+DESCR)

/*
 * copy descriptor block
 *
 * NOTE: may overlap!!
 * (bcopy deals with this but some memcpy's do not)!!!
 */
#define MOVBLK(A,B,C) bcopy( (B)+DESCR, (A)+DESCR, (C) )

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

#define APDSP(A,B) bcopy(S_SP(B), S_SP(A)+S_L(A), S_L(B)); S_L(A) += S_L(B)
#define FSHRTN(A,B) S_L(A) -= B; S_O(A) += B
#define SHORTN(A,B) S_L(A) -= B

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

/* fast compare for equality.  check first char before calling bcmp?? */
#define LEXEQ(A,B) (S_L(A) == S_L(B) && bcmp(S_SP(A),S_SP(B),S_L(A)) == 0)

/****************
 * system stack
 */

struct descr *cstack;


/* for RCALL
 *
 * GC expects RCALL to push SOMETHING on each RCALL!
 * keep OSTACK/CSTACK by the book?
 *	ie; make cstack a descr
 *	SAVSTK() -> PUSH(&cstack)
 *	RSTSTK() -> POP(&cstack)
 */
#define SAVSTK() struct descr *ostack = cstack; PUSH(ZEROCL);
#define RSTSTK() cstack = ostack

/* overflow check */
#define OFCHK()	{ if ((int)cstack > (int)STACK+STSIZE*DESCR) OVER(NULL); }

#ifdef DO_UFCHK
#define UFCHK()
#else  /* DO_UFCHK not defined */
/* for debug only (internal error); */
#define UFCHK()	{ if ((int)cstack < (int)STACK) INTR10(NULL); }
#endif /* DO_UFCHK not defined */

#define PUSH(x)	D(cstack+1) = D(x); cstack++; OFCHK()
#define POP(x)	cstack--; UFCHK(); D(x) = D(cstack+1)

#define SPUSH(x) _SPEC(cstack+1) = _SPEC(x); cstack += SPEC/DESCR; OFCHK()
#define SPOP(x)	 cstack -= SPEC/DESCR; UFCHK(); _SPEC(x) = _SPEC(cstack+1)

#define ISTACK() cstack = (struct descr *)STACK
#define PSTACK(x) D_A(x) = (int)(cstack-1); D_F(x) = D_V(x) = 0

/****************/

#define PANIC(S)

/****************/

/* XXX implement these!! */
#define CLR_MATH_ERROR()
#define MATH_ERROR() 0
