/* $Id$ */

/* generic support for HOST() on systems with no uname(2) */

#ifndef CONFIG_GUESS
#define CONFIG_GUESS ""
#endif

#ifdef ANSI_STRINGS
#include <strings.h>
#else
#include <string.h>
#define rindex(S,C) strrchr(S,C)	/* XXX move to h.h? */
#endif

void
hwname(cp)
    char *cp;
{
#ifdef HWNAME
    strcpy(cp, HWNAME);
#else
    char *sp, *tp;
    int sl;

    sp = CONFIG_GUESS;
    sl = 0;
    tp = sp;
    while (*tp && *tp++ != '-')
	sl++;

    if (sl == 0) {			/* no hyphen found */
	strcpy(cp, "unknown");		/* default value */
	return;
    }
    strncpy(cp, sp, sl);
    cp[sl] = '\0';
#endif
}

void
osname(cp)
    char *cp;
{
#ifdef OSNAME
    strcpy(cp, OSNAME);
#else
    char *sp;

    /* find last hyphen */
    sp = rindex(CONFIG_GUESS, '-');
    if (sp)
	sp++;
    if (!sp || !*sp)			/* null or empty string */
	sp = "unknown";

    strcpy(cp, sp);
#endif
}
