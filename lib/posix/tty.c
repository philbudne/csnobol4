/* $Id$ */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/*
 * tty mode, echo
 * POSIX.1 version
 */

static struct termios old;
static int lastcbreak, lastnoecho, lastfd;

void
tty_save()
{
    tcgetattr(STDIN_FILENO, &old);	/* XXX stdin??? */
    lastfd = -1;
}

void
tty_restore()
{
    tcsetattr(STDIN_FILENO, TCSADRAIN, &old); /* XXX stdin??? */
}

void
tty_mode( fp, cbreak, noecho )
    FILE *fp;
    int cbreak, noecho;
{
    struct termios new;
    int fd;

    fd = fileno(fp);
    if (fd == lastfd && cbreak == lastcbreak && noecho == lastnoecho)
	return;

#if 1
    /* what if not stdin (ie; some other tty)???
     * BUT last "raw" request could have crushed members shared by VMIN/VTIME
     */
    new = old;
#else
    tcgetattr(fd, &new);
#endif

    if (cbreak) {
#if 0
	new.c_lflag &= ~ICANON;		/* kill canon proc, leave signals */
#else
	new.c_lflag &= ~(ICANON|ISIG);	/* kill canon, signal processing */
#endif
	new.c_iflag &= ~(ICRNL|INLCR);	/* hey, system! leave CR/LF alone! */
	new.c_cc[VMIN] = 1;		/* want at least one */
	new.c_cc[VTIME] = 0;		/* no minimum */
    }
    else {
	/* XXX restore normality, what ever that was??? */
    }

    if (noecho)
	new.c_lflag &= ~ECHO;		/* kill echo */

    tcsetattr(fd, TCSADRAIN, &new);

    lastcbreak = cbreak;
    lastnoecho = noecho;
    lastfd = fd;
}
