/* $Id$ */

#include <termios.h>

/* lib/posix/pty.c: */
extern int forkpty(int *amaster, char *name, struct termios *termp, struct winsize *winp);
