/* $Id$ */

/*
 * tty mode, echo
 * VMS version
 * started 9/17/97 -pb
 */

#include <stdio.h>
#include <errno.h>

#include <iodef.h>
#include <ssdef.h>

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)
#define SETERR(_STAT) do { vaxc$errno = (_STAT); errno = EVMSERR; } while(0)

extern char *stdin_file, *stdout_file;	/* from getredirect.c */
extern char *term_file;			/* from term.c */
extern FILE *term_fd;			/* from term.c */

static struct {
    short status;
    short size;
    short termlen;
    short term;
} iosb;

static struct ttychan {
    struct ttychan *next;
    FILE *f;
    int chan;
} *chans;

int
fisatty(f)
    FILE *f;
{
    /* should only return true for files that tty_read() works with! */
    return isatty(fileno(f));
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
} /* tty_mode */

void
tty_save()
{
}

void
tty_restore()
{
}

/* advisory notice */
void
tty_close(f)
    FILE *f;
{
    register struct ttychan *tp, *pp;

    /* see if we have an open channel */
    for (tp = chans, pp = NULL; tp; tp = tp->next, pp = tp) {
	if (tp->f == f)
	    break;
    }
    if (!tp)
	return;				/* nope */

    /* unlink */
    if (pp)
	pp->next = tp->next;
    else
	chans = tp->next;

    SYS$DASSGN(tp->chan);
    free(tp);
}


/* binary read; must define TTY_READ for this to be called; */
int
tty_read(f, buf, len, noecho, fname)
    FILE *f;
    char *buf;
    int len;
    int noecho;
    char *fname;
{
    int chan;
    int op;
    int status;
    int term[2];
    struct ttychan *tp;
    
    /* see if we have an open channel */
    for (tp = chans; tp; tp = tp->next) {
	if (tp->f == f)
	    break;
    }
    
    if (tp == NULL) {
	struct descr {
	    int len;
	    char *ptr;
	} d;
	int sts;
	
	tp = (struct ttychan *)malloc(sizeof(struct ttychan));
	if (!tp)
	    return -1;
	
	if (f == stdin)
	    d.ptr = stdin_file;
	else if (f == stdout)
	    d.ptr = stdout_file;
	else if (f == term_fd)
	    d.ptr = term_file;
	else
	    d.ptr = fname;
	
	d.len = strlem(d.ptr);
	status = SYS$ASSIGN(&d, &chan, 0, 0);
	if (!(status & STS$M_SUCCESS)) {
	    SETERR(status);
	    free(tp);
	    return -1;
	}
	
	tp->chan = chan;
	tp->f = f;
	tp->next = chans;
	chans = tp;
    }
    chan = tp->chan;
    op = IO$_READVBLK | IO$_NOFILTR;	/* read; no edit chars */
    if (noecho)
	op |= IO$M_NOECHO;
    term[0] = term[1] = 0;		/* no break chars */
    status = SYS$QIOW(0, chan, op, &iosb, 0, 0, buf, len, 0, term, 0, 0);
    if (!SUCCESS(status)) {
	SETERR(status);
	return -1;
    }
    if (!SUCCESS(iosb.status)) {
	SETERR(iosb.status);
	return -1;
    }
    return iosb.size;
}

#ifdef TEST
#define TRUE 1
main() {
  char buf[2];
  int cc;

  cc = tty_read(stdin, buf, 1, TRUE, TRUE);
  if (cc) printf("%d\n", buf[0]);
  cc = tty_read(stdin, buf, 1, TRUE, FALSE);
  if (cc) printf("%d\n", buf[0]);
  tty_restore();
}
#endif

