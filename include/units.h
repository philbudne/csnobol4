/* $Id$ */

#define NUNITS 256			/* XXX set at runtime? */

/*
 * "external" unit numbers (one-based)
 * (modern FORTRAN seems to use unit zero for stderr!)
 */
#define UNITI	5		/* INPUT: stdin */
#define UNITO	6		/* OUTPUT: stdout */
#define UNITP	7		/* TERMINAL output: stderr */
#define UNITT	8		/* TERMINAL input */

/* names associated with UNITI, UNITO, UNITP(!), UNITT */
#define STDIN_NAME  "stdin"		/* XXX "-" ? */
#define STDOUT_NAME "stdout"		/* XXX "-" ? */
#define STDERR_NAME "stderr"
#define TERMIN_NAME "termin"		/* terminal input */
