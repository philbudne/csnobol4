/* $Id$ */

/* NOTE! int_t and real_t should be the same size!!! */

#ifdef INT_T
typedef INT_T int_t; 			/* used to hold integers, addrs */
#else  /* INT_T not defined */
typedef long int_t; 			/* used to hold integers, addrs */
#endif /* INT_T not defined */

#ifdef REAL_T
typedef REAL_T real_t;
#else  /* REAL_T not defined */
typedef float real_t;
#endif /* REAL_T not defined */

union addr {
    int_t i;	
    real_t f;
    void *ptr;				/* not (yet) used (except by gdb) */
};

/* FFLD/VFLD ensure consistant sizing between descr and spec; */

#ifndef FFLD_T
#define FFLD_T char
#endif /* FFLD_T not defined */

#define FFLD(name) FFLD_T name

/*  flags  */
#define FNC	01
#define TTL	02
#define STTL	04
#define MARK	010
#define PTR	020
#define FRZN	040			/* [PLB34] table frozen */
/* only one bit left! */

#ifdef NO_BITFIELDS
#ifndef VFLD_T
#define VFLD_T int			/* at least 31 bits */
#endif
#define VFLD(name) VFLD_T name
#define SIZLIM 0x7fffffff		/* maximum object size (in addrs) */
#else  /* NO_BITFIELDS not defined */
#define VFLD(name) unsigned name : 24
#define SIZLIM 0xffffff
#endif /* NO_BITFIELDS not defined */

/*
 * maximum object sizes for various v-field sizes;
 *
 *			max object size
 * v field	DESCR	bytes	DESCRs
 * 16b		8B	64K	8K
 * 24b		8B	16M	2M
 * 31b		12B	2G	170M
 *
 * max object size in bytes determines maximum string length
 * max object size in DESCRs determines maximum size for;
 *	arrays (one descr per entry)
 *	initial table size (two descrs / entry)
 *	number of identifiers (two descrs / id)
 *	number of functions (two descrs / function)
 *	pattern size (four descrs / node)
 */

/* addressing unit is "char" */
#define CPA	1			/* chars per addr (for BUFLEN) */

/* chars per descr (for BKSIZE,GETLTH) */
#define CPD	DESCR

/*
 * descriptor; the basic SIL data type
 */

struct descr {
    union addr a;			/* address (new: v) */
    FFLD(f);				/* flag (new: f) */
    VFLD(v);				/* value (new: t) data-type/size */
};

#define DESCR (sizeof(struct descr))

/*
 * string specifier (qualifier)
 *
 * NOTE!! A specifier is made up of two adjacent decsriptors,
 * thus the "unused" field, and the use of "union addr" for "l"
 */

struct spec {				/* (new: qualifier) */
    union addr l;			/* length (overlays addr) */
    FFLD(unused);			/* MBZ (must overlay flags) */
    VFLD(o);				/* offset */
    union addr a;			/* address (new: v) */
    FFLD(f);				/* flags */
    VFLD(v);				/* value (new: t) */
};
#define SPEC (sizeof(struct spec))

/* for generated code which deals with function pointers */
typedef int (*func_t)();
