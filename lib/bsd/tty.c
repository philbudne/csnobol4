/* $Id$ */

#include <stdio.h>
#include <sgtty.h>

/*
 * tty mode, echo
 * BSD/V7 version
 */

/* XXX restore ALL settings on exit?? */

/* keep settings for each fd in a list; */
static struct save {
    struct save *next;
    int fd;
    struct sgttyb t;
#if defined(TTY_RAW) && defined(LPASS8)
    int local;
#endif
    int cbreak, noecho;
} *list;

static struct sgttyb old;		/* stdin on entry */
#if defined(TTY_RAW) && defined(LPASS8)
static int lflags;
#endif

#define STDIN_FILENO 0

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
    gtty(STDIN_FILENO, &old);
#if defined(TTY_RAW) && defined(LPASS8)
    ioctl(STDIN_FILENO, TIOCLGET, &lflags);
#endif
}

void
tty_restore()
{
    /* XXX call tty_close_fd(STDIN_FILENO)?? */
    stty(STDIN_FILENO, &old);		/* use TIOCSETN? */
#if defined(TTY_RAW) && defined(LPASS8)
    ioctl(STDIN_FILENO, TIOCLSET, &lflags);
#endif
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    struct sgttyb new;
#if defined(TTY_RAW) && defined(LPASS8)
    int l;
#endif
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
    gtty(fd, &sp->t);			/* save settings */
#if defined(TTY_RAW) && defined(LPASS8)
    ioctl(fd, TIOCLGET, &sp->local);
#endif

    sp->noecho = sp->cbreak = 0;	/* ??? */

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
#if defined(TTY_RAW) && defined(LPASS8)
    l = sp->local;
#endif
    if (cbreak) {
#ifdef TTY_RAW
	new.sg_flags |= RAW;
#ifdef LPASS8
	l |= LPASS8|LLITOUT;
#endif
#else
	new.sg_flags |= CBREAK;
#endif
	new.sg_flags &= ~CRMOD;		/* leave LF alone */
    }

    if (noecho)
	new.sg_flags &= ~ECHO;		/* kill echo */

    stty(fd, &new);			/* use TIOCSETN? */
#if defined(TTY_RAW) && defined(LPASS8)
    ioctl(fd, TIOCLSET, &l);
#endif

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
}

/*
 * advisory notice; discard saved info.
 *
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
