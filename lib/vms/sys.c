/* $Id$ */

/* support for HOST() on VMS */

#ifdef vax
#define HWNAME "VAX"
#define OSNAME "VMS"			/* see osname() below */
#else  /* vax not defined */
#ifdef __ALPHA
#define HWNAME "alpha"			/* lower case, like OSF/1 uname */
#define OSNAME "OpenVMS"
#else  /* __ALPHA not defined */
#define HWNAME "???"
#define OSNAME "VMS"
#endif /* __ALPHA not defined */
#endif /* vax not defined */

void
hwname(cp)
    char *cp;
{
    strcpy(cp, HWNAME);
}

void
osname(cp)
    char *cp;
{
    /* XXX get version (using $GETSYIW? or DCL F$GETSYI("VERSION")?? */
    /* C-Kermit does SYI$SYI_ARCH_NAME (4454) to test for "OpenVMS" on VAX */
    strcpy(cp, OSNAME);
}
