/* $Id$ */

/*
 * tty mode, echo
 * VMS version
 * started 9/17/97 -pb
 */

#include <stdio.h>
#include <errno.h>

#include <iodef.h>
#include <stsdef.h>

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)
#define SETERR(_STAT) do { vaxc$errno = (_STAT); errno = EVMSERR; } while(0)

struct read_iosb {
    short status;
    short size;				/* offset to terminator */
    short termlen;			/* terminator size */
    short term;				/* first char of terminator */
};

/* list of channels to use, by fileno */
struct ttychan {
    struct ttychan *next;
    int fd;
    int chan;
};
static struct ttychan *chans;

int
fisatty(f, fname)
    FILE *f;
    char *fname;
{
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
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
} /* tty_mode */

void
tty_save()
{
    tty_mode(stdin, 0, 0, 0);
}

void
tty_restore()
{
    tty_mode(stdin, 0, 0, 0);
}

/* advisory notice */
void
tty_close(f)
    FILE *f;
{
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
 * not called for cooked (non-raw) reads
 */

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
	if (iosb.termlen == 0) {
	    /* XXX need to flush rest of line! */
	}
	if (keepeol)
	    return iosb.size + iosb.termlen;
    }
    return iosb.size;
} /* tty_read */

#ifdef TEST
#define TRUE 1
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
}
#endif /* TEST */

