/* $Id$ */

/*
 * ndbm database module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 9/2/2004
 *
 * "ndbm" is a database API created in 4.3BSD
 * and included in Version 2 of the "Single Unix Specification" (SUSv2)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <fcntl.h>
#include <ndbm.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"

static struct handle_list dbm_files;

/*
 * LOAD("DBM_OPEN(STRING,STRING,INTEGER)INTEGER")
 * Create and initialize a TCL interpreter
 *
 * first arg:
 *	filename
 * second arg:
 *	empty:	readonly
 *	"W":	read/write
 *	"C":	create
 * third arg:
 *	file "mode" (default 0660)
 *
 * return handle, or failure
 */
int
DBM_OPEN( LA_ALIST ) LA_DCL
{
    snohandle_t h;
    char base[1024];		/* XXX */
    const char *cp;
    int i;
    int flags;
    int wr, create;
    int mode;
    DBM *f;

    getstring(LA_PTR(0), base, sizeof(base));
    cp = LA_STR_PTR(1);
    i = LA_STR_LEN(1);
    mode = LA_INT(2);

    wr = create = 0;
    while (i-- > 0) {
	switch (*cp++) {
	case 'r': case 'R': break;
	case 'w': case 'W': wr = 1; break;
	case 'c': case 'C': create = 1; break;
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


    if (mode == 0)
	mode = 0660;

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
 * LOAD("DBM_CLOSE(INTEGER)STRING")
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
 * LOAD("DBM_STORE(INTEGER,STRING,STRING,INTEGER)INTEGER")
 *
 * arg 1:	file handle
 * arg 2:	key
 * arg 3:	data
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
 * LOAD("DBM_FETCH(INTEGER,STRING)STRING")
 *
 * arg 1:	file handle
 * arg 2:	key
 *
 * returns:
 * data or failure
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
 * LOAD("DBM_FIRSTKEY(INTEGER)STRING")
 *
 * arg 1:	file handle
 *
 * returns:
 * data or failure
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
 * LOAD("DBM_NEXTKEY(INTEGER)STRING")
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
 * LOAD("DBM_DELETE(INTEGER,STRING)INTEGER")
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
    datum key, data;
    DBM *f = lookup_handle(&dbm_files, LA_INT(0));
    int flags;
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
