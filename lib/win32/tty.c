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
#include <io.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/* keep settings for each fd in a list; */
struct save {
    struct save *next;
    HANDLE h;
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
    HANDLE h;

    /*
     * See if it's something we can handle
     *
     * Microsoft isatty() returns true for any character device
     * (including console, serial, printer).  Revert this to
     * isatty(fileno(fp)) when all devices supported below?
     *
     * use GetCommState() to detect serial lines
     */

    h = (HANDLE)_get_osfhandle(fileno(f));
    return GetConsoleMode(h, &flags);
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    DWORD new;
    HANDLE h;
    struct save *sp;

    h = (HANDLE)_get_osfhandle(fileno(fp));
    for (sp = list; sp; sp = sp->next) {
	if (h == sp->h)
	    goto found;
    }
    sp = (struct save *)malloc(sizeof(struct save));
    if (sp == NULL)
	return;				/* ??? */

    /* save "original" settings (used for "cooked" I/O) */
    sp->h = h;
    GetConsoleMode(h, &sp->flags);
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
	/* MUST clear ECHO to disable LINE_INPUT */
	new &= ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
#ifdef TTY_RAW
	new &= ~ENABLE_PROCESSED_INPUT;	/* ^C processing */
#endif /* TTY_RAW defined */
    }

    if (noecho)
	new &= ~ENABLE_ECHO_INPUT;	/* only works in line mode? */

    FlushConsoleInputBuffer(h);		/* else gets CR LF? */
    SetConsoleMode(h, new);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
}

/*
 * advisory notice; discard saved info.
 * NOTE: this loses if device remains open
 *	(ie; in use by a child proc, or has been dup'ed)
 */
static void
tty_close_handle(h)
    HANDLE h;
{
    struct save *sp, *pp;

    for (pp = NULL, sp = list; sp; pp = sp, sp = sp->next) {
	if (sp->h == h) {
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
    tty_close_handle((HANDLE)_get_osfhandle(fileno(f)));
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
    DWORD cc;
    HANDLE h;

    if (!raw)				/* paranoia */
	return -1;

    h = (HANDLE)_get_osfhandle(fileno(f));
    if (!ReadFile(h, buf, len, &cc, NULL))
	return -1;

    /*
     * perform raw mode echo
     * OUCH!! too many system calls
     * save output handle in struct save??
     */
    if (cc > 0 && !noecho && h == GetStdHandle(STD_INPUT_HANDLE)) {
      DWORD occ;
      WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, cc, &occ, NULL);
    }

    /* cc == 0 means EOF */
    return cc;
} /* tty_read */
