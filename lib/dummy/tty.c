/* $Id$ */

#include <stdio.h>
#include "h.h"				/* for FALSE */

/*
 * tty mode, echo
 * dummy version
 */

int
fisatty(f)
    FILE *f;
{
    return FALSE;			/* why not? */
}

void
tty_save()
{
}

void
tty_restore()
{
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
}

/* advisory notice */
void
tty_close(f)
    FILE *f;
{
    /* should not be called (fisatty returns FALSE) */
}
