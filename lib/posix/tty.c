/* $Id$ */

/*
 * tty mode, echo
 * POSIX.1 version
 */

/*
 * Tries to detect multiple fd's on same device (e.g. stdin, stderr)
 * using fstat() and comparing rdev, but /dev/tty (which could
 * indirectly refer to the same device open on stdin) slips by, and
 * gets entered separately.
 *
 * Could poke distinctive values into c_cc array on known fd's
 * and see if they show up on /dev/tty fd to detect collision.
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
#else  /* USE_TERMIO not defined */
#include <termios.h>
#endif /* USE_TERMIO not defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else  /* HAVE_UNISTD_H not defined */
#define STDIN_FILENO 0
#endif /* HAVE_UNISTD_H not defined */

#include <signal.h>

#include <sys/types.h>			/* for sys/stat.h */
#include <sys/stat.h>

/*
 * keep settings for each device in a list (by dev_t rather than fd
 * since multiple fd's can be open to same device (stdin/out/err))
 */
static struct save {
    struct save *next;
    dev_t dev;
    struct termios save, curr;
    int cbreak, noecho;
    int recl;
} *list;

enum action { FIND, CREATE, REMOVE };

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
    return isatty(fileno(f));
}

/* invalid last mode; insure next use sets to "curr" state */
static void
tty_invalidate(sp)
    struct save *sp;
{
    sp->noecho = sp->cbreak = sp->recl = -1;
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
    tcgetattr(fd, &sp->save);		/* save settings */
    tty_invalidate(sp);
    
    /* link into list */
    sp->next = list;
    list = sp;

    return sp;
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
    if (sp->dev == last && cbreak == sp->cbreak && noecho == sp->noecho &&
	(!cbreak || recl == sp->recl))
	return;				/* nothing to do! */

    fflush(fp);				/* flush pending output */
    
    sp->curr = sp->save;		/* start with original */
    if (cbreak) {
	sp->curr.c_lflag &= ~ICANON;	/* kill canonical processing */
	sp->curr.c_iflag &= ~(ICRNL|INLCR); /* hey, system! leave CR/LF alone! */
#ifdef TTY_RAW
	sp->curr.c_lflag &= ~ISIG;	/* kill signal processing */
	sp->curr.c_iflag &= ~(IXON|IXOFF); /* pass CTRLS/CTRLQ */
	sp->curr.c_oflag &= ~OPOST;	/* kill output post-processing */
	/* XXX set CS8, PASS8, IGNPAR, clear ISTRIP? */
	/* XXX clear IUCLC, XCASE (if they exist)? */
#endif /* TTY_RAW defined */
	
	if (recl > 0xff)		/* VMIN is a char! */
	    recl = 0xff;
	
	sp->curr.c_cc[VMIN] = recl;	/* number of chars wanted */
	sp->curr.c_cc[VTIME] = 0;	/* wait as long as we have to */
    }
    
    if (noecho)
	sp->curr.c_lflag &= ~ECHO;		/* kill echo */
    
    tcsetattr(fd, TCSADRAIN, &sp->curr);
    
    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
    sp->recl = recl;
    last = sp->dev;
}

/* advisory notice (does not perform close) */
void
tty_close(f)
    FILE *f;
{
    struct save *sp;
    int fd;

    fd = fileno(f);

#ifdef TTY_CLOSE_FREE
    sp = find_by_fd(fd, REMOVE);
#else
    /* try keeping information! */
    sp = find_by_fd(fd, FIND);
#endif
    if (!sp)
	return;				/* not found, bad fd, bad device */
    
    tcsetattr(fd, TCSADRAIN, &sp->save);
    
#ifdef TTY_CLOSE_FREE
    free(sp);
#else
    tty_invalidate(sp);
#endif
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
	tcsetattr(fd, TCSADRAIN, &sp->save);

    proc_suspend();

    /* here on process resume; */
    if (sp)
	tcsetattr(fd, TCSADRAIN, &sp->curr);
}
#endif /* SIGTSTP defined */
