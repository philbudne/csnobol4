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
#include <io.h>		/* _get_osfhandle */
#include <malloc.h>	/* available (must have!) in VS 2019 Win64 */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

/* keep settings for each handle in a list; */
struct save {
    struct save *next;
    HANDLE h;				/* input handle */
    HANDLE oh;				/* output handle */
    DWORD flags;			/* saved flags */
    int cbreak, noecho;			/* current state */
};

static VAR struct save *list;

int
fisatty(FILE *f) {
    DWORD flags;
    HANDLE h;

    /*
     * See if it's something we can handle
     *
     * Microsoft isatty() returns true for any character device
     * (including console, serial, printer).  Revert this to
     * isatty(fileno(fp)) when more devices supported below?
     *
     * use GetCommState() to detect serial lines
     */

    h = (HANDLE)_get_osfhandle(fileno(f));
    return GetConsoleMode(h, &flags);
}

void
tty_mode(FILE *fp, int cbreak, int noecho, int recl) {
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

    /* save "original" settings */
    sp->h = h;
    GetConsoleMode(h, &sp->flags);	/* SHOULD NEVER FAIL! */
    sp->noecho = sp->cbreak = 0;	/* ??? */
    sp->oh = INVALID_HANDLE_VALUE;

    /* link into list (can we have more than one console per process???) */
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

    SetConsoleMode(h, new);
    FlushConsoleInputBuffer(h);		/* avoid leftover CRLFs? */

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
tty_close_handle(HANDLE h) {
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
tty_close(FILE *f) {
    tty_close_handle((HANDLE)_get_osfhandle(fileno(f)));
}

/* called for raw tty reads if TTY_READ_RAW defined */
int
tty_read(FILE *f, char *buf, int len,
	 int raw, int noecho, int keepeol, char *fname) {
    HANDLE h;

    if (!raw)
	return -1;

    h = (HANDLE)_get_osfhandle(fileno(f));
    if (noecho) {
	DWORD cc;

	if (ReadFile(h, buf, len, &cc, NULL))
	    return cc;
	return -1;
    }
    else {
	struct save *sp;
	int cc;
	DWORD x;

	/* find output handle, if any */
	for (sp = list; sp && sp->h != h; sp = sp->next)
	    ;
	if (sp && sp->oh == INVALID_HANDLE_VALUE)
	    sp->oh = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE, 
				FALSE, OPEN_EXISTING, 0, NULL);

	/* read character at a time, echo to output console */
	for (cc = 0; cc < len; cc++ ) {
	    if (!ReadFile(h, buf, 1, &x, NULL) || x != 1)
		break;
	    if (sp && sp->oh != INVALID_HANDLE_VALUE)
	        (void) WriteFile(sp->oh, buf, 1, &x, NULL);
	    buf++;
	}
	return cc;
    }
} /* tty_read */
