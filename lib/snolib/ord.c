#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * LOAD("ORD(STRING)INTEGER")
 *
 * Usage;	ORD(STRING)
 * Returns;	ordinal value of first character of STRING
 *		CHAR(ORD('X')) should return 'X'
 */

ORD( LA_ALIST ) LA_DCL
{
    char str[2];
    getstring( LA_PTR(0), str, sizeof(str) );
    /* sigh; this returns zero for the empty string.  FAIL instead?? */
    RETINT( (unsigned char)str[0] );
}
