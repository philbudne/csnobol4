/* $Id$ */

/*
 * GNU readline() function
 */

/*
**=pea
**=sect NAME
**snobol4readline \- SNOBOL4 readline interface
**=sect SYNOPSIS
**=code
**-INCLUDE 'readline.sno'
**        INPUT_STRING = READLINE(PROMPT_STRING)
**        ADD_HISTORY(STRING)
**        STRING = HISTORY_EXPAND(STRING)
**=ecode
**=sect DESCRIPTION
**=cut
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
**=pea
**=item B<READLINE(>I<prompt>B<)>
**performs user input with line editing.
**=cut
*/
/*
 * LOAD("READLINE(STRING)STRING", READLINE_DL)
 */
lret_t
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
**=pea
**=item B<ADD_HISTORY(>I<string>B<)>
**add I<string> to the input history available via editing commands.
**=cut
*/
/*
 * LOAD("ADD_HISTORY(STRING)", READLINE_DL)
 */
lret_t
ADD_HISTORY( LA_ALIST ) LA_DCL
{
    char *line;

    line = mgetstring(LA_PTR(0));
    add_history(line);
    free(line);
    RETNULL;
}

/*
**=pea
**=item B<HISTORY_EXPAND(>I<string>B<)>
**expand I<string> (typically B<!command>).
**=cut
*/
/*
 * LOAD("HISTORY_EXPAND(STRING)STRING", READLINE_DL)
 */
lret_t
HISTORY_EXPAND( LA_ALIST ) LA_DCL
{
    char *line;
    char *exp;
    int ret;

    line = mgetstring(LA_PTR(0));
    ret = history_expand(line, &exp);
    if (ret < 0 || ret == 2)
	RETFAIL;
    free(line);
    RETSTR_FREE(exp);			/* hope this is right! */
}

/*
**=pea
**=sect SEE ALSO
**B<snobol4>(1), B<readline>(3)
**=sect AUTHOR
**Philip L. Budne
**=cut
*/

