typedef long int_t; 			/* used to hold integers, addrs */
typedef float real_t;

union addr {
    int_t i;	
    real_t f;
    void *ptr;				/* not (yet) used (except by gdb) */
};

/*
 * NOTE!! could make f a "char" and v a 24-bit bitfield,
 * like on the '360 (limits object size to 2MB**), but could
 * slow things down quite a bit!!
 *
 * ** 2^24 / DESCR
 * would be 16MB if "descr" was the unit of addressing (and not bytes)
 */

struct descr {
    union addr a;			/* address (new: v) */
    int f;				/* flag (new: f) */
    int v;				/* value (new: t) data-type/size */
};

/* addressing unit is "char" */
#define CPA	1			/* chars per addr */
#define CPD	DESCR			/* chars per descr */

#define DESCR (sizeof(struct descr))

/*
 * string specifier (qualifier)
 *
 * NOTE!! A specifier is made up of two adjacent decsriptors,
 * thus the "unused" field, and the use of "union addr" for "l"
 */

struct spec {				/* (new: qualifier) */
    union addr l;			/* length (overlays addr) */
    int unused;				/* MBZ (must overlay flags) */
    int o;				/* offset */
    union addr a;			/* address (new: v) */
    int f;				/* flags */
    int v;				/* value (new: t) */
};
#define SPEC  (sizeof(struct spec))

typedef int (*func_t)();
/* $Id$ */

