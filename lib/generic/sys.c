/* $Id$ */

/* generic support for HOST() on systems with no uname(2) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifndef CONFIG_GUESS
#define CONFIG_GUESS ""
#endif /* CONFIG_GUESS not defined */

#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "lib.h"
#include "str.h"

void
hwname(cp)
    char *cp;
{
#ifdef HWNAME
    strcpy(cp, HWNAME);
#else  /* HWNAME not defined */
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
#endif /* HWNAME not defined */
}

void
osname(cp)
    char *cp;
{
#ifdef OSNAME
    strcpy(cp, OSNAME);
#else  /* OSNAME not defined */
    char *sp;

    /* find last hyphen */
    sp = rindex(CONFIG_GUESS, '-');
    if (sp)
	sp++;
    if (!sp || !*sp)			/* null or empty string */
	sp = "unknown";

    strcpy(cp, sp);
#endif /* OSNAME not defined */
}
