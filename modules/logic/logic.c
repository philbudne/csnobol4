/*
 * $Id$
 *
 * Catspaw Macro SPITBOL compatible LOGIC() function
 * Phil Budne <phil@ultimate.com>
 * January 21, 2002
 *
 * To compile;
 *	gcc -shared -o logic.so -fPIC logic.c -I/usr/local/lib/snobol4
 *
 * To use;
 * -INCLUDE 'logic.sno'
 */

#ifdef HAVE_CONFIG_H
#include "config.h"			/* before ANYTHING */
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio, h.h */
#include <stdlib.h>			/* for malloc(), strtol() */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"

#include "load.h"			/* LA_xxx macros */
#include "equ.h"			/* datatypes I/S */

/* move to snotypes.h? */
typedef unsigned INT_T u_int_t;

/* move to snotypes.h? */
#ifndef BPC
#define BPC 8				/* bits/char */
#endif /* BPC not defined */

#define INTBITS (sizeof(u_int_t)*BPC)	/* int size, in bits */

#define MAXLEN 512			/* at least 512 for SPITBOL compat. */

/* XXX move to logic.h; use to generate logic.sno? */
#define OP_MINOP OP_NOT
#define OP_NOT	1
#define OP_AND	2
#define OP_OR	3
#define OP_XOR	4
#define OP_NAND	5
#define OP_NOR	6
#define OP_UPLUS 7
#define OP_UMINUS 8
#define OP_UMUL	9
#define OP_UDIV	10
#define OP_SHL	11
#define OP_SHR	12
#define OP_SAR	13
#define OP_ROL	14
#define OP_ROR	15
#define OP_DIB	16
#define OP_IDB	17
#define OP_MAXOP OP_IDB

/* declare as "inline"? */
static unsigned char
logic_byte( op, arg2, arg3 )
    int op;
    unsigned char arg2, arg3;
{
    switch (op) {
    case OP_AND:
	return(arg2 & arg3);
    case OP_OR:
	return(arg2 | arg3);
    case OP_XOR:
	return(arg2 ^ arg3);
    case OP_NAND:
	return(~(arg2 & arg3));
    case OP_NOR:
	return(~(arg2 | arg3));
    case OP_UPLUS:
	return(arg2 + arg3);
    case OP_UMINUS:
	return(arg2 - arg3);
    case OP_UMUL:
	return(arg2 * arg3);
    case OP_UDIV:
	return(arg2 / arg3);
    case OP_SHL:
	return(arg2 << arg3);
    case OP_SHR:
	return(arg2 >> arg3);
    case OP_SAR:
	return(((int_t)arg2) >> arg3);
    case OP_ROL:
	return((arg2 << arg3) | (arg2 >> (BPC-arg3)));
    case OP_ROR:
	return((arg2 >> arg3) | (arg2 << (BPC-arg3)));
    }
}

int
LOGIC( LA_ALIST ) LA_DCL
{
    int_t op = LA_INT(0);
    int a2type, a3type;
    u_int_t arg2, arg3;
    int len, retlen;
    const unsigned char *cp;
    unsigned char *rp;
    unsigned char retbuf[MAXLEN];

    if (op < OP_MINOP || op > OP_MAXOP)
	RETFAIL;

    a2type = LA_TYPE(1);
    a3type = LA_TYPE(2);

    if (op == OP_DIB || op == OP_IDB) {	/* conversion */
	int base;
	char buf[32];
	char *cp;

	if (a3type == I)
	    base = LA_INT(2);
	else if (a3type == S && LA_PTR(2) == NULL) {
	    getstring(LA_PTR(2), buf, sizeof(buf));
	    if (strlen(buf) > 0)
		RETFAIL;
	    base = 0;			/* null str */
	}
	else
	    RETFAIL;

	if (base == 0)
	    base = 16;
	else if (base < 2 || base > 16)
	    RETFAIL;

	if (op == OP_DIB) {
	    int_t result;

	    if (a2type != S)
		RETFAIL;

	    getstring(LA_PTR(1), buf, sizeof(buf));

	    /* XXX sigh; this loses if int_t is "long long" */
	    /* XXX DANGER!! Some systems may not have strtol()!! */
	    result = strtol(buf, &cp, base);
	    if (*cp)
		RETFAIL;		/* incomplete conversion */

	    RETTYPE = I;
	    RETINT(result);
	}
	else {
	    if (a2type != I)
		RETFAIL;

	    cp = buf + sizeof(buf);
	    arg2 = LA_INT(1);
	    do {
		*--cp = "0123456789ABCDEF"[arg2 % base];
		arg2 /= base;
	    } while (arg2);
	    RETSTR2(cp, buf + sizeof(buf) - cp);
	}
    } /* conversions */
    else if (op == OP_NOT) {		/* NOT (takes single arg) */
	if (LA_TYPE(1) == I) {
	    RETTYPE = I;
	    RETINT(~LA_INT(1));
	}
	else if (LA_TYPE(1) == S) {
	    retlen = len = LA_STR_LEN(1);
	    if (len > MAXLEN)
		RETFAIL;
	    cp = (unsigned char *)LA_STR_PTR(1);
	    rp = retbuf;
	    while (len-- > 0)
		*rp++ = ~*cp++;
	    RETSTR2(retbuf, retlen);
	}
	else
	    RETFAIL;
    } /* NOT */

    /* both args must be either INTEGER or STRING */
    if (LA_TYPE(1) != I && LA_TYPE(1) != S ||
	LA_TYPE(2) != I && LA_TYPE(2) != S)
	RETFAIL;

    if (LA_TYPE(1) == I && LA_TYPE(2) == I) { /* both INTEGER */
	arg2 = LA_INT(1);
	arg3 = LA_INT(2);

	RETTYPE = I;
	switch (op) {
	case OP_AND:
	    RETINT(arg2 & arg3);
	case OP_OR:
	    RETINT(arg2 | arg3);
	case OP_XOR:
	    RETINT(arg2 ^ arg3);
	case OP_NAND:
	    RETINT(~(arg2 & arg3));
	case OP_NOR:
	    RETINT(~(arg2 | arg3));
	case OP_UPLUS:
	    RETINT(arg2 + arg3);
	case OP_UMINUS:
	    RETINT(arg2 - arg3);
	case OP_UMUL:
	    RETINT(arg2 * arg3);
	case OP_UDIV:
	    RETINT(arg2 / arg3);
	case OP_SHL:
	    RETINT(arg2 << arg3);
	case OP_SHR:
	    RETINT(arg2 >> arg3);
	case OP_SAR:
	    RETINT(((int_t)arg2) >> arg3);
	case OP_ROL:
	    RETINT((arg2 << arg3) | (arg2 >> (INTBITS-arg3)));
	case OP_ROR:
	    RETINT((arg2 >> arg3) | (arg2 << (INTBITS-arg3)));
	}
	RETFAIL;
    } /* both integers */

    /* here with at least one STRING arg */
    if (a2type == S) {
	len = LA_STR_LEN(1);
	if (a3type == S && LA_STR_LEN(2) != len)
	    RETFAIL;
    }
    else
	len = LA_STR_LEN(2);

    if (len == 0)
	RETNULL;			/* easy!! */

    if (len > MAXLEN)
	RETFAIL;

    retlen = len;
    rp = retbuf;
    if (a2type == S && a3type == S) {	/* ARG2, ARG3 STRING */
	const unsigned char *cp2;

	cp = (const unsigned char *)LA_STR_PTR(1);
	cp2 = (const unsigned char *)LA_STR_PTR(2);
	while (len-- > 0) 
	    *rp++ = logic_byte(op, *cp++, *cp2++);
    }
    else if (a2type == S) {		/* ARG2 STRING, ARG3 INTEGER */
	cp = (const unsigned char *)LA_STR_PTR(1);
	arg3 = LA_INT(2);
	while (len-- > 0) 
	    *rp++ = logic_byte(op, *cp++, arg3);
    }
    else {				/* ARG2 INTEGER, ARG3 STRING */
	arg2 = LA_INT(1);
	cp = (const unsigned char *)LA_STR_PTR(2);
	while (len-- > 0) 
	    *rp++ = logic_byte(op, arg2, *cp++);
    }
    RETSTR2(retbuf, retlen);
} /* LOGIC */
