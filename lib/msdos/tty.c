/* $Id$ */

/*
 * tty mode, echo
 * borland version
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <conio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

int
fisatty(f)
    FILE *f;
{
    /*
     * XXX we can only do tty_read() on keyboard so this better not
     * return true for opens on COM: devices. If so replace "isatty"
     * with check for fd 0, 1 or 2?
     */
    return isatty(fileno(f));
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

/* called for raw tty reads if TTY_READ_RAW defined */
int
tty_read(f, buf, len, raw, noecho, keepeol, fname)
    FILE *f;
    char *buf;
    int len;
    int raw;
    int noecho;
    int keepeol;
    char *fname;
{
    int cc;

    if (!raw)				/* paranoia */
	return -1;

    for (cc = 0; cc < len; cc++) {
	while (!kbhit())
	    ;
	*buf++ = noecho ? getch() : getche();
    }
    return cc;
} /* tty_read */
