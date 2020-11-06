/* $Id$ */

/*
 * find libdir
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* for lib.h */
#include <stdlib.h>			/* free */
#ifdef HAVE_DIRNAME
#include <libgen.h>			/* XPG4.2 on mingw!! */
#endif

#include "h.h"
#include "snotypes.h"
#include "lib.h"			/* prototype, strjoin */
#include "str.h"			/* strlen(), strdup() */

void
find_snolib_directory(const char *av0, char **vdirp, char **vlibp) {
    if (vlibp && isdir(*vlibp))
	return;

    /* C:\Program Files\SNOBOL4\x.y\bin\snobol4.exe */
    char *av0dup = strdup(av0);

    /* C:\Program Files\SNOBOL4\x.y\bin */
    char *vbindir = dirname(av0dup);

    /* C:\Program Files\SNOBOL4\x.y */
    char *vdir = dirname(vbindir);

    char *vlibdir = strjoin(vdir, DIR_SEP, "lib", NULL);
    if (isdir(vlibdir)) {
	if (vdirp) {
	    if (*vdirp)
		free(*vdirp);
	    *vdirp = strdup(vdir);
	}
	if (vlibp) {
	    if (*vlibp)
		free(*vlibp);
	    *vlibp = strdup(vlibdir);
	}
    }
    else {
	/* XXX TEMP? will complain during build */
	fprintf(stderr, "No library directory at %s\n", vlibdir);
    }
    free(av0dup);
}
