/* $Id$ */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/*
 * tty mode, echo
 * POSIX.1 version
 */

/* XXX remove saved settings on close? restore all settings on exit?? */

/* keep settings for each fd in a list; */
static struct save {
    struct save *next;
    int fd;
    struct termios t;
    int cbreak, noecho;
#ifdef TTY_SAVE_RECL
    int recl;
#endif
} *list;

static struct termios old;		/* stdin on entry */

int
fisatty(f)
    FILE *f;
{
    return isatty(fileno(f));
}

void
tty_save()
{
    /* XXX call tty_save_fd(STDIN_FILENO)?? */
    tcgetattr(STDIN_FILENO, &old);
}

void
tty_restore()
{
    /* XXX call tty_close_fd(STDIN_FILENO)?? */
    tcsetattr(STDIN_FILENO, TCSADRAIN, &old);
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
#ifdef TTY_SAVE_RECL
    sp->recl = -1;
#endif

    /* link into list */
    sp->next = list;
    list = sp;
 found:
    /* XXX ensure cbreak & noecho are canonical? x = !!x?? */
    if (cbreak == sp->cbreak && noecho == sp->noecho
#ifdef TTY_SAVE_RECL
	&& recl == sp->recl
#endif
	)
	return;				/* nothing to do! */

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
#endif
#ifdef TTY_SAVE_RECL
	new.c_cc[VMIN] = recl;		/* number of chars wanted */
#else
	new.c_cc[VMIN] = 1;		/* one character */
#endif
	new.c_cc[VTIME] = 0;		/* wait as long as we have to */
    }

    if (noecho)
	new.c_lflag &= ~ECHO;		/* kill echo */

    tcsetattr(fd, TCSADRAIN, &new);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
#ifdef TTY_SAVE_RECL
    sp->recl = recl;
#endif
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
