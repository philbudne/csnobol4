/* $Id$ */

typedef long int_t; 			/* used to hold integers, addrs */
typedef float real_t;

union addr {
    int_t i;	
    real_t f;
    void *ptr;				/* not (yet) used (except by gdb) */
};

/* FFLD/VFLD ensure consistant sizing between descr and spec; */

# define FFLD(name) char name

/*  flags  */
# define FNC	01
# define TTL	02
# define STTL	04
# define MARK	010
# define PTR	020

/*
 * NOTE!! could make v a 24-bit bitfield, like on the '360
 * but could slow things down!!
 */

# if 1
# define VFLD(name) long name		/* at least 31 bits */
# define SIZLIM 0x7fffffff		/* maximum object size (in addrs) */
# else  /* not 1 */
# define VFLD(name) unsigned name : 24
# define SIZLIM 0xffffff
# endif /* not 1 */

/*
 * maximum object sizes for various v-field sizes;
 *
 * with byte addressing (CPA == 1)
 *			max object size
 * v field	DESCR	bytes	DESCRs
 * 16b		8B	64K	8K
 * 24b		8B	16M	2M
 * 31b		12B	2G	170M
 *
 * with DESCR addressing (CPA == DESCR)
 *			max object size
 * v field	DESCR	bytes	DESCRs
 * 16b		8B	512K	64K
 * 24b		8B	16M	128KB
 * 31b		12B	24G	2G
 *
 * max object size in bytes determines maximum string size
 * max object size in DESCRs determines maximum size for;
 *	arrays (one descr per entry)
 *	initial table size (two descrs / entry)
 *	number of identifiers (two descrs / id)
 *	number of functions (two descrs / function)
 *	pattern size (four descrs / node)
 */

/* addressing unit is "char" */
# define CPA	1			/* chars per addr */
# define CPD	DESCR			/* chars per descr */

struct descr {
    union addr a;			/* address (new: v) */
    FFLD(f);				/* flag (new: f) */
    VFLD(v);				/* value (new: t) data-type/size */
};

# define DESCR (sizeof(struct descr))

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
# define SPEC (sizeof(struct spec))

/* for generated code which deals with function pointers */
typedef int (*func_t)();
