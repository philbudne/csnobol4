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
#define HWNAME "Alpha"
#else  /* __ALPHA not defined */
#define HWNAME "???"
#endif /* __ALPHA not defined */
#endif /* vax not defined */

/* shouldn't this come from some include file?? */
struct item {
    short buflen;			/* result buffer length */
    short code;				/* item code */
    char *buf;				/* result buffer */
    long *len;				/* result length (short *?) */
};

#define MAXSTR 32
static char _hwname[MAXSTR];
static char _osname[MAXSTR+1+MAXSTR];

static void
sys_init() {
    static int inited;
    struct item items[2];
    char temp[MAXSTR];
    long len;
    char *os, *hw;

    if (inited)
	return;

    items[0].code = SYI$_ARCH_NAME;		/* "hw_arch" */
    items[0].buflen = sizeof(temp) - 1;
    items[0].buf = temp;
    items[0].len = &len;
    items[1].code = items[1].buflen = 0;

    hw = HWNAME;
    len = 0;				/* paranoia */
    if ((SYS$GETSYIW(0, 0, 0, items, 0, 0, 0) & 1) == 1) {
	os = "OpenVMS";
	temp[len] = '\0';
	hw = temp;
    }
    else
	os = "VMS";
    strcpy(_hwname, hw);

    items[0].code = SYI$_VERSION;
    items[0].buflen = sizeof(temp) - 1;
    items[0].buf = temp;
    items[0].len = &len;
    items[1].code = items[1].buflen = 0;

    len = 0;				/* paranoia */
    if ((SYS$GETSYIW(0, 0, 0, items, 0, 0, 0) & 1) == 1) {
	/* trim trailing spaces? */
	while (len > 0 && temp[len-1] == ' ')
	    len--;
	temp[len] = '\0';
	/* remove leading "V"?? */
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
}

#ifdef TEST
main() {
    sys_init();

    printf("OS: %s\n", _osname);
    printf("HW: %s\n", _hwname);
}
#endif
