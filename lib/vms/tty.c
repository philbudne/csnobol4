/* $Id$ */

#include <stdio.h>
#include <descrip.h>
#include <ttdef.h>
/*#include <tt2def.h>*/
#include <iodef.h>
#include <ssdef.h>

/*
 * tty mode, echo
 * VMS version
 * started 9/17/97 -pb
 * compiles, does not work?
 */

struct ttysense {
    char class;
    char type;
    short width;
    unsigned stat : 24;
    unsigned char len;
    unsigned int stat2;
};

static struct {
    short status;
    short offset;
    short termlen;
    short term;
} iosb;

/* keep settings for each fd in a list; */
static struct save {
    struct save *next;
    int fd;
    struct ttysense t;
    int cbreak, noecho;
} *list;

int
fisatty(f)
    FILE *f;
{
    return isatty(fileno(f));
}

void
tty_mode( fp, cbreak, noecho, recl )
    FILE *fp;
    int cbreak, noecho, recl;
{
    struct ttysense new;
    struct save *sp;
    int fd;

    fd = fileno(fp);			/* XXX this right?? */

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
    SYS$QIOW (0, fd, IO$_SENSEMODE, &iosb, 0, 0,
	      &sp->t, sizeof(sp->t), 0, 0, 0, 0 );
    sp->noecho = sp->cbreak = 0;	/* ??? */

    /* link into list */
    sp->next = list;
    list = sp;
 found:
    cbreak = !!cbreak;
    noecho = !!noecho;
    if (cbreak == sp->cbreak && noecho == sp->noecho)
	return;				/* nothing to do! */

    fflush(fp);				/* flush pending output */

    new = sp->t;			/* start with original */
    if (cbreak) {
	/* XXX
	 * setup new for "cbreak"
	 * no CR/LF processing
	 */
    }

    if (noecho)
	new.stat |= TT$M_NOECHO;	/* kill echo */

    SYS$QIOW(0, fd, IO$_SETMODE, &iosb, 0, 0, &new, sizeof(new), 0, 0, 0, 0);

    /* save current state */
    sp->cbreak = cbreak;
    sp->noecho = noecho;
} /* tty_mode */

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

#ifdef TEST
main() {
  char c;
  int cc;

  tty_save();
  tty_mode(stdin, 1, 1, 1);
  cc = read(fileno(stdin), &c, 1);
  if (cc) printf("%d\n", c);
  tty_restore();
}
#endif

