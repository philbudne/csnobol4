/* $Id$ */

/* support for HOST() on VMS */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <syidef.h>

#ifndef SYI$_ARCH_NAME
#define SYI$_ARCH_NAME 4454
#endif /* SYI$_ARCH_NAME not defined */

#ifdef vax
#define HWNAME "VAX"
#else  /* vax not defined */
#ifdef __ALPHA
#define HWNAME "alpha"			/* lower case, like OSF/1 uname */
#else  /* __ALPHA not defined */
#define HWNAME "???"
#endif /* __ALPHA not defined */
#endif /* vax not defined */

struct item {
    short buflen;			/* result buffer length */
    short code;				/* item code */
    char *buf;				/* result buffer */
    long *len;				/* result length */
    long zero;
};

#define MAXSTR 32
static char _hwname[MAXSTR];
static char _osname[MAXSTR+1+MAXSTR];

static void
sys_init() {
    static int inited;
    struct item item;
    char temp[MAXSTR];
    long len;
    char *os, *hw;

    if (inited)
	return;

    item.code = SYI$_ARCH_NAME;		/* "hw_arch" */
    item.buflen = sizeof(temp) - 1;
    item.buf = temp;
    item.len = &len;
    item.zero = 0;

    hw = HWNAME;
    if ((SYS$GETSYIW(0, 0, 0, &item, 0, 0, 0) & 1) == 1) {
	os = "OpenVMS";
	temp[len] = '\0';
	hw = temp;
    }
    else
	os = "VMS";
    strcpy(_hwname, hw);

    item.code = SYI$_VERSION;
    item.buflen = sizeof(temp) - 1;
    item.buf = temp;
    item.len = &len;
    item.zero = 0;
    if ((SYS$GETSYIW(0, 0, 0, &item, 0, 0, 0) & 1) == 1) {
	/* trim trailing spaces? */
	while (len > 0 && temp[len-1] == ' ')
	    len--;
	temp[len] = '\0';
	sprintf(_osname, "%s %s", os, temp);
    }
    else
	strcpy(_osname, os);
}

void
hwname(cp)
    char *cp;
{
    sys_init();
    strcpy(cp, _hwname);
}

void
osname(cp)
    char *cp;
{
    sys_init();
    strcpy(cp, _osname);
    puts("osname() returning");		/* blows up on VAX if this not here? */
}

#ifdef TEST
main() {
    sys_init();

    printf("OS: %s\n", _osname);
    printf("HW: %s\n", _hwname);
}
#endif
