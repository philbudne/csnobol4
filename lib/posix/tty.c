/* $Id$ */

/*
 * tty mode, echo
 * POSIX.1 version
 */

/*
 * BUGS: while attempting to minimize thrashing tty modes
 *	doesn't catch multple fd's on same device.
 * 	could do fstat() and compare "rdev", but
 *	/dev/tty still slips by!
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>

#ifdef USE_TERMIO
/* awful; but better than duplicating the whole file! */
#include <termio.h>
#define tcgetattr(FD,T) ioctl(FD, TCGETA, T)
#define tcsetattr(FD,HOW,T) ioctl(FD, HOW, T)
#define TCSADRAIN TCSETAW
#define termios termio
#define STDIN_FILENO 0
#else  /* USE_TERMIO not defined */
#include <termios.h>
#include <unistd.h>
#endif /* USE_TERMIO not defined */

/* keep settings for each fd in a list; */
static struct save {
    struct save *next;
    int fd;
    struct termios t;
    int cbreak, noecho;
    int recl;
} *list;

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
    return isatty(fileno(f));
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    struct termios new;
    struct save *sp;
    int fd;

    fd = fileno(fp);

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
    tcgetattr(fd, &sp->t);		/* save settings */
    sp->noecho = sp->cbreak = 0;	/* ??? */
    sp->recl = -1;

    /* link into list */
    sp->next = list;
    list = sp;
 found:
    if (cbreak == sp->cbreak && noecho == sp->noecho &&
	(!cbreak || recl == sp->recl))
	return;				/* nothing to do! */

    fflush(fp);				/* flush pending output */

    new = sp->t;			/* start with original */
    if (cbreak) {
	new.c_lflag &= ~ICANON;		/* kill canonical processing */
	new.c_iflag &= ~(ICRNL|INLCR);	/* hey, system! leave CR/LF alone! */
#ifdef TTY_RAW
	new.c_lflag &= ~ISIG;		/* kill signal processing */
	new.c_iflag &= ~(IXON|IXOFF);	/* pass CTRLS/CTRLQ */
	new.c_oflag &= ~OPOST;		/* kill output post-processing */
	/* XXX set CS8, PASS8, IGNPAR, clear ISTRIP? */
	/* XXX clear IUCLC, XCASE (if they exist)? */
#endif /* TTY_RAW defined */

	if (recl > 0xff)		/* VMIN is a char! */
	    recl = 0xff;

	new.c_cc[VMIN] = recl;		/* number of chars wanted */
	new.c_cc[VTIME] = 0;		/* wait as long as we have to */
    }

    if (noecho)
	new.c_lflag &= ~ECHO;		/* kill echo */

    tcsetattr(fd, TCSADRAIN, &new);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
    sp->recl = recl;
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

void
tty_save()
{
    /* XXX call tty_save_fd(STDIN_FILENO)?? */
    tty_mode(stdin, 0, 0, 0);		/* force initial save */
}

void
tty_restore()
{
    /* XXX call tty_close_fd(STDIN_FILENO)?? */
    tty_mode(stdin, 0, 0, 0);		/* restore initial settings */
}

/* advisory notice */
void
tty_close(f)
    FILE *f;
{
    tty_close_fd(fileno(f));
}
