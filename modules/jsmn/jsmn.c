/* $Id$ */

/*
 * jsmn (JSON tokenizer) module for CSNOBOL4
 * see funcs.sno for user callable function JSMN_DECODE
 * Phil Budne <phil@ultimate.com> 7/2/2026
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "module.h"

#define JSMN_STATIC
#include "jsmn.h"

#include <stdlib.h>			/* malloc/free */
SNOBOL4_MODULE(jsmn)

MFUNC(JSMN_TOKENS);

static int
t2c(int type) {
    switch (type) {
    case JSMN_PRIMITIVE: return 'p';
    case JSMN_OBJECT: return 'o';
    case JSMN_ARRAY: return 'a';
    case JSMN_STRING: return 's';
    }
    return '?';
}

/*
 * LOAD("JSMN_TOKENS(STRING,INTEGER,ARRAY)", JSMN_DL)
 */
lret_t
JSMN_TOKENS( LA_ALIST ) {
    int ntok = LA_INT(1);
    struct descr *out = LA_PTR(2);
    struct jsmn_parser p;
    jsmntok_t *tokens;
    int r;

    (void) nargs;
    jsmn_init(&p);
    if (ntok > 0)
	tokens = malloc(sizeof(jsmntok_t) * ntok);
    else
	tokens = NULL;			/* just return count */

    r = jsmn_parse(&p, LA_STR_PTR(0), LA_STR_LEN(0), tokens, ntok);

    if (tokens && out && r > 0) {
	int i;
	jsmntok_t *tp = tokens;
	int j = 5;

	/* copy tokens to "out" 2D array */
#define COPY(VAL) \
	    out[j].a.i = VAL; \
	    out[j].f = 0; \
	    out[j].v = I; \
	    j++

	for (i = 0; i < r; i++, tp++) {
	    COPY(t2c(tp->type));
	    COPY(tp->start);		/* zero based start char pos */
	    COPY(tp->end);		/* zero based end char pos */
	    COPY(tp->size);		/* number of child tokens */
	}
	free(tokens);
    }
    if (r < 0)
	RETFAIL;
    RETINT(r);
}
