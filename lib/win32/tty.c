/* $Id$ */

/*
 * tty mode, echo
 * Win32 version
 * started 1/28/98 -phil
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include <windows.h>

/* keep settings for each fd in a list; */
struct save {
    struct save *next;
    int fd;				/* XXX use HANDLE instead? */
    DWORD flags;			/* saved flags */
    int cbreak, noecho;			/* current state */
};

#ifdef NO_STATIC_VARS
#include "vars.h"
#define list ttylist
#else
static struct save *list;
#endif

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
    DWORD flags;

    /*
     * See if it's something we can handle
     *
     * Microsoft isatty() returns true for any character device
     * (including console, serial, printer).  Revert this to
     * isatty(fileno(fp)) when all devices supported below?
     *
     * use GetCommState() to detect serial lines
     */

    return GetConsoleMode((HANDLE)_get_osfhandle(fileno(f)), &flags);
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    int fd;
    DWORD new;
    HANDLE hand;
    struct save *sp;

    fd = fileno(fp);
    hand = (HANDLE)_get_osfhandle(fd);

    /* XXX move to tty_save_fd()?? */
    for (sp = list; sp; sp = sp->next) {
	if (sp->fd == fd)
	    goto found;
    }
    sp = (struct save *)malloc(sizeof(struct save));
    if (sp == NULL)
	return;				/* ??? */

    /* save "original" settings (used for "cooked" I/O) */
    sp->fd = fd;
    GetConsoleMode(hand, &sp->flags);
    sp->noecho = sp->cbreak = 0;	/* ??? */

    /* link into list */
    sp->next = list;
    list = sp;
 found:
    if (cbreak == sp->cbreak && noecho == sp->noecho)
	return;				/* nothing to do! */

    fflush(fp);				/* flush pending output */

    new = sp->flags;			/* start with original */
    if (cbreak) {
	new &= ~ENABLE_LINE_INPUT;
#ifdef TTY_RAW
	new &= ~(ENABLE_PROCESSED_INPUT|
		 ENABLE_PROCESSED_OUTPUT|
		 ENABLE_WRAP_AT_EOL_OUTPUT);
#endif /* TTY_RAW defined */
#ifdef NEED_SETMODE
	setmode(fd, O_BINARY);		/* needed?? */
#endif /* NEED_SETMODE defined */
    }
    else {
#ifdef NEED_SETMODE
	setmode(fd, O_TEXT);		/* needed?? */
#endif /* NEED_SETMODE defined */
    }

    if (noecho) {
	/* only works in line mode? */
	new &= ~ENABLE_ECHO_INPUT;
    }

    SetConsoleMode(hand, new);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
}

/* advisory notice; discard saved info.
 * NOTE: this loses if device remains open
 *	(ie; in use by a child proc, or has been dup'ed)
 */
static void
tty_close_fd(fd)
    int fd;
{
    struct save *sp, *pp;

    for (pp = NULL, sp = list; sp; pp = sp, sp = sp->next) {
	if (sp->fd == fd) {
	    if (pp) {
		pp->next = sp->next;
	    }
	    else {
		list = sp->next;
	    }
	    free(sp);
	    break;
	}
    }
}

/* advisory notice */
void
tty_close(f)
    FILE *f;
{
    tty_close_fd(fileno(f));
}
