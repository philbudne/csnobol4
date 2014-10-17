/* $Id$ */

/*
 * GNU readline() function
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "equ.h"
#include "str.h"			/* strlen() */
#undef RETURN

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* for free() */
#endif

/*
 * READLINE(STRING)STRING
 */
int
READLINE( LA_ALIST ) LA_DCL
{
    char *prompt;
    char *ret;

    prompt = mgetstring(LA_PTR(0));
    ret = readline(prompt);
    free(prompt);
    if (!ret)
	RETFAIL;
    RETSTR_FREE(ret);
}

/*
 * ADD_HISTORY(STRING)
 */
int
ADD_HISTORY( LA_ALIST ) LA_DCL
{
    char *line;

    line = mgetstring(LA_PTR(0));
    add_history(line);
    free(line);
    RETNULL;
}

/*
 * HISTORY_EXPAND(STRING)STRING
 */
int
HISTORY_EXPAND( LA_ALIST ) LA_DCL
{
    char *line;
    char *exp;
    int ret;

    line = mgetstring(LA_PTR(0));
    ret = history_expand(line, &exp);
    if (ret < 0 || ret == 2)
	RETFAIL;
    RETSTR_FREE(exp);			/* hope this is right! */
}
