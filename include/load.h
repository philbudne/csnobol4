/* $Id$ */

/*
 * plb 6/2/94
 */

/* prototype for external (LOADed) functions */
#ifdef __STDC__
#define LOAD_PROTO struct descr *retval, int nargs, struct descr *args
#else
#define LOAD_PROTO
#endif

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
#else
#define LA_ALIST retval, nargs, args
#define LA_DCL struct descr *retval, *args; int nargs;
#endif

/* macros to fetch arguments
 * XXX check nargs?  check datatypes???
 */
#define LA_DESCR(N) (args + (N))	/* pointer to descr for n'th arg */
#define LA_TYPE(N) D_V(LA_DESCR(N))	/* datatype of n'th arg */
#define LA_INT(N) D_A(LA_DESCR(N))	/* n'th arg as integer */
#define LA_REAL(N) D_RV(LA_DESCR(N))	/* n'th arg as real */
#define LA_PTR(N) ((void *)LA_INT(N))	/* n'th arg as pointer */

/* macros to return values; */
#define RETINT(x) { D_A(retval) = (x); return TRUE; }
#define RETREAL(x) { D_RV(retval) = (x); return TRUE; }
/* strings */
#define RETSTR(CP,LEN) { retstring(retval, (CP), (LEN)); return TRUE; }
#define RETCSTR(CP) \
	{ char *cp = (CP); retstring(retval, cp, strlen(cp)); return TRUE; }
#define RETNULL { D_A(retval) = 0; return TRUE; }

/* return failure */
#define RETFAIL return FALSE

/* access return value type */
#define RETTYPE D_V(retval)
