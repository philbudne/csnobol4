/* $Id$ */

/*
 * tty mode, echo
 * BSD/V7 version
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

#ifdef USE_TTYIO
#include <sys/ttyio.h>			/* Research Version 10 */
#else  /* USE_TTYIO not defined */
#include <sgtty.h>
#endif /* USE_TTYIO not defined */

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(TTY_RAW) && defined(LPASS8)
#define TTY_RAW_PASS8			/* shorthand */
#endif /* defined(TTY_RAW) && defined(LPASS8) */

/*
 * keep settings for each device in a list (by dev_t rather than fd
 * since multiple fd's can be open to same device (stdin/out/err))
 */
static struct save {
    struct save *next;
    dev_t dev;
    struct state {
	struct sgttyb t;
#ifdef TTY_RAW_PASS8
	int local;
#endif /* TTY_RAW_PASS8 defined */
    } save, curr;
    int cbreak, noecho;
} *list;

#ifdef TTY_RAW_PASS8
static int lflags;
#endif /* TTY_RAW_PASS8 defined */

#define STDIN_FILENO 0

/* Research V10 has no stty()? */
#ifdef TIOCSETP
/* in case; */
#undef stty
#undef gtty
/* use TIOCSETN? */
#define stty(F,P) ioctl(F, TIOCSETP, P)
#define gtty(F,P) ioctl(F, TIOCGETP, P)
#endif /* TIOCSETP defined */

enum action { FIND, CREATE, REMOVE };

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
    return isatty(fileno(f));
}

static void
tty_invalidate(sp)
    struct save *sp;
{
    sp->noecho = sp->cbreak = -1;
}

static struct save *
find_by_fd(fd, action)
    int fd;
    enum action action;
{
    struct stat st;
    struct save *sp, *pp;

    if (fstat(fd, &st) < 0 || !S_ISCHR(st.st_mode))
	return NULL;
    
    for (pp = NULL, sp = list; sp; pp = sp, sp = sp->next) {
	if (sp->dev == st.st_rdev) {
	    if (action == REMOVE)
		if (pp)			/* not first? */
		    pp->next = sp->next; /* alter prev's next */
		else
		    list = sp->next;	/* alter list head */
	    return sp;
	}
    }

    if (action != CREATE)
	return NULL;

    sp = (struct save *)malloc(sizeof(struct save));
    if (sp == NULL)
	return NULL;
    
    /* save "original" settings (used for "cooked" I/O) */
    sp->dev = st.st_rdev;

    gtty(fd, &sp->save.t);		/* save settings */
#ifdef TTY_RAW_PASS8
    ioctl(fd, TIOCLGET, &sp->save.local);
#endif /* TTY_RAW_PASS8 defined */
    tty_invalidate(sp);
    
    /* link into list */
    sp->next = list;
    list = sp;

    return sp;
}

static void
tty_set(fd, stp)
    int fd;
    struct state *stp;
{
    stty(fd, &stp->t);
#ifdef TTY_RAW_PASS8
    ioctl(fd, TIOCLSET, &stp->local);
#endif /* TTY_RAW_PASS8 defined */
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    struct save *sp;
    static dev_t last;
    int fd;

    fd = fileno(fp);
    sp = find_by_fd(fd, CREATE);
    if (!sp)
	return;				/* malloc failed, bad fd, bad dev */

    /*
     * check if last time thru;
     * was the same device & same mode combination
     */
    if (sp->dev == last && cbreak == sp->cbreak && noecho == sp->noecho)
	return;				/* nothing to do! */
    
    fflush(fp);				/* flush pending output */

    sp->curr = sp->save;		/* start with original */
    if (cbreak) {
#ifdef TTY_RAW
	sp->curr.t.sg_flags |= RAW;
#ifdef LPASS8
	sp->curr.local |= LPASS8|LLITOUT;
#endif /* LPASS8 defined */
#else  /* TTY_RAW not defined */
	sp->curr.t.sg_flags |= CBREAK;
#endif /* TTY_RAW not defined */
	sp->curr.t.sg_flags &= ~CRMOD;	/* leave LF alone */
    }

    if (noecho)
	sp->curr.t.sg_flags &= ~ECHO;	/* kill echo */

    tty_set(fd, &sp->curr);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
    last = sp->dev;
}

/* advisory notice (does not perform close) */
static void
tty_close(f)
    FILE *f;
{
    struct save *sp;
    int fd;

    fd = fileno(f);

#ifdef TTY_CLOSE_FREE
    sp = find_by_fd(fd, REMOVE);
#else  /* TTY_CLOSE_FREE not defined */
    /* try keeping information! */
    sp = find_by_fd(fd, FIND);
#endif /* TTY_CLOSE_FREE not defined */
    if (!sp)
	return;				/* not found, bad fd, bad device */
    
    stty(fd, &sp->save.t);
#ifdef TTY_RAW_PASS8
    ioctl(fd, TIOCLSET, &sp->save.local);
#endif /* TTY_RAW_PASS8 defined */
    
#ifdef TTY_CLOSE_FREE
    free(sp);
#else  /* TTY_CLOSE_FREE not defined */
    /* if this state reused, force set back to "curr" */
    tty_invalidate(sp);
#endif /* TTY_CLOSE_FREE not defined */
}

#ifdef SIGTSTP
void
tty_suspend()
{
    struct save *sp;
    int fd;

    fd = fileno(stdin);
    sp = find_by_fd(fd, FIND);
    if (sp)
	tty_set(fd, &sp->save);
    
    proc_suspend();

    /* here on process resume; */
    if (sp)
	tty_set(fd, &sp->curr);
}
#endif /* SIGTSTP defined */
