/* $Id$ */

/*
 * opendir/readdir module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 11/15/2013
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <dirent.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* free() */
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"			/* strlen, for RETSTR */

static handle_handle_t dir_handles;

/*
**=pea
**=sect NAME
**snobol4dirs \- filesystem directory interface for SNOBOL4
**=sect SYNOPSYS
**B<-INCLUDE 'dirs.sno'>
**=sect DESCRIPTION
**B<OPENDIR(>I<path>B<)> opens a directory and returns a handle.
**=cut
*/

/*
 * LOAD("OPENDIR(STRING)INTEGER", DIRS_DL)
 * Open a directory
 *
 * first arg:
 *	filename
 * return handle, or failure
 */
int
OPENDIR( LA_ALIST ) LA_DCL
{
    char *fname = mgetstring(LA_PTR(0));
    DIR *d = opendir(fname);
    snohandle_t h;

    free(fname);

    if (!d)
	RETFAIL;

    h = new_handle(&dir_handles, d);
    if (h == BAD_HANDLE) {
	closedir(d);
	RETFAIL;
    }
    RETINT(h);
}

/*
**=pea
**
**B<READDIR(>I<handle>B<)> returns a filename or fails.
**=cut
*/
/*
 * LOAD("READDIR(INTEGER)STRING", DIRS_DL)
 *
 * return name or failure
 */
int
READDIR( LA_ALIST ) LA_DCL
{
    DIR *d = lookup_handle(&dir_handles, LA_INT(0));
    struct dirent *dp;

    if (!d)
	RETFAIL;


    dp = readdir(d);
    if (!dp)
	RETFAIL;

    RETSTR(dp->d_name);
}

/*
**=pea
**
**B<REWINDDIR(>I<handle>B<)> rewinds a directory handle.
**=cut
*/
/*
 * LOAD("REWINDDIR(INTEGER)STRING", DIRS_DL)
 * returns: null string or failure
 */
int
REWINDDIR( LA_ALIST ) LA_DCL
{
    snohandle_t h = LA_INT(0);
    DIR *d = lookup_handle(&dir_handles, LA_INT(0));

    if (!d)
	RETFAIL;
    rewinddir(d);
    RETNULL;
}

#ifndef _WIN32 				/* should be HAVE_SEEKDIR */
/*
**=pea
**
**B<TELLDIR(>I<handle>B<)> reports directory handle position.
**(may not be available on all platforms).
**=cut
*/
/*
 * LOAD("TELLDIR(INTEGER)INTEGER", DIRS_DL)
 * returns: integer or failure
 */
int
TELLDIR( LA_ALIST ) LA_DCL
{
    snohandle_t h = LA_INT(0);
    DIR *d = lookup_handle(&dir_handles, LA_INT(0));

    if (!d)
	RETFAIL;
    RETINT(telldir(d));
}

/*
**=pea
**
**B<TELLDIR(>I<handle>B<)> adjusts directory handle position.
**=cut
*/
/*
 * LOAD("SEEKDIR(INTEGER,INTEGER)STRING", DIRS_DL)
 * returns: null string or failure
 */
int
SEEKDIR( LA_ALIST ) LA_DCL
{
    snohandle_t h = LA_INT(0);
    DIR *d = lookup_handle(&dir_handles, LA_INT(0));
    if (!d)
	RETFAIL;
    seekdir(d, LA_INT(1));
    RETNULL;
}
#endif

/*
**=pea
**
**B<CLOSEDIR(>I<handle>B<)> closes directory handle.
**=cut
*/
/*
 * LOAD("CLOSEDIR(INTEGER)STRING", DIRS_DL)
 * returns: null string or failure
 */
int
CLOSEDIR( LA_ALIST ) LA_DCL
{
    snohandle_t h = LA_INT(0);
    DIR *d = lookup_handle(&dir_handles, LA_INT(0));

    if (!d)
	RETFAIL;
    remove_handle(&dir_handles, h);
    if (closedir(d) < 0)
	RETFAIL;
    RETNULL;
}

/*
**=pea
**=sect SEE ALSO
**B<snobol4>(1), B<opendir>(3), B<readdir(>3B<), B<rewinddir>(3),
**B<telldir>(3), B<seekdir>(3), B<closedir>(3)
**=sect AUTHOR
**Philip L. Budne
**=cut
*/
