/* $Id$ */

/*
 * plb 6/2/94
 */

/* prototype for external (LOADed) functions */
#ifdef __STDC__
#define LOAD_PROTO struct descr *retval, int nargs, struct descr *args
#else  /* __STDC__ not defined */
#define LOAD_PROTO
#endif /* __STDC__ not defined */

/* macros for loadable user functions;
 *
 * ie;
 * int
 * myfunc(LA_ALIST) LA_DCL
 * {
 * }
 */

#ifdef __STDC__
#define LA_ALIST LOAD_PROTO
#define LA_DCL
#else  /* __STDC__ not defined */
#define LA_ALIST retval, nargs, args
#define LA_DCL struct descr *retval, *args; int nargs;
#endif /* __STDC__ not defined */

/*
 * macros to fetch arguments
 * XXX check nargs?  check datatypes???
 */
#define LA_DESCR(N) (args + (N))	/* pointer to descr for n'th arg */
#define LA_TYPE(N) D_V(LA_DESCR(N))	/* datatype of n'th arg */
#define LA_INT(N) D_A(LA_DESCR(N))	/* n'th arg as integer */
#define LA_REAL(N) D_RV(LA_DESCR(N))	/* n'th arg as real */
#define LA_PTR(N) ((void *)LA_INT(N))	/* n'th arg as pointer */

/* avoid copying raw with getstring() */
#define LA_STR_LEN(N) (LA_PTR(N) ? D_V(LA_PTR(N)) : 0)

/* NOT NUL TERMINATED!!! MUST NOT BE MODIFIED!!! */
#define LA_STR_PTR(N) (LA_PTR(N) ? ((const char *)LA_PTR(N) + BCDFLD) : NULL)

/*
 * macros to return values;
 * NOTE: use of do { .... } while (0) allows user to 
 * place macro invocations anywhere, and to use a ';' after
 */

#define RETINT(x) \
    do { D_A(retval) = (x); RETTYPE = I; return TRUE; } while (0)

#define RETREAL(x) \
    do { D_RV(retval) = (x); RETTYPE = R; return TRUE; } while (0)

/* strings */
#define RETSTR2(CP,LEN) \
    do { retstring(retval, (CP), (LEN)); return TRUE; } while(0)

#define RETNULL do { D_A(retval) = 0; RETTYPE = S; return TRUE; } while (0)

/*
 * improved!! old version now called RETSTR2().
 * NOTE: evaluates argument once, so it can be a function call.
 * handles NULL pointer (returns NULL string)
 *	have an alternate version that returns failure for NULL??
 */
#define RETSTR(CP) \
    do { \
	const char *cp = (CP); \
        if (cp == NULL) RETNULL; \
	RETSTR2(cp, strlen(cp)); \
    } while (0)

/* return failure */
#define RETFAIL return FALSE

/* access return value type */
#define RETTYPE D_V(retval)

/* extern/prototypes for functions; */
#ifndef __P
#ifdef __STDC__
#define __P(X) X
#else  /* __STDC__ not defined */
#define __P(X) ()
#endif /* __STDC__ not defined */
#endif /* __P not defined */

/* lib/snolib/getstring.c; */
void getstring __P((const void *, char *, int));

/* lib/snolib/retstring.c; */
void retstring __P((struct descr *retval, const char *cp, int len));

/* lib/io.c; */
int io_findunit __P((void));

#ifdef EOF				/* stdio included */
FILE *io_getfp __P((int));
#endif /* NULL defined */

