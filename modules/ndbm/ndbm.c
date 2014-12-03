/* $Id$ */

/*
 * ndbm database module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 9/2/2004
 */

/*
**=pea
**=sect NAME
**snobol4dbm \- SNOBOL4 NDBM interface
**
**=sect SYNOPSIS
**=code
**-INCLUDE 'ndbm.sno'
**        dbhandle = DBM_OPEN(file,flags,mode)
**        DBM_CLOSE(dbhandle)
**        DBM_STORE(dbhandle,key,datum,flags)
**        datum = DBM_FETCH(dbhandle,key)
**        DBM_DELETE(dbhandle,key)
**        key = DBM_FIRSTKEY(dbhandle)
**        key = DBM_NEXTKEY(dbhandle)
**        DBM_ERROR(dbhandle)
**        DBM_CLEARERR(dbhandle)
**=ecode
**
**=sect DESCRIPTION
**"NDBM" (for New Data Base Management) is an industry standard fast
**hashed storage API first created in 4.3BSD, and included in the Unix
**98 (SUSv2) standard.  The original DBM API appeared in AT&T Research
**Unix Version 7, and only allowed access to a single file at a time.
**
**There are many different implementations of this API, including:
**
**=bull
**The original BSD 4.3 ndbm, based on AT&T dbm. Found in commercial Un*x offerings.
**
**=bull
**Berkeley DB v1 compatibility interface.
**Supplied with 4.4BSD based systems: (Free|Open|Net)BSD, MacOS X.
**
**=bull
**GNU DBM (GDBM) found in Linux distributions
**(may require a DBM compatibility package to be installed).
**
**=bull
**SDBM, Ozan Yigit's Public Domain implementation of NDBM.
**Supplied with this distribution, and used as a last resort
**on Un*x systems (and by default on non Un*x systems).
**=cut
**/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H                    /* before stdio, h.h */
#include <stdlib.h>                     /* for strtol() */
#endif

#include <fcntl.h>

/* only one will be set: */
#ifdef HAVE_NDBM_H
/*
 * may be AT&T/BSD or Berkeley DB v1 compat (*BSD, Darwin)
 * for NDBM compat w/ Berkeley DB v2+:
 * #define DB_DBM_H
 * #include <db.h>
 */
#include <ndbm.h>			
#endif /* HAVE NDBM_H */
#ifdef HAVE_DB_H
#include <db.h>
#endif /* HAVE_DB_H defined */
#ifdef HAVE_GDBM_SLASH_NDBM_H
#include <gdbm/ndbm.h>
#endif /* HAVE_GDBM_SLASH_NDBM_H defined */
#ifdef HAVE_GDBM_DASH_NDBM_H
#include <gdbm-ndbm.h>
#endif /* HAVE_GDBM_DASH_NDBM_H defined */
#ifdef HAVE_SDBM_H
#include <sdbm.h>
#endif /* HAVE_SDBM_H defined */

/*
 * On glibc-2.1 systems, including Redhat 6.1, to use Berkeley v1:
 * #include <db1/ndbm.h>
 * link w/ -ldb1
 */

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"

static handle_handle_t dbm_files;

/*
**=pea
**=item B<DBM_OPEN(>I<filename>,I<flags>,I<mode>)
**takes a filename (STRING), flags (either "R" for read-only, "W" for
**write access, or "CW" to create and write a new file), and a "mode"
**string, which defaults to "0666" (octal) and returns a database
**handle which can be passed to the remaining functions.
**=cut
*/

/*
 * LOAD("DBM_OPEN(STRING,STRING,STRING)INTEGER", NDBM_DL)
 * Open or create an indexed data file
 * return handle, or failure
 */
int
DBM_OPEN( LA_ALIST ) LA_DCL
{
    snohandle_t h;
    char *base;
    char modestr[1024];
    const char *cp;
    char *ep;
    int i;
    int flags;
    int wr, create;
    int mode;
    DBM *f;

    cp = LA_STR_PTR(1);
    i = LA_STR_LEN(1);
    getstring(LA_PTR(2), modestr, sizeof(modestr));

    wr = create = 0;
    while (i-- > 0) {
	switch (*cp++) {
	case 'r': case 'R': break;
	case 'w': case 'W': wr = 1; break;
	case 'c': case 'C': create = 1; break;
/* XXX add flag for locking? */
	default: RETFAIL;
	}
    }

    if (wr) {
	flags = O_RDWR;
	if (create)
	    flags |= O_CREAT;
    }
    else
	flags = O_RDONLY;


    /* XXX take symbolic mode! */
    if ((mode = strtol(modestr, &ep, 0)) == 0) {
	if (*ep != '\0')
	    RETFAIL;
	mode = 0666;
    }

    base = mgetstring(LA_PTR(0));
    f = dbm_open(base, flags, mode);
    free(base);
    if (!f)
	RETFAIL;

    h = new_handle(&dbm_files, f);
    if (h == BAD_HANDLE) {
	dbm_close(f);
	RETFAIL;
    }
    RETINT(h);
}

/*
**=pea
**=item B<DBM_CLOSE>
**closes the database file. B<DBM_CLOSE> I<MUST>
**be called to ensure that all your data is written.
**=cut
*/

/*
 * LOAD("DBM_CLOSE(INTEGER)STRING", NDBM_DL)
 *
 * return null string or failure
 */
int
DBM_CLOSE( LA_ALIST ) LA_DCL
{
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (!f)
	RETFAIL;

    remove_handle(&dbm_files, LA_INT(0));
    dbm_close(f);
    RETNULL;
}

/*
**=pea
**=item B<DBM_STORE>
**takes a database handle, key and datum strings, and a flag (either
**B<DBM_INSERT> to insert a new pair, or fail if the key already exists,
**or B<DBM_REPLACE> to insert or replace existing data).  The key and
**datum strings may contain an arbitrary series of characters.
**=cut
*/

/*
 * LOAD("DBM_STORE(INTEGER,STRING,STRING,INTEGER)INTEGER", NDBM_DL)
 *
 * arg 1:	file handle
 * arg 2:	key
 * arg 3:	datum
 * arg 4:	non-zero to replace
 *
 * returns:
 * 0	success
 * 1	key exists
 *
 */
int
DBM_STORE( LA_ALIST ) LA_DCL
{
    datum key, data;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    int flags;
    int ret;

    if (!f)
	RETFAIL;

    key.dptr = LA_STR_PTR(1);
    key.dsize = LA_STR_LEN(1);

    data.dptr = LA_STR_PTR(2);
    data.dsize = LA_STR_LEN(2);

    if (LA_INT(3))
	flags = DBM_REPLACE;
    else
	flags = DBM_INSERT;

    ret = dbm_store(f, key, data, flags);
    if (ret < 0)
	RETFAIL;
    RETINT(ret);
}

/*
**=pea
**=item B<DBM_FETCH>
**returns the stored datum (if any) for the supplied key, or fails.
**=cut
*/
/*
 * LOAD("DBM_FETCH(INTEGER,STRING)STRING", NDBM_DL)
 *
 * arg 1:	file handle
 * arg 2:	key
 *
 * returns:
 * datum or failure
 */
int
DBM_FETCH( LA_ALIST ) LA_DCL
{
    datum key, data;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (!f)
	RETFAIL;

    key.dptr = LA_STR_PTR(1);
    key.dsize = LA_STR_LEN(1);
    data = dbm_fetch(f, key);
    if (!data.dptr)
	RETFAIL;

    RETSTR2(data.dptr, data.dsize);
}

/*
**=pea
**=item B<DBM_DELETE>
**deletes the stored datum (if any) for the supplied key, or fails.
**=cut
*/
/*
 * LOAD("DBM_DELETE(INTEGER,STRING)INTEGER", NDBM_DL)
 *
 * arg 1:	file handle
 * arg 2:	key
 *
 * returns:
 * 0	success
 * 1	key did not exist
 * failure: error
 */
int
DBM_DELETE( LA_ALIST ) LA_DCL
{
    datum key;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    int ret;

    if (!f)
	RETFAIL;

    key.dptr = LA_STR_PTR(1);
    key.dsize = LA_STR_LEN(1);

    ret = dbm_delete(f, key);
    if (ret < 0)
	RETFAIL;
    RETINT(ret);
}

/*
**=pea
**=item B<DBM_FIRSTKEY>
**and subsequent calls to B<DBM_NEXTKEY>
**allow you to traverse all stored keys.  The keys will be returned in
**arbitrary order, and the routines will fail at the end of the
**traversal.
**=cut
*/
/*
 * LOAD("DBM_FIRSTKEY(INTEGER)STRING", NDBM_DL)
 *
 * arg 1:	file handle
 *
 * returns:
 * datum or failure
 */
int
DBM_FIRSTKEY( LA_ALIST ) LA_DCL
{
    datum key;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (!f)
	RETFAIL;

    key = dbm_firstkey(f);
    if (!key.dptr)
	RETFAIL;

    RETSTR2(key.dptr, key.dsize);
}

/*
 * LOAD("DBM_NEXTKEY(INTEGER)STRING", NDBM_DL)
 *
 * arg 1:	file handle
 *
 * returns:
 * data or failure
 */
int
DBM_NEXTKEY( LA_ALIST ) LA_DCL
{
    datum key;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (!f)
	RETFAIL;

    key = dbm_nextkey(f);
    if (!key.dptr)
	RETFAIL;

    RETSTR2(key.dptr, key.dsize);
}

/*
**=pea
**=item B<DBM_ERROR>
**is a predicate which succeeds if the database handle is valid
**and an I/O error has occurred on the file.
**=cut
*/
/*
 * LOAD("DBM_ERROR(INTEGER)STRING", NDBM_DL)
 *
 * predicate
 */

int
DBM_ERROR( LA_ALIST ) LA_DCL
{
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (f && dbm_error(f))
	RETNULL;
    RETFAIL;				/* fails if no error! */
}

/*
**=pea
**=item B<DBM_CLEARERR>
**is a predicate which succeeds if if the database handle is valid, and has
**the side effect of clearing the I/O error flag.
**=cut
*/
/*
 * LOAD("DBM_CLEARERR(INTEGER)STRING", NDBM_DL)
 * predicate
 */

int
DBM_CLEARERR( LA_ALIST ) LA_DCL
{
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    if (!f)
	RETFAIL;
    dbm_clearerr(f);
    RETNULL;
}

/*
**=pea
**=sect FILES
**NDBM, GDBM, and SDBM create two files: I<filename>.dir and I<filename>.pag.
**Berkeley DB creates a single I<filename>.db file.
**
**=sect SEE ALSO
**B<snobol4sqlite3dbm>(3),
**B<ndbm>(3),
**B<dbopen>(3),
**B<gdbm>(3).
**
**=sect AUTHOR
**Philip L. Budne
**
**=sect BUGS
**None of the implementations allow concurrent read and write. The
**B<snobol4sqlite3dbm>(3) interface achives this portably using
**B<sqlite>(1).
**
**Some implementations (classic NDBM and SDBM) place limits on the total
**size of key plus datum (typically slightly less than 1KB).
**
**NOTE: Some implementations (classic NDBM and SDBM) create sparse file
**which appear (to "ls -l") to be larger than they are (see "ls -s").
**Copying such files may cause the non-allocated blocks to be "filled"
**with zeroed disk blocks, and then the files really will be large!
**
**Only GDBM provides locking to eliminate the possibility of file corruption,
**or reading of incomplete data.
**
**GDBM locking sometimes fails on NFS mounted partitions (particularly on
**Linux systems).  GDBM's NDBM interface does not provide a way to disable
**locking.
**
**DBM databases accessed concurrently by multiple processes are
**traditionally (re)created from text files and used for fast disk-based
**read-only table lookups.  Programs which need to update the file
**generate a new temporary copy using a different name, and then rename
**the new file(s), so that the next reader gets the new copies (existing
**readers continue to see old data).
**=cut
*/
