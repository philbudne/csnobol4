/* $Id$ */

/*
 * LOAD("ORD(STRING)INTEGER")
 *
 * Usage;	ORD(STRING)
 * Returns;	ordinal value of first character of STRING
 *		CHAR(ORD('X')) should return 'X'
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"

int
ORD( LA_ALIST ) LA_DCL
{
    if (LA_PTR(0) == NULL || LA_STR_LEN(0) == 0)
	RETFAIL;
    RETINT((unsigned char)*LA_STR_PTR(0));
}
