/* $Id$ */

/*
 * tty mode, echo
 * VMS version
 * started 9/17/97 -pb
 */

#include <stdio.h>
#include <iodef.h>
#include <ssdef.h>

#define SETERR(_STAT) do { vaxc$errno = (_STAT); errno = EVMSERR; } while(0)

static struct {
    short status;
    short size;
    short termlen;
    short term;
} iosb;

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
}

/* must define TTY_READ for this to be called; */

int
tty_read(f, buf, len, raw, noecho)
    FILE *f;
    char *buf;
    int len;
    int raw;
    int noecho;
{
#if 0
    if (raw) {
	int chan;
	int op;
	int status;

/* XXX UGH; TEMP for testing */
	struct descr {
	  int len;
	  char *ptr;
	} d;

	d.len = sizeof("SYS$COMMAND")-1;
	d.ptr = "SYS$COMMAND";
	SYS$ASSIGN(&d, &chan, 0, 0);
/* XXX END OF TEMP CODE */

	op = IO$_READVBLK;
	if (noecho)
	    op |= IO$M_NOECHO;
	status = SYS$QIOW(0, chan, op, &iosb, 0, 0, buf, len, 0, 0, 0, 0);
	if (status != SS$_NORMAL) {
	    SETERR(status);
	    return -1;
	}
	if (iosb.status != SS$_NORMAL) {
	    SETERR(iosb.status);
	    return -1;
	}
	return iosb.size;
    }
    else
#endif
    {
	if (noecho)
	    return -1;			/* don't echo passwords!! */
	return fread(buf, 1, len, f);
    }
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

