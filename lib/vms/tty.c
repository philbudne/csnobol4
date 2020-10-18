/* $Id$ */

/*
 * tty mode, echo
 * VMS version
 * started 9/17/97 -pb
 *
 * 9/20/2020 (23y3d later!): perhaps could be implemented aa a ttyio_obj?
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* for malloc */

#ifdef HAVE_UNISTD_H			/* !! */
#include <unistd.h>			/* isatty() */
#endif /* HAVE_UNISTD_H defined */

#include <stdio.h>
#include <errno.h>

#include <iodef.h>
#include <stsdef.h>
#include <starlet.h>			/* syscall prototypes */

#include "h.h"
#include "snotypes.h"
#include "lib.h"
#include "str.h"

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)
#define SETERR(_STAT) do { vaxc$errno = (_STAT); errno = EVMSERR; } while(0)

struct read_iosb {
    short status;
    short size;				/* offset to terminator */
    short term;				/* first char of terminator */
    short termlen;			/* terminator size */
};

/* list of channels to use, by fileno */
struct ttychan {
    struct ttychan *next;
    int fd;
    int chan;
};
static struct ttychan *chans;

int
fisatty(FILE *f) {
    /* should only return true for files that tty_read() works with! */
    return isatty(fileno(f));
}

/*
 * Currently tty_mode is a noop, as it doesn't SEEM necessary to set
 * any mode bits (ie; PASSTHRU).  If it turns out otherwise, tty_mode
 * should open a channel, and do a mode SENSE and save the existing
 * settings in the ttychan struct.
 */

void
tty_mode(FILE *fp, int cbreak, int noecho, int recl) {
} /* tty_mode */

/* advisory notice */
void
tty_close(FILE *f) {
    register struct ttychan *tp, *pp;
    int fd;

    /* see if we have an open channel */
    fd = fileno(f);
    for (tp = chans, pp = NULL; tp; tp = tp->next, pp = tp) {
	if (tp->fd == fd)
	    break;
    }
    if (!tp)
	return;				/* nope */

    /* unlink */
    if (pp)
	pp->next = tp->next;
    else
	chans = tp->next;

    /* release channel */
    SYS$DASSGN(tp->chan);
    free(tp);
}


/*
 * perform tty reads;
 * must define TTY_READ_RAW for this to be called from io.c
 * also called for cooked reads if TTY_READ_COOKED defined
 */

int
tty_read(FILE *f, char *buf, int len,
	 int raw, int noecho, int keepeol, const char *fname) {
    int chan;
    int fd;
    int op;				/* QIO op & flags */
    int status;				/* QIO status */
    int term[2], *termp;
    struct ttychan *tp;
    struct read_iosb iosb;		/* QIO io status block */

    /* see if we have an open channel */
    fd = fileno(f);
    for (tp = chans; tp; tp = tp->next) {
	if (tp->fd == fd)
	    break;
    }
    
    if (tp == NULL) {			/* no channel; open one */
	char namebuf[1024];
	struct descr {
	    int len;
	    char *ptr;
	} d;
	int sts;
	
	tp = (struct ttychan *)malloc(sizeof(struct ttychan));
	if (!tp)
	    return -1;

	d.ptr = fgetname(f, namebuf, 1); /* 1 forces VMS filename */
	if (!d.ptr)
	    return -1;
	d.len = strlen(d.ptr);
	status = SYS$ASSIGN(&d, &chan, 0, 0);
	if (!SUCCESS(status)) {
	    SETERR(status);
	    free(tp);
	    return -1;
	}
	
	tp->fd = fd;
	tp->chan = chan;

	/* link into list */
	tp->next = chans;
	chans = tp;
    }

    chan = tp->chan;
    op = IO$_READVBLK;
    if (noecho)
	op |= IO$M_NOECHO | IO$M_TRMNOECHO;

    if (raw) {
	op |= IO$M_NOFILTR;		/* no edit processing */
	term[0] = term[1] = 0;		/* no break chars */
	termp = term;
    }
    else
	termp = NULL;			/* default break chars */

    status = SYS$QIOW(0, chan, op, &iosb, 0, 0, buf, len, 0, termp, 0, 0);
    if (!SUCCESS(status)) {
	SETERR(status);
	return -1;
    }
    if (!SUCCESS(iosb.status)) {
	SETERR(iosb.status);
	return -1;
    }

    if (!raw) {
	/*
	 * XXX if termlen == 0, overran buffer
	 * need to flush rest of record!
	 */
	if (keepeol)
	    return iosb.size + iosb.termlen;
    }
    return iosb.size;
} /* tty_read */

void
tty_suspend(void)
{
} /* tty_suspend */

#ifdef TEST
#define TRUE 1
int
main() {
  char buf[3];
  int cc;

  printf("raw, noecho:\n");
  cc = tty_read(stdin, buf, sizeof(buf), TRUE, TRUE, FALSE, "-");
  if (cc) printf("%d\n", buf[0]);

  printf("raw, echo:\n");
  cc = tty_read(stdin, buf, sizeof(buf), TRUE, FALSE, FALSE, "-");
  if (cc) printf("%d\n", buf[0]);

  printf("cooked, echo:\n");
  cc = tty_read(stdin, buf, sizeof(buf), FALSE, FALSE, TRUE, "-");
  if (cc) printf("%d\n", buf[0]);

  printf("cooked, noecho:\n");
  cc = tty_read(stdin, buf, sizeof(buf), FALSE, TRUE, TRUE, "-");
  if (cc) printf("%d\n", buf[0]);

  tty_close(stdin);
  return 0;
}
#endif /* TEST defined */
