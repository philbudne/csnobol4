/* $Id$ */

/*
 * ``Poor Man's LOAD''
 * list of functions loaded with snobol4 image.
 *
 * See lib/snolib/*.c for example functions
 */

#if 0 /* examples: */
PMLFUNC2("FOO", bar)			/* function FOO entry point at "bar" */
#endif /* 0 */

#ifndef NO_PML_HOST
PMLFUNC(HOST)				/* SPITBOL compat */
PMPROTO("HOST(,)")			/* two polymorphic params */
#endif /* NO_PML_HOST not defined */

#ifndef NO_PML_EXIT
PMLFUNC(EXIT)				/* SPITBOL compat */
PMPROTO("EXIT()")			/* one polymorphic param */
#endif /* NO_PML_EXIT not defined */

#ifndef NO_PML_REAL
#ifndef NO_PML_SQRT
PMLFUNC(SQRT)
PMPROTO("SQRT(REAL)REAL")
#endif /* NO_PML_SQRT not defined */

#ifndef NO_PML_EXP
PMLFUNC(EXP)
PMPROTO("EXP(REAL)REAL")
#endif /* NO_PML_EXP not defined */

#ifndef NO_PML_LOG
PMLFUNC(LOG)
PMPROTO("LOG(REAL)REAL")
#endif /* NO_PML_LOG not defined */

#ifndef NO_PML_CHOP
PMLFUNC(CHOP)
PMPROTO("CHOP(REAL)REAL")
#endif /* NO_PML_CHOP not defined */

#ifndef NO_PML_TRIG
PMLFUNC(SIN)
PMPROTO("SIN(REAL)REAL")

PMLFUNC(COS)
PMPROTO("COS(REAL)REAL")

PMLFUNC(TAN)
PMPROTO("TAN(REAL)REAL")
#endif /* NO_PML_TRIG not defined */
#endif /* NO_PML_REAL not defined */

#ifndef NO_PML_FILE
PMLFUNC(FILE)				/* SITBOL compat */
PMPROTO("FILE(STRING)STRING")
#endif /* NO_PML_FILE not defined */

#ifndef NO_PML_DELETE
PMLFUNC(DELETE)				/* SITBOL compat */
PMPROTO("DELETE(STRING)STRING")
#endif /* NO_PML_DELETE not defined */

#ifndef NO_PML_RENAME
PMLFUNC(RENAME)				/* SITBOL compat */
PMPROTO("RENAME(STRING)STRING")
#endif /* NO_PML_RENAME not defined */

/* 4/7/97; */
PMLFUNC(IO_FINDUNIT)
PMPROTO("IO_FINDUNIT()INTEGER")

/* experimental 3/12/99; */
PMLFUNC(SSET)
PMPROTO("SSET(INTEGER,INTEGER,INTEGER,INTEGER)INTEGER")

/* 4/27/2001; */
#ifdef PML_SERV
PMLFUNC(SERV_LISTEN)
PMPROTO("SERV_LISTEN(STRING,STRING,STRING)INTEGER")
#endif /* PML_SERV defined */

/* 12/6/2001; */
PMLFUNC(ORD)
PMPROTO("ORD(STRING)INTEGER")

/* 1/21/2002; */
PMLFUNC(LOGIC)
/* LOAD() in load.sno */
