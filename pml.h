/* $Id$ */

/*
 * ``Poor Man's LOAD''
 * list of functions loaded with snobol4 image.
 *
 * See lib/snolib/*.c for example functions
 */

/*
 * NOTE!! PMLFUNC2 used for exntry points renamed due to 
 * name collisions (many due to VMS config.h define games)
 */

#ifndef NO_PML_HOST
PMLFUNC(HOST)				/* SPITBOL compat */
PMPROTO("HOST()")			/* two polymorphic params */
#endif /* NO_PML_HOST not defined */

#ifndef NO_PML_EXIT
PMLFUNC2("EXIT",EXIT)			/* SPITBOL compat */
PMPROTO("EXIT()")			/* one polymorphic param */
#endif /* NO_PML_EXIT not defined */

#ifndef NO_PML_REAL
#ifndef NO_PML_SQRT
PMLFUNC2("SQRT",SQRT)
PMPROTO("SQRT(REAL)REAL")
#endif /* NO_PML_SQRT not defined */

#ifndef NO_PML_EXP
PMLFUNC2("EXP",EXP)
PMPROTO("EXP(REAL)REAL")
#endif /* NO_PML_EXP not defined */

#ifndef NO_PML_LOG
PMLFUNC2("LOG",LOG)
PMPROTO("LOG(REAL)REAL")
#endif /* NO_PML_LOG not defined */

#ifndef NO_PML_CHOP
PMLFUNC(CHOP)
PMPROTO("CHOP(REAL)REAL")
#endif /* NO_PML_CHOP not defined */

#ifndef NO_PML_TRIG
PMLFUNC2("SIN",SIN)
PMPROTO("SIN(REAL)REAL")

PMLFUNC2("COS",COS)
PMPROTO("COS(REAL)REAL")

PMLFUNC2("TAN",TAN)
PMPROTO("TAN(REAL)REAL")
#endif /* NO_PML_TRIG not defined */
#endif /* NO_PML_REAL not defined */

#ifndef NO_PML_FILE
PMLFUNC2("FILE",FILE2)			/* SITBOL compat */
PMPROTO("FILE(STRING)STRING")
#endif /* NO_PML_FILE not defined */

#ifndef NO_PML_DELETE
PMLFUNC2("DELETE",DELETE)		/* SITBOL compat */
PMPROTO("DELETE(STRING)STRING")
#endif /* NO_PML_DELETE not defined */

#ifndef NO_PML_RENAME
PMLFUNC2("RENAME",RENAME)		/* SITBOL compat */
PMPROTO("RENAME(STRING)STRING")
#endif /* NO_PML_RENAME not defined */

/* 4/7/97; */
PMLFUNC2("IO_FINDUNIT",IO_FINDUNIT)
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
/* LOAD() in logic.sno */
