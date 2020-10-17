/* $Id$ */

/*
 * sqlite3 database module for CSNOBOL4
 * Phil Budne <phil@ultimate.com> 11/2/2013
 * from ndbm.c 9/2/2004
 *
 * For sqlite3 function docs, see:
 * https://www.sqlite.org/c3ref/funclist.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>			/* free() */

#include <sqlite3.h>

#ifdef DEBUG
#include <stdio.h>
#define DEBUGF(X) printf X
#else
#define DEBUGF(X)
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "handle.h"
#include "str.h"			/* strlen, for RETSTR */

MODULE(sqlite3);

static handle_handle_t sqlite3_dbs;
static handle_handle_t sqlite3_stmts;

/*
**=pea
**=sect NAME
**snobol4sqlite3 \- SQLITE3 interface for SNOBOL4
**=sect SYNOPSYS
**B<-INCLUDE 'sqlite3.sno'>
**=sect DESCRIPTION
**=item B<SQLITE3_OPEN(>I<path>B<)>
**opens a database and returns a database handle or failure.
**=cut
*/

/*
 * LOAD("SQLITE3_OPEN(STRING)", SQLITE3_DL)
 * Open a database file
 *
 * first arg:
 *	filename
 * return db_handle, or failure
 */

static void
free_sqlite3(void *db) {
    sqlite3_close(db);
}

lret_t
SQLITE3_OPEN( LA_ALIST ) {
    sqlite3 *db;
    char *fname = mgetstring(LA_PTR(0));
    int ret = sqlite3_open(fname, &db);
    snohandle_t h;

    free(fname);

    if (ret != SQLITE_OK)
	RETFAIL;

    h = new_handle2(&sqlite3_dbs, db, "sqlite3_dbs", free_sqlite3, &module);
    if (!OK_HANDLE(h)) {
	sqlite3_close(db);
	RETFAIL;
    }
    RETHANDLE(h);
}

/*
**=pea
**=item B<SQLITE3_CLOSE(>I<db_handle>B<)>
**closes database and returns empty string or failure.
**=cut
*/
/*
 * LOAD("SQLITE3_CLOSE(EXTERNAL)STRING", SQLITE3_DL)
 *
 * return null string or failure
 */
lret_t
SQLITE3_CLOSE( LA_ALIST ) {
    snohandle_t h = LA_HANDLE(0);
    sqlite3 *db = lookup_handle(&sqlite3_dbs, h);
    if (!db)
	RETFAIL;

    remove_handle(&sqlite3_dbs, h);
    sqlite3_close(db);
    RETNULL;
}

/*
**=pea
**=item B<SQLITE3_ERRMSG(>I<db_handle>B<)>
**Returns an SQLITE3 error string or failure if the handle is invalid.
**=cut
*/
/*
 * LOAD("SQLITE3_ERRMSG(EXTERNAL)STRING", SQLITE3_DL)
 * arg1: db handle
 * return string or failure
 */
lret_t
SQLITE3_ERRMSG( LA_ALIST ) {
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_HANDLE(0));
    if (!db)
	RETFAIL;

    RETSTR(sqlite3_errmsg(db));
}

/*
**=pea
**=item B<SQLITE3_LAST_INSERT_ROWID(>I<db_handle>B<)>
**Returns an integer or failure if the handle is invalid.
**=cut
*/
/*
 * LOAD("SQLITE3_LAST_INSERT_ROWID(EXTERNAL)INTEGER", SQLITE3_DL)
 * arg1: db handle
 * return id or failure
 */
lret_t
SQLITE3_LAST_INSERT_ROWID( LA_ALIST ) {
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_HANDLE(0));
    if (!db)
	RETFAIL;

    RETINT(sqlite3_last_insert_rowid(db));
}

/*
**=pea
**=item B<SQLITE3_PREPARE(>I<db_handle>,I<SQL>[,I<params ...>]B<)>
**Parses an SQL statement and optionally binds positional parameters to it.
**Call B<SQLITE3_ROW_ARRAY()> or B<SQLITE3_ROW_TABLE()> to fetch a row of results
**once all parameters have been bound.
**Returns a statement handle or failure.
**=cut
*/
/*
 * LOAD("SQLITE3_PREPARE(EXTERNAL,STRING)", SQLITE3_DL)
 *
 * arg 1:	db handle
 * arg 2:	SQL statement
 *
 * returns:
 * stmt handle
 * or failure
 *
 */
static void
free_stmt(void *stmt) {
    sqlite3_finalize(stmt);
}

lret_t
SQLITE3_PREPARE( LA_ALIST ) {
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_HANDLE(0));
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

    DEBUGF(("PREP: dbh %ld db %p stp %p\n", LA_INT(0), db, st));

    sh = new_handle2(&sqlite3_stmts, st, "sqlite3_stmts", free_stmt, &module);
    if (!OK_HANDLE(sh)) {
	sqlite3_finalize(st);
	RETFAIL;
    }

    DEBUGF(("PREP: dbh %ld db %p stp %p sth %ld\n", LA_INT(0), db, st, sh));

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
    RETHANDLE(sh);
}

/*
**=pea
**=item B<SQLITE3_BIND_ANY(>I<st_handle>, I<position>, I<value>B<)>
**Binds I<value> at (one-based) I<position> to a prepared SQL statement.
**Returns null string or failure.
**=cut
*/
/*
 * bind a single host parameter (any type)
 * LOAD("SQLITE3_BIND_ANY(EXTERNAL,INTEGER,)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	anonymous host parameter number to bind (one based)
 * arg 3:	value
 *
 * returns:
 * null string or failure
 *
 */
lret_t
SQLITE3_BIND_ANY( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
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
**=pea
**=item B<SQLITE3_BIND_BLOB(>I<st_handle>, I<position>, I<value>B<)>
**Binds I<value> to (one-based) I<position> as a BLOB value to prepared SQL statement.
**Returns null string or failure.
**=cut
*/
/*
 * bind a single host parameter with a BLOB value
 * LOAD("SQLITE3_BIND_BLOB(EXTERNAL,INTEGER,STRING)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	anonymous host parameter number to bind (one based)
 * arg 3:	value
 *
 * returns:
 * null string or failure
 *
 */
lret_t
SQLITE3_BIND_BLOB( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
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
**=pea
**=item B<SQLITE3_BIND_MANY(>I<st_handle>, I<values ...>B<)>
**Binds multiple positional parameter I<values> to a prepared SQL statement.
**Returns null string or failure.
**=cut
*/
/*
 * bind multiple positional arguments to a prepared statement
 * LOAD("SQLITE3_BIND_MANY(EXTERNAL,)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2+:	values...
 *
 * returns:
 * null string or failure
 *
 */
lret_t
SQLITE3_BIND_MANY( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
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
**=pea
**=item B<SQLITE3_BIND_PARAMETER_COUNT(>I<st_handle>B<)>
**Returns the number of parameters required by a prepared SQL statement, or fails.
**=cut
*/
/*
 * LOAD("SQLITE3_BIND_PARAMETER_COUNT(EXTERNAL)INTEGER", SQLITE3_DL)
 * arg1: stmt handle
 * return integer or failure
 */
lret_t
SQLITE3_BIND_PARAMETER_COUNT( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (st)
	RETINT(sqlite3_bind_parameter_count(st));
    RETFAIL;
}

/*
**=pea
**=item B<SQLITE3_BIND_PARAMETER_NAME(>I<st_handle>, I<position>B<)>
**Returns the name (if any) for the parameter I<position> number in
**a prepared SQL statement.
**=cut
*/
/*
 * LOAD("SQLITE3_BIND_PARAMETER_NAME(EXTERNAL,INTEGER)STRING", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	named parameter number fetch name for
 *
 * returns:
 * string or failure
 *
 */
lret_t
SQLITE3_BIND_PARAMETER_NAME( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
    if (st)
	RETSTR(sqlite3_bind_parameter_name(st, LA_INT(1)));
    RETFAIL;
}

/*
**=pea
**=item B<SQLITE3_BIND_PARAMETER_INDEX(>I<st_handle>, I<name>B<)>
**Returns the positional index for named parameter I<name> in
**prepared SQL statement.
**=cut
*/
/*
 * LOAD("SQLITE3_BIND_PARAMETER_INDEX(EXTERNAL,STRING)INTEGER", SQLITE3_DL)
 *
 * arg 1:	stmt handle
 * arg 2:	parameter name to fetch index for
 *
 * returns:
 * integer or failure
 *
 */
lret_t
SQLITE3_BIND_PARAMETER_INDEX( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
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
 * LOAD("SQLITE3_STEP(EXTERNAL)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return string or failure
 */
lret_t
SQLITE3_STEP( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
    int val;

    if (!st)
	RETFAIL;

    val = sqlite3_step(st);
    DEBUGF(("STEP: sth %ld stp %p val: %d\n", LA_INT(0), st, val));
    switch (val) {
    case SQLITE_ROW: RETSTR("row");
    case SQLITE_DONE: RETSTR("done");
    case SQLITE_BUSY: RETSTR("busy");	/* retry?? */
    case SQLITE_ERROR: RETFAIL;
    }
    /* also SQLITE_INTERRUPT, SQLITE_SCHEMA, SQLITE_CORRUPT?? */
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_COUNT(EXTERNAL)INTEGER", SQLITE3_DL)
 * arg1: stmt handle
 * return int or failure
 */
lret_t
SQLITE3_COLUMN_COUNT( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (!st)
	RETFAIL;

    RETINT(sqlite3_column_count(st));
}

/*
 * LOAD("SQLITE3_COLUMN_NAME(EXTERNAL,INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return string or failure
 */
lret_t
SQLITE3_COLUMN_NAME( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (st)
	RETSTR((char *)sqlite3_column_name(st, LA_INT(1)));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_TEXT(EXTERNAL,INTEGER)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return string or failure
 */
lret_t
SQLITE3_COLUMN_TEXT( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
    if (st)
	RETSTR((char *)sqlite3_column_text(st, LA_INT(1)));
    RETFAIL;
}

/*
 * LOAD("SQLITE3_COLUMN_VALUE(EXTERNAL,INTEGER)", SQLITE3_DL)
 * arg1: stmt handle
 * arg2: column number
 * return value or failure
 */
lret_t
SQLITE3_COLUMN_VALUE( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));
    int col = LA_INT(1);

    DEBUGF(("COLVAL: sth %ld stp %p col %d\n", LA_INT(0), st, col));
    if (!st)
	RETFAIL;

    DEBUGF(("COLVAL: type %d\n", sqlite3_column_type(st, col)));
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
**=pea
**=item B<SQLITE3_CLEAR_BINDINGS(>I<st_handle>B<)>
**Use this routine to reset all parameter bindings to NULL.
**=cut
*/
/*
 * LOAD("SQLITE3_CLEAR_BINDINGS(EXTERNAL)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
lret_t
SQLITE3_CLEAR_BINDINGS( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (!st)
	RETFAIL;

    if (sqlite3_clear_bindings(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
**=pea
**=item B<SQLITE3_RESET(>I<st_handle>B<)>
**Reset a prepared statement back to its initial state, ready to be
**re-executed. Any SQL statement variables that had values bound to
**them retain their values. Use B<sqlite3_clear_bindings()>
**to reset the bindings.
**=cut
*/
/*
 * LOAD("SQLITE3_RESET(EXTERNAL)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
lret_t
SQLITE3_RESET( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (!st)
	RETFAIL;
    if (sqlite3_reset(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
**=pea
**=item B<SQLITE3_FINALIZE(>I<st_handle>B<)>
**Release (delete) a statement handle.
**=cut
*/
/*
 * LOAD("SQLITE3_FINALIZE(EXTERNAL)STRING", SQLITE3_DL)
 * arg1: stmt handle
 * return empty string or failure
 */
lret_t
SQLITE3_FINALIZE( LA_ALIST ) {
    sqlite3_stmt *st = lookup_handle(&sqlite3_stmts, LA_HANDLE(0));

    if (!st)
	RETFAIL;

    if (sqlite3_finalize(st) == SQLITE_OK)
	RETNULL;
    RETFAIL;
}

/*
**=pea
**=item B<SQLITE3_EXEC(>I<db_handle>,I<SQL statement(s)>B<)>
**Run one or more SQL statements that do not require parameters.
**=cut
*/
/*
 * LOAD("SQLITE3_EXEC(EXTERNAL,STRING)STRING", SQLITE3_DL)
 *
 * arg 1:	db handle
 * arg 2:	SQL statement
 *
 * "The sqlite3_exec() interface is a convenience wrapper around
 * sqlite3_prepare_v2(), sqlite3_step(), and sqlite3_finalize(), that
 * allows an application to run multiple statements of SQL without
 * having to use a lot of C code.
 *
 * returns:
 * null string or failure
 *
 */

/* XXX take params, do it the hard way!!!! */
lret_t
SQLITE3_EXEC( LA_ALIST ) {
    sqlite3 *db = lookup_handle(&sqlite3_dbs, LA_HANDLE(0));
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
