/* $Id$ */

/*
 * ``Poor Man's LOAD''
 * list of functions loaded with snobol4 image  
 */

#if 0 /* examples: */
PMLFUNC(FILE)				/* see snolib/file.c */
PMLFUNC(RENAME)				/* see snolib/rename.c */
PMLFUNC(DELETE)				/* see snolib/delete.c */
PMLFUNC2("FOO", bar)			/* entry point at "bar" */
#endif /* 0 */

#ifndef NO_PML_HOST
PMLFUNC(HOST)				/* SPITBOL compat */
PMLOAD("HOST(,)")			/* two polymorphic params */
#endif /* NO_PML_HOST not defined */

#ifndef NO_PML_EXIT
PMLFUNC(EXIT)				/* SPITBOL compat */
PMLOAD("EXIT()")			/* one polymorphic param */
#endif /* NO_PML_EXIT not defined */

#ifndef NO_PML_REAL
#ifndef NO_PML_SQRT
PMLFUNC(SQRT)
PMLOAD("SQRT(REAL)REAL")
#endif /* NO_PML_SQRT not defined */

#ifndef NO_PML_EXP
PMLFUNC(EXP)
PMLOAD("EXP(REAL)REAL")
#endif /* NO_PML_EXP not defined */

#ifndef NO_PML_LOG
PMLFUNC(LOG)
PMLOAD("LOG(REAL)REAL")
#endif /* NO_PML_LOG not defined */

#ifndef NO_PML_CHOP
PMLFUNC(CHOP)
PMLOAD("CHOP(REAL)REAL")
#endif /* NO_PML_CHOP not defined */

#ifndef NO_PML_TRIG
PMLFUNC(SIN)
PMLOAD("SIN(REAL)REAL")

PMLFUNC(COS)
PMLOAD("COS(REAL)REAL")

PMLFUNC(TAN)
PMLOAD("TAN(REAL)REAL")
#endif /* NO_PML_TRIG not defined */
#endif /* NO_PML_REAL not defined */
