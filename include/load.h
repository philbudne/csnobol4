/* $Id$ */

/*
 * plb 6/2/94
 */

/* prototype for external (LOADed) functions */
#define LOAD_PROTO struct descr *retval, int nargs, struct descr *args

/* macros for loadable user functions;
 *
 * ie;
 * lret_t
 * myfunc(LA_ALIST) {
 *   RETFAIL;
 * }
 */

#define lret_t EXPORT(int)

#define LA_ALIST LOAD_PROTO
#define LA_DCL				/* K&R artifact */

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
/* used to use "const char *" but turned it off for NDBM on VMS */
#define LA_STR_PTR(N) (LA_PTR(N) ? ((char *)LA_PTR(N) + BCDFLD) : NULL)

/*
 * macros to return values;
 * NOTE: use of do { .... } while (0) allows user to 
 * place macro invocations anywhere, and to use a ';' after
 */

#define SETRETVAL(VAL, TYPE) \
    do { \
	VAL; \
	D_F(retval) = 0; \
	RETTYPE = (TYPE); \
	return TRUE; \
    } while (0)

#define RETINT(x) SETRETVAL(D_A(retval) = (x), I)
#define RETREAL(x) SETRETVAL(D_RV(retval) = (x), R)
#define RETNULL SETRETVAL(D_A(retval) = 0, S)

/*
 * return a string from loaded function.
 * need not be a NUL terminated C-string.
 * buffer will be malloc'ed by retstring, and freed by relstring
 */
#define RETSTR2(CP,LEN) do { \
	retstring(retval, CP, LEN); \
	return TRUE;		\
    } while (0)

/*
 * return a counted (possibly non-NUL terminated) string in malloc'ed
 * memory.  Memory will be freed by call to relstring.
 */
#define RETSTR2_FREE(CP,LEN) do { \
	retstring_free(retval, CP, LEN); \
	return TRUE;		\
    } while (0)

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

/*
 * return (& free) a malloc'ed C-string.
 * As of v2.2, returns pointer to SNOBOL as type 'M' (malloc'ed linked string)
 * After string "interned" (variable generated), relstring is called.
 * (used by ffi & readline).  Binaries built with this .h file are not
 * backwards compatible (returned string will appear as EXTERNAL,
 * memory will not be freed).
 */
#define RETSTR_FREE(CP) \
    do { \
	char *cp = (CP); \
        if (cp == NULL) RETNULL; \
	retstring_free(retval, (cp), strlen(cp)); \
	return TRUE; \
    } while (0)

/* return failure */
#define RETFAIL return FALSE

/* access return value type */
#define RETTYPE D_V(retval)

/* return predicate value */
#define RETPRED(SUCC) \
    do { \
	if (SUCC) RETNULL; \
	else RETFAIL; \
    } while (0)

#if defined(DYNAMIC) || defined(DLL) /* building dynamically loadable module */
#define SNOEXP(X) IMPORT(X)
#else			/* building snobol4 executable (or shared library) */
#define SNOEXP(X) EXPORT(X)
#endif

/* extern/prototypes for functions; */
/* lib/snolib/getstring.c; */
SNOEXP(void) getstring(const void *, char *, int);
SNOEXP(char *) mgetstring(const void *);

/* lib/snolib/retstring.c; */
SNOEXP(void) retstring(struct descr *retval, const char *cp, int len);
SNOEXP(void) retstring_free(struct descr *retval, const char *cp, int len);

/* lib/io.c; */
SNOEXP(int) io_findunit(void);		/* find a free (external) unit */
SNOEXP(int) io_closeall(int iunit);	/* **INTERNAL** (zero-based unit) */
SNOEXP(int) io_attached(int xunit);	/* boolean */
EXPORT(char *) io_fname(int xunit);
EXPORT(int) io_skip(int xunit);

#ifdef EOF				/* stdio included */
SNOEXP(int) io_mkfile(int xunit, FILE *, char*); /* external (1-based unit) */
SNOEXP(int) io_mkfile_noclose(int xunit, FILE *, char *name);
/* temporarily(?) unavailable in 2.2: */
SNOEXP(FILE *) io_getfp(int xunit);	/* external (1-based unit) */
#endif /* EOF defined */
