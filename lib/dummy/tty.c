/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/*
 * tty mode, echo
 * dummy version
 */

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
    return FALSE;			/* why not? */
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

void
tty_suspend()
{
}
