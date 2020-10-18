/* $Id$ */

/*
 * GNU readline() function
 *
 * sdb depends on readline, and can't avoid the include,
 * so supply trivial functionality even if readline not available.
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
#undef RETURN

#include <stdio.h>
#include <stdlib.h>			/* free() */
#include <string.h>			/* strlen() */

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif /* not HAVE_READLINE */


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
READLINE( LA_ALIST ) {
    char *prompt = mgetstring(LA_PTR(0));
#ifdef HAVE_READLINE
    char *ret = readline(prompt);
    free(prompt);
    if (!ret)
	RETFAIL;
#else
#define BUFSIZE 1024
    char *ret = malloc(BUFSIZE+1);
    int len;
    ret[BUFSIZE] = '\0';
    /* maybe this should be in io.c, to deal with terminal modes?? */
    fputs(prompt, stdout);
    fflush(stdout);
    if (!fgets(ret, BUFSIZE, stdin)) {
	free(ret);
	RETFAIL;
    }
    len = strlen(ret);
    while (len > 0) {
	if (ret[len-1] == '\n' || ret[len-1] == '\r')
	    len--;
	else
	    break;
    }
    ret[len] = '\0';
#endif
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
ADD_HISTORY( LA_ALIST ) {
#ifdef HAVE_READLINE
    char *line;

    line = mgetstring(LA_PTR(0));
    add_history(line);
    free(line);
    RETNULL;
#else
    RETFAIL;
#endif
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
HISTORY_EXPAND( LA_ALIST ) {
#ifdef HAVE_READLINE
    char *line;
    char *exp;
    int ret;

    line = mgetstring(LA_PTR(0));
    ret = history_expand(line, &exp);
    if (ret < 0 || ret == 2)
	RETFAIL;
    free(line);
    RETSTR_FREE(exp);			/* hope this is right! */
#else
    RETFAIL;
#endif
}

/*
**=pea
**=sect SEE ALSO
**B<snobol4>(1), B<readline>(3)
**=sect AUTHOR
**Philip L. Budne
**=cut
*/

