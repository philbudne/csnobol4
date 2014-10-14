/* $Id$ */

/*
 * ndbm database module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 9/2/2004
 *
 * "ndbm" is a database API created in 4.3BSD
 * and included in Version 2 of the "Single Unix Specification" (SUSv2)
 *
 * Several different packages implement this API, including:
 *
 * * Original BSD 4.3 ndbm, based on AT&T dbm
 *	found in commercial Un*x offerings
 * * Berkeley DB v1 compatibility interface
 *	supplied with many 4.4BSD based systems (Free|Open|Net)BSD, MacOS X
 * * GNU DBM (GDBM)
 *	found in Linux distributions
 * * SDBM, Ozan Yigit's Public Domain implementation of NDBM
 *	supplied in this distribution
 *
 * None of the above allow concurrent read & write, and only GDBM
 * provides locking to eliminate the possibility of file corruption.
 *
 * DBM databases were traditionally (re)created from text files and
 * used for fast disk-based read-only table lookups.  Programs which
 * need to update the file generate a new copy, and rename the new
 * file(s) in-place, so that the next reader gets the new copies
 * (existing readers continue to see the old data).
 *
 * File names:
 *
 * NDBM, GDBM, SDBM:
 *	filename.dir, filename.pag
 *
 * Berkeley DB:
 *	filename.db
 *
 * NDBM and SDBM place restrictions on the total size of key & datum
 * (often around 1K).
 */

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
 * LOAD("DBM_OPEN(STRING,STRING,STRING)INTEGER", NDBM_DL)
 * Open or create an indexed data file
 *
 * first arg:
 *	filename
 * second arg:
 *	empty:	readonly
 *	"W":	read/write
 *	"C":	create
 * third arg:
 *	file "mode" (default 0660)
 *	XXX take [ugo][rwx]*
 *
 * return handle, or failure
 */
int
DBM_OPEN( LA_ALIST ) LA_DCL
{
    snohandle_t h;
    char base[1024];		/* XXX */
    char modestr[1024];		/* XXX */
    const char *cp;
    char *ep;
    int i;
    int flags;
    int wr, create;
    int mode;
    DBM *f;

    getstring(LA_PTR(0), base, sizeof(base));
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
	mode = 0660;
    }

    f = dbm_open(base, flags, mode);
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
