/*
 * $Id$
 *
 * C structs for accessing SNOBOL4 internal data
 * structures from C-code, or when using a source-code debugger.
 * Phil Budne 8/15/1997
 *
 * THIS file should be called "snotypes.h"
 * the existing snotypes.h should be called "siltypes.h"
 */

/*
 * Page numbers refer to "The Macro Implementation of SNOBOL4"
 * Ralph E. Griswold, W.H. Freeman & Co, San Francisco, 1972
 * ISBN 0-7167-0447-1
 */

/*
 * All internal data structures (and allocated "blocks") start with a
 * "title" descriptor for "storage regeneration", AKA garbage collecttion.
 *
 * The title "v" field contains information on the size of the string/block.
 * The "f" (flags) field always has the TTL flag set.
 * The "a" (addr) field contains a self pointer, used for relocation
 *	during GC.
 */

/*
 * Important rules;
 *
 * In order for the garbage collector to find all active data;
 * + All pointers to data must have the PTR flag set.
 * + All static data that contains pointers must be located in
 *   one of the "resident" blocks listed at PRMTBL.
 */

/*
 * datatype codes
 *
 * in many contexts a desciptor is used to represent a SNOBOL4 datum.
 * the "v" field contains a datatype, and "a" contains a value or address.
 *
 * here is the list of datatype codes used;
 * (type codes vs. names are kept in pairlist DTATL)
 *
 *	A = ARRAY
 *	B = BLOCK (internal)
 *	C = CODE
 *	E = EXPRESSION
 *	I = INTEGER (a.i contains value)
 *	K = KEYWORD (NAME)
 *	L = LINKED STRING (internal -- returned by loaded functions)
 *	N = NAME
 *	P = PATTERN
 *	R = REAL (a.f contains value)
 *	S = STRING (a.ptr is pointer to natural variable for value)
 *	T = TABLE
 *	"user datatypes" are represented as numbers starting at DATSTA)
 */

/****************************************************************
 * "natural variables" (p. 96)
 *
 * used to represent all strings and variable names.
 * only one instance of each NV exists, and all are entered in
 * that OBSTRT hash table
 */

struct nv {
    /* title (see above)
     * v: count of CHARACTERS in variable name/string.
     * f: STTL flag is always set
     * a: self pointer
     */
    struct descr title;

    /* SNOBOL4 variable value;
     * v: datatype code
     * f: PTR flag is set if "a" used for pointer to value
     * a: integer/real value or pointer
     */
    struct descr value;

    /* SNOBOL4 label information; */
    struct descr attrib;

    /* link information for for natural variable hash table;
     * a: pointer to next natural variable in this hash chain
     * v: "order number" (or assention value) used to order chain
     */
    struct descr lnkfld;

    /* natural variable name */
    char string[1];
};

/*
 * BLOCKS
 *
 * All other data is stored in blocks of descriptors.
 * The title "v" field is a count of ADDITIONAL descriptors
 *	multiplied DESCR (size of a descriptor in addressing units)
 * in the block.
 */

/****************************************************************
 * function "link descriptor" block (pg. 66)
 */

/*
 * All functions are entered into the "function table" value/name pair
 * block (see below) referenced by FNCPL. Each pair in the table is
 * known as a "function descriptor". The name descriptor entries point
 * to string specifiers for function names. The value descriptor
 * entries point to a "link descriptor block" which contains
 * information about the function.  Each function has only one link
 * descriptor block, but multiple table entries can refer to the same
 * link descriptor (due to OPSYN).
 */

struct func_ld_block {
    /*
     * link descriptor;
     * v: function argument count
     * f: FNC flag???
     * a: pointer to function
     */
    struct descr link;

    /*
     * definition descriptor.
     * used by user defined functions and user datatypes (see below)
     */
    struct descr defn;
};


/****************************************************************
 * function "define block" (pg. 100);
 */

/* definition descriptor of a user defined function points to
 * a "define block".  The number of vars which are args vs. locals
 * can be determined from the "link" descriptor of the function
 * link definition block.
 */

struct defineblock {
    struct descr title;
    struct descr name;			/* name of function */
    struct descr vars[1];		/* names of args & locals */
};

/****************************************************************
 * user datatypes (p. 105)
 */

/*
 * user datatype numbers and names are entered in the value/name
 * block pointed to by DTATL.
 */

/*
 * Each user datatype creates one entry in the function table (see
 * above) for it's datatype name "constructor" function, and one entry
 * for each field in the datatype. If a field of the same name in a
 * different datatype already exists, the existing "field block" is
 * expanded.
 */

/*
 * The "definition descriptor" for a constructor contains datatype type
 * number in the "v" field and a pointer to a DATA block in the "a"
 * field.
 */

struct datablock {			/* see p. 107 */
    struct descr title;
    struct descr type;			/* v: type number */
    struct descr name;			/* string pointer to name nv */
    struct descr fields[1];		/* string pointers to field nv's */
};

/*
 * The definition descriptor for a field function points to a
 * FIELD block.
 */

/* for each field i=1..n in a data type */
struct fieldblock {			/* see p. 108 */
    struct descr title;

    /* one entry for each datatype this field name is used in */
    struct typeoffset {
	struct descr type;		/* v: type number f: 0 a: 0 */
	struct descr offset;		/* v: 0 f: 0 a: (i-1)*DESCR */
    } typeoffset[1];
};

/****************************************************************
 * a frequent use of blocks is to store pairs of types and values
 */

struct pairblock {
    struct descr title;
    struct pair {
	struct descr value;
	struct descr type;
    } pairs[1];
};

/****************************************************************
 * code is first constructed using "code nodes" trees
 * before generating prefix code.
 */

struct codenode {
    struct descr father;		/* father */
    struct descr lson;			/* leftmost son */
    struct descr rsib;			/* rightmost sibling */
    struct descr code;
};

/****************************************************************
 * patterns
 */

struct pattern {
    struct descr title;
    struct descr patnodes[1];		/* indexed by offsets */
};

struct patnode {
    /* pointer to matching function descriptor
     * (pointer to pointer to function)
     * v: 3 if optional argument (below) present, else 2
     * f: FNC flag
     */
    struct descr function;

    /* offsets from start of pattern node list;
     * v: offset to alternate pattern node
     * a: offset to subsequent pattern node
     */
    struct descr connector;


    /* match heuristic information;
     * v: residual (minimum that can be matched by remaining pattern)
     * a: total length (minimum subject length that can match)
     */
    struct descr heuristic;

    /* optional function arg (see above) */
    struct descr argument;
};

/****************************************************************
 * Arrays (p. 110)
 */

struct array {
    struct descr title;

    /* v: S (STRING)
     * f: PTR flag
     * a: pointer to prototype string ("low(1):high(1),low(2):high(2),...")
     */
    struct descr prototype;

    /* number of dimensions;
     * v: zero
     * f: zero
     * a: number of dimensions
     */
    struct descr ndim;

    /* list n..1 of;
     * v: size(i)
     * f: zero
     * a: low(i)
     *	where size(i) = high(i) - low(i) + 1
     *
     * array element values in row major order;
     *  (ie; A[1,1]; A[2,1]; A[1,2]; A[2,2])
     */
    struct descr data[1];
};

/****************************************************************
 * Tables (p. 112)
 */

/*
 * tables are implemented as pair blocks with additional extents
 * added once the current block is filled.
 */

/*
 * for TABLE(M,N)
 *
 * initial block:
 * =============
 * title (v: 2*M)
 * M-1 value, index descr pairs
 * the last pair contains;
 * v: 0 f: 0 a.i: 2*N
 * v: 0 f: PTR a.ptr: next
 *
 * additional extents:
 * ==================
 * title (v: 2*N)
 * (N-1) value, index pairs
 * the last pair contains;
 * v: 0 f: 0 a.i: 2*N
 * v: 0 f: PTR a.ptr: next
 * (last entry of last block contains v=0 f=0 a.i=1)
 *
 * empty pair entries have a zero "b" entry
 */

/****************************************************************
 * Keywords (p. 115)
 *
 * keywords are stored in two value/name pair tables;
 * KNATL (points to KNLIST) for unprotected keywords
 * KVATL (points to KVLIST) for protected keywords
 */

/****************************************************************
 * other stuff?
 *
 * input/output tables/blocks (p. 133)
 * tracing (p. 135)
 * prefix code
 */
