/* $Id$ */

/*
 * sqlite3 database module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 11/2/2013
 * from ndbm.c 9/2/2004
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>			/* free() */
#endif

#include <sqlite3.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"			/* strlen, for RETSTR */

static handle_handle_t sqlite3_dbs;
static handle_handle_t sqlite3_stmts;

/*
 * LOAD("SQLITE3_OPEN(STRING)INTEGER", SQLITE3_DL)
 * Open a database file
 *
 * first arg:
 *	filename
 * return handle, or failure
 */
int
SQLITE3_OPEN( LA_ALIST ) LA_DCL
{
    sqlite3 *db;
    char *fname = mgetstring(LA_PTR(0));
    int ret = sqlite3_open(fname, &db);
    snohandle_t h;

    free(fname);

    if (ret != SQLITE_OK)
	RETFAIL;

    h = new_handle(&sqlite3_dbs, db);
    if (h == BAD_HANDLE) {
	sqlite3_close(db);
	RETFAIL;
    }
    RETINT(h);
}

/*
 * LOAD("SQLITE3_CLOSE(INTEGER)STRING", SQLITE3_DL)
 *
 * return null string or failure
 */
int
SQLITE3_CLOSE( LA_ALIST ) LA_DCL
{
    int h = LA_INT(0);
    sqlite3 *db = lookup_handle(&sqlite3_dbs, h);
    if (!db)
	RETFAIL;

    remove_handle(&sqlite3_dbs, h);
    sqlite3_close(db);
    RETNULL;
}

/*
 * LOAD("SQLITE3_ERRMSG(INTEGER)STRING", SQLITE3_DL)
 * arg1: db handle
 * return string or failure
 */
int
SQLITE3_ERRMSG( LA_ALIST ) LA_DCL
{
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_INT(0));
    if (!db)
	RETFAIL;

    RETSTR(sqlite3_errmsg(db));
}

/*
 * LOAD("SQLITE3_LAST_INSERT_ROWID(INTEGER)INTEGER", SQLITE3_DL)
 * arg1: db handle
 * return id or failure
 */
int
SQLITE3_LAST_INSERT_ROWID( LA_ALIST ) LA_DCL
{
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_INT(0));
    if (!db)
	RETFAIL;

    RETINT(sqlite3_last_insert_rowid(db));
}

/*
 * LOAD("SQLITE3_PREPARE(INTEGER,STRING)INTEGER", SQLITE3_DL)
 *
 * arg 1:	db handle
 * arg 2:	SQL statement
 *
 * returns:
 * stmt handle
 * or failure
 *
 */
int
SQLITE3_PREPARE( LA_ALIST ) LA_DCL
{
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_INT(0));
    sqlite3_stmt *st;
    snohandle_t sh;			/* stmt handle */
    int arg;
    int ret;

    if (!db)
	RETFAIL;

    /* DANGER!! won't make a copy if it sees a NUL byte before len reached! */
    ret = sqlite3_prepare_v2(db, LA_STR_PTR(1), LA_STR_LEN(1), &st, NULL);
    if (ret != SQLITE_OK)
	RETFAIL;

    sh = new_handle(&sqlite3_stmts, st);
    if (sh == BAD_HANDLE) {
	sqlite3_finalize(st);
	RETFAIL;
    }

    /* take additional arguments as positional parameters */
    for (arg = 2; arg < nargs; arg++) {
	int par = arg - 1;		/* one based */
	int ret;

	switch (LA_TYPE(arg)) {
	case I:
	    if (sizeof(int_t) == 8)
		ret = sqlite3_bind_int64(st, par, LA_INT(arg));
	    else
		ret = sqlite3_bind_int(st, par, LA_INT(arg));
	    break;
	case R:
	    ret = sqlite3_bind_double(st, par, (double)LA_REAL(arg));
	    break;
	case S:
	    if (LA_STR_LEN(arg) > 0)
		ret = sqlite3_bind_text(st, par,
					(char *)LA_STR_PTR(arg),
					LA_STR_LEN(arg),
					SQLITE_TRANSIENT);
	    else
		ret = sqlite3_bind_null(st, par);
	    break;
	default:
	    RETFAIL;
	}
#if 0
	if (ret != SQLITE_OK)
	    RETFAIL;
#endif
    }
    RETINT(sh);
}

/*
 * bind a single host parameter (any type)
 * LOAD("SQLITE3_BIND_ANY(INTEGER,INTEGER,)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	anonymous host parameter number to bind (one based)
 * arg 3:	value
 *
 * returns:
 * null string or failure
 *
 */
int
SQLITE3_BIND_ANY( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    int par = LA_INT(1);		/* parameter number */
    int ret;

    if (!st || nargs != 3)
	RETFAIL;

    switch (LA_TYPE(2)) {
    case I:
	if (sizeof(int_t) == 8)
	    ret = sqlite3_bind_int64(st, par, LA_INT(2));
	else
	    ret = sqlite3_bind_int(st, par, LA_INT(2));
	break;
    case R:
	ret = sqlite3_bind_double(st, par, (double)LA_REAL(2));
	break;
    case S:
	if (LA_STR_LEN(2) > 0)
	    ret = sqlite3_bind_text(st, par,
				    (char *)LA_STR_PTR(2),
				    LA_STR_LEN(2),
				    SQLITE_TRANSIENT);
	else
	    ret = sqlite3_bind_null(st, par);
	break;
    default:
	RETFAIL;
    }
#if 0
    if (ret == SQLITE_MISUSE)
	RETFAIL;
#endif
    RETNULL;
}

/*
 * bind a single host parameter with a BLOB value
 * LOAD("SQLITE3_BIND_BLOB(INTEGER,INTEGER,STRING)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	anonymous host parameter number to bind (one based)
 * arg 3:	value
 *
 * returns:
 * null string or failure
 *
 */
int
SQLITE3_BIND_BLOB( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    int par = LA_INT(1);		/* parameter number */
    int ret;

    if (!st || nargs != 3)
	RETFAIL;

    if (LA_TYPE(2) != S)
	RETFAIL;
    
    ret = sqlite3_bind_blob(st, par,
			    (char *)LA_STR_PTR(2),
			    LA_STR_LEN(2),
			    SQLITE_TRANSIENT);
#if 0
    if (ret == SQLITE_MISUSE)
	RETFAIL;
#endif
    RETNULL;
}

/*
 * bind multiple positional arguments to a prepared statement
 * LOAD("SQLITE3_BIND_MANY(INTEGER,)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2+:	values...
 *
 * returns:
 * null string or failure
 *
 */
int
SQLITE3_BIND_MANY( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    int arg;

    if (!st)
	RETFAIL;

    for (arg = 1; arg < nargs; arg++) {
	int par = arg;			/* one based */
	int ret;

	switch (LA_TYPE(arg)) {
	case I:
	    if (sizeof(int_t) == 8)
		ret = sqlite3_bind_int64(st, par, LA_INT(arg));
	    else
		ret = sqlite3_bind_int(st, par, LA_INT(arg));
	    break;
	case R:
	    ret = sqlite3_bind_double(st, par, (double)LA_REAL(arg));
	    break;
	case S:
	    if (LA_STR_LEN(arg) > 0)
		ret = sqlite3_bind_text(st, par,
					(char *)LA_STR_PTR(arg),
					LA_STR_LEN(arg),
					SQLITE_TRANSIENT);
	    else
		ret = sqlite3_bind_null(st, par);
	    break;
	default:
	    RETFAIL;
	}
#if 0
	if (ret != SQLITE_OK)
	    RETFAIL;
#endif
    }
    RETNULL;
}

/*
 * LOAD("SQLITE3_BIND_PARAMETER_COUNT(INTEGER)INTEGER", SQLITE3_DL)
 * arg1: stmt handle
 * return integer or failure
 */
int
SQLITE3_BIND_PARAMETER_COUNT( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (st)
	RETINT(sqlite3_bind_parameter_count(st));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_BIND_PARAMETER_NAME(INTEGER,INTEGER)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	named parameter number fetch name for
 *
 * returns:
 * string or failure
 *
 */
int
SQLITE3_BIND_PARAMETER_NAME( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    if (st)
	RETSTR(sqlite3_bind_parameter_name(st, LA_INT(1)));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_BIND_PARAMETER_INDEX(INTEGER,STRING)INTEGER", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	parameter name to fetch index for
 *
 * returns:
 * integer or failure
 *
 */
int
SQLITE3_BIND_PARAMETER_INDEX( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    char *name;
    int ret;

    if (!st)
	RETFAIL;

    name = mgetstring(LA_PTR(1));
    ret = sqlite3_bind_parameter_index(st, name);
    free(name);
    RETINT(ret);
}

/*
 * LOAD("SQLITE3_STEP(INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return string or failure
 */
int
SQLITE3_STEP( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    int ret;

    if (!st)
	RETFAIL;

    switch (sqlite3_step(st)) {
    case SQLITE_ROW: RETSTR("row");
    case SQLITE_DONE: RETSTR("done");
    }
    /* here with BUSY or ERROR; if BUSY retry???? */
    /* also SQLITE_INTERRUPT, SQLITE_SCHEMA, SQLITE_CORRUPT?? */
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_COUNT(INTEGER)INTEGER", SQLITE3_DL)
 * arg1: stmt handle
 * return int or failure
 */
int
SQLITE3_COLUMN_COUNT( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (!st)
	RETFAIL;

    RETINT(sqlite3_column_count(st));
}

/*
 * LOAD("SQLITE3_COLUMN_NAME(INTEGER,INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return string or failure
 */
int
SQLITE3_COLUMN_NAME( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (st)
	RETSTR((char *)sqlite3_column_name(st, LA_INT(1)));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_TEXT(INTEGER,INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return string or failure
 */
int
SQLITE3_COLUMN_TEXT( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    if (st)
	RETSTR((char *)sqlite3_column_text(st, LA_INT(1)));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_VALUE(INTEGER,INTEGER)", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return value or failure
 */
int
SQLITE3_COLUMN_VALUE( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));
    int col = LA_INT(1);

    if (!st)
	RETFAIL;

    switch (sqlite3_column_type(st, col)) {
    case SQLITE_INTEGER:
	RETINT(sqlite3_column_int64(st, col));
    case SQLITE_FLOAT:
	RETREAL(sqlite3_column_double(st, col));
    case SQLITE_BLOB:
	RETSTR2((char *)sqlite3_column_blob(st, col),
		sqlite3_column_bytes(st, col));
    case SQLITE_NULL:
	RETNULL;
    case SQLITE3_TEXT:
    default:
	RETSTR((char *)sqlite3_column_text(st, col));
    }
    RETFAIL;				/* SNH */
}

/*
 * Reset All Bindings On A Prepared Statement
 *    "Contrary to the intuition of many, sqlite3_reset() does not
 *    reset the bindings on a prepared statement. Use this routine to
 *    reset all host parameters to NULL."
 *
 * LOAD("SQLITE3_CLEAR_BINDINGS(INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
int
SQLITE3_CLEAR_BINDINGS( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (!st)
	RETFAIL;

    if (sqlite3_clear_bindings(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
 * Reset A Prepared Statement Object
 *   "The sqlite3_reset() function is called to reset a prepared
 *   statement object back to its initial state, ready to be
 *   re-executed. Any SQL statement variables that had values bound to
 *   them using the sqlite3_bind_*() API retain their values. Use
 *   sqlite3_clear_bindings() to reset the bindings."
 *
 * LOAD("SQLITE3_RESET(INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
int
SQLITE3_RESET( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (!st)
	RETFAIL;
    if (sqlite3_reset(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
 * LOAD("SQLITE3_FINALIZE(INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
int
SQLITE3_FINALIZE( LA_ALIST ) LA_DCL
{
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_INT(0));

    if (!st)
	RETFAIL;

    if (sqlite3_finalize(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
 * LOAD("SQLITE3_EXEC(INTEGER,STRING)STRING", SQLITE3_DL)
 *
 * arg 1:	db handle
 * arg 2:	SQL statement
 *
 * returns:
 * null string or failure
 *
 */

/* XXX take params, do it the hard way!!!! */
int
SQLITE3_EXEC( LA_ALIST ) LA_DCL
{
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_INT(0));
    char *sql;
    int ret;

    if (!db)
	RETFAIL;

    sql = mgetstring(LA_PTR(1));
    ret = sqlite3_exec(db, sql, NULL, NULL, NULL);
    free(sql);
    if (ret == SQLITE_OK)
	RETNULL;
    RETFAIL;
}
