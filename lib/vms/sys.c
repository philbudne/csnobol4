/* $Id$ */

/* support for HOST() on VMS */

#ifdef vax
#define HWNAME "vax"
#else
/* safe, until other systems infected */
#define HWNAME "alpha"			/* lower case, like OSF/1 uname */
#endif

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
    strcpy(cp, "VMS");			/* XXX get version (using $GETSYIW?) */
}
