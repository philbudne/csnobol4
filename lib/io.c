/* $Id$ */

#ifdef USE_STDARG_H			/* only if varargs not available */
#include <stdarg.h>
#else  /* USE_STDARG_H not defined */
#include <varargs.h>
#endif /* USE_STDARG_H not defined */

#include <stdio.h>
#include <ctype.h>

#ifdef NO_OFF_T
typedef long off_t;
#else  /* NO_OFF_T not defined */
#include <sys/types.h>			/* off_t */
#endif /* NO_OFF_T not defined */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H defined */

#ifdef ANSI_STRINGS
#include <string.h>
#else  /* ANSI_STRINGS not defined */
#include <strings.h>			/* berkeley strings */
#endif /* ANSI_STRINGS not defined */

#include "h.h"
#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "libret.h"			/* IO_xxx */

/* generated */
#include "equ.h"			/* for BCDFLD (for X_LOCSP), res.h */
#include "res.h"			/* needed on VAX/VMS for data.h */
#include "data.h"			/* for FILENM */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET not defined */

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif /* SEEK_CUR not defined */

#ifndef SEEK_END
#define SEEK_END 2
#endif /* SEEK_END not defined */

#define NUNITS 256			/* XXX set at runtime? */

/* check an internal (zero based) unit number; */
#define BADUNIT(U) ((U) < 0 || (U) >= NUNITS)

/* names associated with UNITI, UNITO, UNITP(!), UNITT */
#define STDIN_NAME  "stdin"		/* XXX "-" ? */
#define STDOUT_NAME "stdout"		/* XXX "-" ? */
#define STDERR_NAME "stderr"
#define TERMIN_NAME "termin"		/* terminal input */

#ifndef SNOLIB_DIR
#define SNOLIB_DIR "./"
#endif /* SNOLIB_DIR not defined */

struct unit {
    struct file *curr;			/* ptr to current file */
    /* for rewind; */
    struct file *head;			/* first file in list */
    off_t offset;			/* offset in first file to rewind to */
};

struct file {
    struct file *next;			/* next input file */
    FILE *f;				/* may be NULL if not (yet) open */
    int flags;				/* FL_xxx */
    /* XXX add close hook (pointer to function)?? */
    /* XXX keep recl (shift flags up?)? */
    enum { LAST_NONE, LAST_OUTPUT, LAST_INPUT } last;
    /* MUST BE LAST!! */
    char fname[1];
};

/*
 * Per-file (unit) flags.
 *
 * ** NOTE **
 *
 * multiple units may refer to same stdio stream (via "-" and
 * /dev/std{in,out,err} magic files) or the same file descriptors (via
 * /dev/fd/N magic pathname) and have different behaviors.
 */

#define FL_PIPE		01		/* file was popen'ed; use pclose() */
#define FL_EOL		02		/* strip EOL on input, add on output */
#define FL_BINARY	04		/* binary: no EOL; use recl */
#define FL_UPDATE	010		/* update: read+write */
#define FL_UNBUF	020		/* unbuffered write */
#define FL_APPEND	040		/* append */
#define FL_TTY		0100		/* is a tty */
#define FL_NOECHO	0200		/* tty: no echo */
#define FL_NOCLOSE	0400		/* don't fclose() */
#define FL_INPUT	01000		/* attached for INPUT() */
#define FL_OUTPUT	02000		/* attached for OUTPUT() */

#define MAXFNAME	1024		/* XXX use MAXPATHLEN? POSIX?? */
#define MAXOPTS		1024

/* XXX malloc at runtime? */
static struct unit io_units[NUNITS];

static struct file *includes;		/* list of included files */
static int compiling;			/* TRUE iff compiler still running */
static FILE *termin;			/* TERMINAL input */

extern int rflag;			/* from init.c */
extern FILE *term_input();		/* from <system>/term.c */
extern void *malloc();

#ifdef NEED_POPEN_DECL
extern FILE *popen();
#endif /* NEED_POPEN_DECL defined */

static struct file *
io_newfile( path )
    char *path;
{
    struct file *fp;

    fp = (struct file *) malloc( sizeof( struct file ) + strlen(path) );
    if (fp == NULL)
	return NULL;

    bzero( (char *)fp, sizeof (struct file) );
    strcpy(fp->fname,path);
    fp->flags = FL_EOL;			/* normal */
    fp->last = LAST_NONE;		/* nothing yet */
    return fp;
}

/* add file to input list */
static int
io_addfile( unit, path, append )
    int unit;
    char *path;
    int append;
{
    /* XXX check for commas in path? */
    struct file *fp;
    struct unit *up;

    fp = io_newfile(path);
    if (fp == NULL)
	return FALSE;

    up = io_units + unit;
    if (append) {			/* add to end of list */
	struct file *tp;

	tp = up->curr;
	if (tp == NULL) {
	    up->head = up->curr = fp;
	    up->offset = 0;
	}
	else {
	    while (tp->next)
		tp = tp->next;
	    tp->next = fp;
	}
    }
    else {				/* prepend (ie; for "include") */
	fp->next = up->curr;
	up->head = up->curr = fp;
	up->offset = 0;
    }
    return TRUE;
} /* io_addfile */

/* close currently open file on a unit */
/* XXX take flag: to free struct file, or not? */
static int
io_close(unit)				/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;
    struct unit *up;
    int ret;

    up = io_units + unit;
    fp = up->curr;
    if (fp == NULL)
	return TRUE;

    if (fp->f) {
	/* XXX call close hook? */
	if (fp->flags & FL_PIPE) {
	    ret = (pclose(fp->f) == 0);	/* XXX is process status!! */
	    fp->f = NULL;
	}
	else {				/* not a pipe */
	    if (fp->flags & FL_TTY) {
		tty_close(fp->f);	/* advisory */
	    }

	    if (fp->flags & FL_NOCLOSE) {
		/* never close stdin, stdout, stderr */
		ret = (fflush(fp->f) != EOF);
	    }
	    else {
		ret = (fclose(fp->f) == 0);
		fp->f = NULL;
	    }
	} /* not a pipe */
    } /* have fp->f */

    up->curr = fp->next;
    return ret;
}

/* close a unit, flush current file list */
int
io_closeall(unit)			/* internal (zero-based unit) */
    int unit;
{
    struct file *fp, *next;
    int ret;

    /* close any/all open files on chain */
    ret = TRUE;
    while (io_units[unit].curr != NULL)
	if (!io_close(unit))
	    ret = FALSE;

    /* free up all file structs */
    fp = io_units[unit].head;
    while (fp != NULL) {
	next = fp->next;
	free(fp);
	fp = next;
    }
    io_units[unit].curr = io_units[unit].head = NULL;

    return ret;
}

static void
io_fopen2( fp, mode )
    struct file *fp;
    char *mode;
{
    char *mp;
    char buf[4];			/* x+b<NUL> */

    fp->f = NULL;

    /* handle magic filenames (have a table (prefix or full str)??) */

    if (fp->fname[0] == '|') {
	/* filename with leading '|' opens a pipe! */
	/* SPITBOL: leading '!' means pipe, (with escaping?) */
	fp->flags |= FL_PIPE;		/* XXX set close hook? */
	fp->f = popen(fp->fname+1, mode);
	return;
    }

    /* filename "-" goes to stdin/out */
    if (strcmp(fp->fname,"-") == 0) {
	if (mode[0] == 'r')
	    fp->f = stdin;
	else
	    fp->f = stdout;
	fp->flags |= FL_NOCLOSE;
	return;
    }
    if (strcmp(fp->fname,"/dev/stdin") == 0) {
	fp->f = stdin;
	fp->flags |= FL_NOCLOSE;
	return;
    }
    if (strcmp(fp->fname,"/dev/stdout") == 0) {
	fp->f = stdout;
	fp->flags |= FL_NOCLOSE;
	return;
    }
    if (strcmp(fp->fname,"/dev/stderr") == 0) {
	fp->f = stderr;
	fp->flags |= FL_NOCLOSE;
	return;
    }
#ifndef NO_FDOPEN
    if (strncmp(fp->fname, "/dev/fd/", 8) == 0) {
	int i;

	if (sscanf(fp->fname+8, "%d", &i) == 1) {
	    fp->f = fdopen(i, mode);
	    /* XXX only if fd is one of STD{IN,OUT,ERR}_FILENO?? */
	    fp->flags |= FL_NOCLOSE;
	}
	return;
    }
#endif /* NO_FDOPEN not defined */
    if (strncmp(fp->fname, "/tcp/", 5) == 0 ||
	strncmp(fp->fname, "/udp/", 5) == 0) {
	char fn2[MAXFNAME];		/* XXX */
	char *host, *service, *cp;
	int priv;
	extern FILE *tcp_open(), *udp_open();

	priv = 0;
	strcpy( fn2, fp->fname + 5 );	/* XXX strdup()? */
	host = fn2;
	service = index(host, '/');
	if (service == NULL)
	    return;
	*service++ = '\0';

	/* look for option suffixes, ignore if unknown */
	cp = index(service, '/');
	if (cp) {
	    char *op;

	    *cp++ = '\0';
	    do {
		op = cp;
		cp = index(cp, '/');
		if (cp)
		    *cp++ = '\0';

		if (strcmp(op, "priv") == 0)
		    priv = 1;
		/* XXX more magic? */
	    } while (cp);
	} /* have suffixes */

	if (fp->fname[1] == 'u')
	    fp->f = udp_open( host, service, -1, priv );
	else
	    fp->f = tcp_open( host, service, -1, priv );
	return;
    }

    /* ANSI tmpfile() function returns anonymous file open for R/W */
    if (strcmp(fp->fname, "/dev/tmpfile") == 0) {
	fp->f = tmpfile();
	return;
    }

#ifdef OSDEP_FOPEN
    /*
     * Allow interception of /dev/tty, /dev/null, etc on non-unix
     * systems.  Function should return TRUE if filename is being
     * intercepted, REGARDLESS of whether the actual open succeeds
     * (on successs, the function should set the FILE ** to point
     * to the open stream).
     */
    if (osdep_open(fp->fname, &fp->f))
	return;				/* intercepted */
#endif

    /* **** add new special filename hacks here *** */

    /* create full mode string for fopen() */
    mp = buf;
    if (mode[0] == 'w' && (fp->flags & FL_APPEND))
	*mp++ = 'a';
    else
	*mp++ = mode[0];		/* XXX append whole string? */
    if (fp->flags & FL_UPDATE)
	*mp++ = '+';
#ifndef NO_FOPEN_B
    if (fp->flags & FL_BINARY)
	*mp++ = 'b';
#endif /* NO_FOPEN_B not defined */
    *mp++ = '\0';

    fp->f = fopen(fp->fname, buf);
} /* io_fopen2 */

static FILE *
io_fopen( fp, mode )
    struct file *fp;
    char *mode;
{
    io_fopen2(fp,mode);
    if (fp->f == NULL)
	return NULL;

    if (fisatty(fp->f, fp->fname)) {
	/* XXX set close hook? */
	fp->flags |= FL_TTY;
    }
    else
	fp->flags &= ~FL_TTY;

    /* XXX if FL_UNBUF call setbuf(fp->f, NULL)??
     * this may force a read or write per-character, which
     * isn't what we're after!
     */
    return fp->f;
} /* io_fopen */

/* skip to next input file */
static int
io_next( unit )				/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;

    fp = io_units[unit].curr;
    if (fp == NULL)
	return FALSE;

    /* in case called preemptively! */
    if (fp->f != NULL)
	io_close(unit);			/* close, and advance */

    /* get new current file (io_close advances to next file in list) */
    fp = io_units[unit].curr;
    if (fp == NULL)
	return FALSE;

    if (fp->f != NULL)			/* already open? */
	return TRUE;

    /* XXX let io_read() do the work??? */
    /* XXX copy flags from previous file? */
    io_fopen( fp, "r");

    return fp->f != NULL;
} /* io_next */


/* here with filename from command line */
void
io_input( path )
    char *path;
{
    io_addfile( UNITI-1, path, TRUE );	/* append to list! */
}

/* setup a unit given an open fp and a "filename" */
static int
io_mkfile2( unit, f, fname, flags )
    int unit;				/* external (1-based) unit */
    FILE *f;
    char *fname;			/* "filename" for error reports */
    int flags;
{
    struct file *fp;
    struct unit *up;

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;
    fp->f = f;
    fp->flags |= flags;
    if (fisatty(f, fname)) {
	/* XXX set close hook? */
	fp->flags |= FL_TTY;
    }

    unit--;				/* make zero-based */
    io_closeall(unit);			/* close unit */

    up = io_units + unit;
    up->head = up->curr = fp;
    up->offset = 0;

    return TRUE;
}

int
io_mkfile( unit, f, fname )
    int unit;				/* external (1-based) unit */
    FILE *f;
    char *fname;			/* "filename" for error reports */
{
    return io_mkfile2( unit, f, fname, 0 );
}

/*
 * implement SIL operations;
 */

void
io_init()				/* here from INIT */
{
    if (io_units[UNITI-1].curr == NULL) { /* no input file(s)? */
	if (!io_mkfile2(UNITI, stdin, STDIN_NAME, FL_NOCLOSE)) {
	    perror("io_mkfile(stdin)");
	    exit(1);
	}
    }
    else {
	if (!io_next(UNITI-1)) {
	    perror(io_units[UNITI-1].curr->fname);
	    exit(1);
	}
    }

    /* XXX support -o outputfile? */

    if (!io_mkfile2(UNITO, stdout, STDOUT_NAME, FL_NOCLOSE)) {
	perror("io_mkfile(stdout)");
	exit(1);
    }

    if (!io_mkfile2(UNITP, stderr, STDERR_NAME, FL_NOCLOSE)) {
	perror("io_mkfile(stderr)");
	exit(1);
    }

    /*
     * tempting to overload UNITP for input/output. This works on Unix
     * (need to fdopen(2,"r+")), but it's bound to cause trouble on
     * SOME other system which doesn't have stderr on fd 2, or didn't
     * open fd 2 for both read and write!
     */
    termin = term_input();		/* call system dependant function */
    if (termin) {
	if (!io_mkfile2(UNITT, termin, TERMIN_NAME, FL_NOCLOSE)) {
	    perror("io_mkfile(termin)");
	    exit(1);
	}
    }

    compiling = 1;
} /* io_init */

/* limited printf */
void
io_printf
#ifdef USE_STDARG_H			/* for systems without varargs.h */
    (int unit, ...)
#else  /* USE_STDARG_H not defined */
    (va_alist) va_dcl
#endif /* USE_STDARG_H not defined */
{
    va_list vp;
    char *format;
    register char c;
    FILE *f;
    char line[1024];			/* XXX */
    char *lp;
#ifdef USE_STDARG_H
    va_start(vp,unit);
#else  /* USE_STDARG_H not defined */
    int unit;

    va_start(vp);

    unit = va_arg(vp, int);
#endif /* USE_STDARG_H not defined */

    unit--;				/* make zero-based */
    if (BADUNIT(unit) ||
	io_units[unit].curr == NULL ||
	(f = io_units[unit].curr->f) == NULL)
	return;

    /* keep output in line buffer, in case output unbuffered (ie; stderr) */
    lp = line;
    format = va_arg(vp, char *);
    while ((c = *format++) != '\0') {
	struct descr *dp;
	struct spec *sp;
	char *cp;

	/* scan forward until first %, and print all at once? */
	if (c != '%') {
	    *lp++ = c;
	    continue;
	}
	c = *format++;
	if (c == '\0')
	    break;
	switch (c) {
	case 'd':			/* plain decimal */
	    dp = va_arg(vp, struct descr *);
	    sprintf(lp, "%d", D_A(dp));
	    lp += strlen(lp);
	    break;
	case 'D':			/* padded decimal */
	    dp = va_arg(vp, struct descr *);
	    sprintf(lp, "%15d", D_A(dp));
	    lp += strlen(lp);
	    break;
	case 'F':			/* padded float */
	    dp = va_arg(vp, struct descr *);
	    sprintf(lp, "%15.3f", D_RV(dp));
	    lp += strlen(lp);
	    break;
	case 's':			/* c-string (from version.c) */
	    cp = va_arg(vp, char *);
	    strcpy(lp, cp);
	    lp += strlen(lp);
	    break;
	case 'S':			/* spec */
	    sp = va_arg(vp, struct spec *);
	    strncpy(lp, S_SP(sp), S_L(sp));
	    lp += S_L(sp);
	    break;
	case 'v':			/* variable */
	    dp = va_arg(vp, struct descr *);
	    dp = (struct descr *) D_A(dp); /* get var pointer */
	    if (dp) {
		struct spec s[1];

		X_LOCSP(s, dp);
		strncpy(lp, S_SP(s), S_L(s));
		lp += S_L(s);
	    }
	    break;
	default:
	    *lp++ = c;
	    break;
	}
    } /* while */
    va_end(vp);
    *lp = '\0';
    fputs(line, f);
} /* io_printf */

void
io_print( iokey, iob, sp )		/* STPRNT */
    struct descr *iokey;
    struct descr **iob;
    struct spec *sp;
{
    int unit;
    struct file *fp;
    FILE *f;
    int ret;

    /*
     * (*iob)[0]	title descr
     * (*iob)[1]	integer unit number
     * (*iob)[2]	pointer to natural var for format
     */

    D_A(iokey) = FALSE;			/* default to error */

    unit = D_A(*iob + 1);
    unit--;				/* make zero-based */

    if (BADUNIT(unit))
	return;

    fp = io_units[unit].curr;
    if (fp == NULL)
	return;

    f = fp->f;
    if (f == NULL)
	return;

    /*
     * ANSI C requires that a file positioning function intervene
     * between output and input. Would not be needed if UPDATE I/O
     * performed using read()/write() (ie; FL_UNBUF)
     */

    if ((fp->flags & FL_UPDATE) && fp->last == LAST_INPUT) {
	fseek(f, 0, SEEK_CUR);		/* seek relative by zero */
    }
    fp->last = LAST_OUTPUT;

    ret = TRUE;
    if (S_A(sp) && S_L(sp)) {
	int len;
	char *cp;

	len = S_L(sp);
	cp = S_SP(sp);
	if (compiling) {
	    char *ep;
	    int l2;

	    /* trim spaces & NUL's (without altering specifier) */
	    ep = cp + len - 1;
	    while (len > 0 && (*ep == ' ' || *ep == '\0')) {
		len--;
		ep--;
	    }

	    /* plug remaining NULs with spaces */
	    for (l2 = len; l2 > 0; l2--) {
		if (*cp == '\0')
		    *cp = ' ';
		cp++;
	    }
	    cp = S_SP(sp);
	} /* compiling */

#ifndef NO_UNBUF_RW
	if (fp->flags & FL_UNBUF) {
	    if (write(fileno(f), cp, len) != len)
		ret = FALSE;
	}
	else
#endif /* NO_UNBUF_RW */
	if (fwrite( cp, 1, len, f ) != len)
	    ret = FALSE;
    } /* have string */

    if (fp->flags & FL_EOL) {
#ifndef NO_UNBUF_RW
	if (fp->flags & FL_UNBUF) {
	    if (write(fileno(f), "\n", 1) != 1)
		ret = FALSE;
	}
	else
#endif /* NO_UNBUF_RW */
	if (putc( '\n', f ) == EOF)
	    ret = FALSE;
    }

#ifdef NO_UNBUF_RW
    if (fp->flags & FL_UNBUF) {
	/* simulate unbuffered I/O */
	if (fflush(f) == EOF)
	    ret = FALSE;
    }
#endif /* NO_UNBUF_RW */

    D_A(iokey) = ret;
} /* io_print */

int
io_endfile(unit)			/* ENFILE */
    int unit;
{
    unit--;				/* make zero-based */
    if (BADUNIT(unit) ||
	io_units[unit].curr == NULL && io_units[unit].head == NULL) {
	/* fatal error in SPITBOL, but not in SNOBOL4+ */
	return TRUE;
    }
    return io_closeall(unit);
}

#define COMPILING(UNIT) ((UNIT) == UNITI-1 && compiling)

enum io_read_ret
io_read( dp, sp )			/* STREAD */
    struct descr *dp;
    struct spec *sp;
{
    int unit;
    int recl;
    int len;
    char *cp;
    FILE *f;
    struct file *fp;

    unit = D_A(dp);
    unit--;				/* make zero-based */
    if (BADUNIT(unit) || io_units[unit].curr == NULL) {
	if (COMPILING(unit)) {
	    return IO_ERR;		/* compiler never quits!! */
	}
	return IO_EOF;
    }

    recl = S_L(sp);			/* YUK! */
    cp = S_SP(sp);
    for (;;) {
	fp = io_units[unit].curr;
	f = fp->f;
	if (f == NULL) {
	    f = io_fopen( fp, "r" );
	    if (f == NULL)
		return IO_ERR;
	}

	/*
	 * ANSI C requires that a file positioning function intervene
	 * between output and input. Would not be needed if UPDATE I/O
	 * performed using read()/write() (ie; FL_UNBUF).
	 *
	 * before FL_TTY check, in case tty_read() uses stdio functions.
	 */

	if ((fp->flags & FL_UPDATE) && fp->last == LAST_OUTPUT) {
	    fseek(f, 0, SEEK_CUR);	/* seek relative by zero */
	}
	fp->last = LAST_INPUT;

	if (fp->flags & FL_TTY) {
	    tty_mode( fp->f,
		     (fp->flags & FL_BINARY) != 0,
		     (fp->flags & FL_NOECHO) != 0,
		     recl );
	} /* FL_TTY set */

	if (fp->flags & FL_BINARY) {
#ifdef TTY_READ_RAW
	    if (fp->flags & FL_TTY)
		len = tty_read(f, cp, recl,
			       TRUE,	/* "raw" */
			       (fp->flags & FL_NOECHO) != 0, /* "noecho" */
			       FALSE,	/* "keepeol" */
			       fp->fname);
	    else
#endif /* TTY_READ_RAW defined */
#ifndef NO_UNBUF_RW
	    if (fp->flags & FL_UNBUF)
		len = read(fileno(f), cp, recl);
	    else
#endif /* NO_UNBUF_RW not defined */
		len = fread(cp, 1, recl, f);

	    if (len > 0)
		break;
	} /* binary */
#ifdef TTY_READ_COOKED
	else if (fp->flags & FL_TTY) {
	    len = tty_read(f, cp, recl,
			   FALSE,	/* "raw" */
			   (fp->flags & FL_NOECHO) != 0, /* "noecho" */
			   (fp->flags & FL_EOL) != 0, /* "keepeol" */
			   fp->fname);
	    if (len > 0)
		break;
	}
#endif /* TTY_READ_COOKED defined */
	else {				/* not binary */
	    /* fgets() returns at most recl-1 characters + NUL */
	    if (fgets(cp, recl, f) != NULL) {
		len = strlen(cp);

		/* ASSERT(len > 0) ??? */
		if (cp[len-1] == '\n') {	/* saw EOL */
		    if (fp->flags & FL_EOL) {	/* hide eol? */
			len--;			/* yes. */
		    }
		}
		else {				/* no EOL seen */
		    register int c;

		    /* ASSERT(len == recl-1) ??? */

		    /* read one more character, to fill in for NUL byte */
		    c = getc(f);
		    if (c != EOF) {
			/* save additional character if not EOL
			 * or if EOL should be returned
			 */
			if (c != '\n' || !(fp->flags & FL_EOL)) {
			    cp[len] = c;
			    len++;
			} /* not EOL or not hiding EOL */
		    } /* extra char not EOF */

		    /* if not at EOL or EOF, discard rest of "record" */
		    while (c != EOF && c != '\n')
			c = getc(f);

		    /* don't care if line terminated by EOL or EOF? */
		} /* no EOL */
		break;
	    } /* fgets OK */
	} /* not binary */

	/* here when read failed */
	if (feof(f)) {
	    if (!io_next(unit)) {	/* skip to next file, if any */
		/* XXX perror? */
		return IO_EOF;		/* no more files */
	    }
	    if (COMPILING(unit)) {
		/* force call to INCCK to pop old FILENM and LNNOCL */
		return IO_EOF;
	    }
	    continue;			/* try again! */
	} /* feof */

	/* error, wasn't eof?! */
	return IO_ERR;
    } /* forever */

    /* here on successful read */
    if (COMPILING(unit)) {
	/* compiler doesn't handle exaustion well; tack on a space.
	 * INBUF has extra room (for LIST RIGHT output)
	 */
	cp[len++] = ' ';
    }
    S_L(sp) = len;

    return IO_OK;
} /* io_read */

void
io_backspace(unit)			/* BKSPCE */
    int unit;
{
    UNDF();
}

void
io_rewind(unit)				/* REWIND */
    int unit;
{
    FILE *f;
    struct file *fp;
    struct unit *up;

    unit--;				/* make zero-based */
    if (BADUNIT(unit))
	return;

    up = io_units + unit;
    if (up->curr != up->head) {
	if (up->curr != NULL)		/* file is open but not first in list */
	    io_close(unit);		/* close it */
	up->curr = up->head;		/* reset to head of list */
	if (up->curr->f == NULL)
	    io_fopen(up->curr, "r");	/* XXX use original mode? */
    }
    fp = up->curr;
    if (fp == NULL)
	return;

    f = fp->f;
    if (f != NULL && (fp->flags & FL_PIPE) == 0) {
	fseek(f, up->offset, SEEK_SET);
	fp->last = LAST_NONE;		/* reset last I/O type */
   }
}

/* extensions; */

/* here at end of compilation */
void
io_ecomp()				/* XECOMP */
{
    struct unit *up;
    struct file *fp;

    compiling = 0;			/* turn off crocks for compiler */

    if (rflag == 0) {
	/* if -r was not given, switch INPUT to stdin!! */

	/* XXX check return?! */
	io_mkfile2( UNITI, stdin, STDIN_NAME, FL_NOCLOSE );
	return;
    }
    /*
     * else (start INPUT after END stmt)
     * save the file position the data begins at for rewind.
     *
     * SITBOL would skip to next input file (ie; io_next())
     * but SPARC SPITBOL doesn't!
     */

    up = io_units + UNITI - 1;

    /* free source files... */
    fp = up->head;
    while (fp != up->curr) {
	struct file *tp;

	tp = fp->next;
	free(fp);
	fp = tp;
    }

    up->head = up->curr;		/* save file for rewind */
    up->offset = ftell(up->curr->f);	/* save offset for rewind */

    /* free list of included filenames */
    while (includes) {
	struct file *tp;

	tp = includes->next;
	free(includes);
	includes = tp;
    }
}

/* process I/O option strings for io_openi and io_openo */
static int
io_options( fp, op, rp )
    struct file *fp;			/* IN: file pointer */
    char *op;				/* IN: options */
    int *rp;				/* OUT: recl */
{
    int flags;
    int recl;

    flags = fp->flags;
    recl = 0;

    /* XXX check here for leading hyphen; process SPITBOL style options? */

    while (*op) {
	switch (*op) {
	case '-':			/* reserved for SPITBOL ops */
	    /* XXX skip ahead 'till space or EOS? */
	    return FALSE;

	case ',':			/* optional SNOBOL4+ seperator */
	    op++;			/* skip it */
	    break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
#if 0
	    if (recl)			/* already got one? */
		return FALSE;		/* boing! */
#endif /* 0 */
	    recl = 0;
	    while (isdigit(*op)) {
		recl = recl * 10 + *op - '0'; /* XXX works for ASCII */
		op++;
	    }
	    break;

	case 'A':			/* SITBOL/SPITBOL: append */
	case 'a':
	    flags |= FL_APPEND;
	    op++;
	    break;

	case 'B':			/* SITBOL/SNOBOL4+: binary */
	case 'b':
	    flags |= FL_BINARY;
	    flags &= ~FL_EOL;
	    op++;
	    break;

	case 'C':			/* SITBOL/SPITBOL: character */
	case 'c':
	    flags |= FL_BINARY;
	    flags &= ~FL_EOL;
#if 0
	    if (recl)			/* already have recl? */
		return FALSE;		/* fail */
#endif /* 0 */
	    recl = 1;
	    op++;
	    break;

	case 'T':			/* SITBOL: "terminal" (no EOL) */
	case 't':
	    flags &= ~FL_EOL;
	    flags |= FL_UNBUF;		/* force prompt output */
	    op++;
	    break;

	case 'Q':			/* SNOBOL4+/SPITBOL: quiet */
	case 'q':
	    flags |= FL_NOECHO;
	    op++;
	    break;

	case 'U':			/* SITBOL/SPITBOL: update */
	case 'u':
	    flags |= FL_UPDATE;
	    op++;
	    break;

	case 'W':			/* SPITBOL: write unbuffered */
	case 'w':
	    flags |= FL_UNBUF;
	    op++;
	    break;

	default:
	    op++;
	    break;
	}
    } /* while *op */

    fp->flags = flags;
    if (rp)
	*rp = recl;
    return TRUE;
}

int
io_openi(dunit, sfile, sopts, drecl)	/* called from SNOBOL INPUT() */
    struct descr *dunit;		/* IN: unit */
    struct spec *sfile;			/* IN: filename */
    struct spec *sopts;			/* IN: options */
    struct descr *drecl;		/* OUT: rec len */
{
    char fname[MAXFNAME];		/* XXX malloc(S_L(sfile)+1)? */
    char opts[MAXOPTS];			/* XXX malloc(S_L(sopts)+1)? */
    struct file *fp;
    FILE *f;
    int unit;
    int recl;

    unit = D_A(dunit);
    unit--;				/* make zero-based */
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    spec2str( sfile, fname, sizeof(fname) );
    spec2str( sopts, opts, sizeof(opts) );

    /* XXX if no sopts;
     * extract options suffix (if any) from filename here?
     */

    if (fname[0]) {
	/* SITBOL takes comma seperated file list */
	fp = io_newfile(fname);
    }
    else {
	fp = io_units[unit].curr;
    }
    if (fp == NULL)
	return FALSE;

    /* process options */
    if (!io_options(fp, opts, &recl))
	return FALSE;

    /* open it now, so we can return status! */
    if (fname[0]) {
	f = io_fopen( fp, "r");
	if (f == NULL) {
	    free(fp);
	    return FALSE;		/* fail; no harm done! */
	}
	io_closeall(unit);
	io_units[unit].curr = io_units[unit].head = fp;
    }

    fp->flags |= FL_INPUT;

    /* pass recl back up */
    D_A(drecl) = recl;
    D_F(drecl) = 0;
    D_V(drecl) = I;

    return TRUE;
} /* io_openi */

int
io_openo(dunit, sfile, sopts)		/* called from SNOBOL OUTPUT() */
    struct descr *dunit;		/* IN: unit */
    struct spec *sfile;			/* IN: filename */
    struct spec *sopts;			/* IN: options */
{
    char fname[MAXFNAME];		/* XXX malloc(S_L(sfile)+1)? */
    char opts[MAXOPTS];			/* XXX malloc(S_L(sopts)+1)? */
    struct file *fp;
    FILE *f;
    int unit;

    unit = D_A(dunit);
    unit--;				/* make zero-based */
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    spec2str( sfile, fname, sizeof(fname) );
    spec2str( sopts, opts, sizeof(opts) );

    /* XXX if no sopts;
     * extract options suffix (if any) from filename here?
     */

    if (fname[0]) {
	/* SITBOL takes comma seperated file list */
	fp = io_newfile(fname);
    }
    else {
	fp = io_units[unit].curr;
    }

    if (fp == NULL)
	return FALSE;			/* fail; no harm done! */

    /* process options */
    if (!io_options(fp, opts, NULL))    /* XXX error if recl set?? */
	return FALSE;

    /* open it now, so we can return status! */
    if (fname[0]) {
	f = io_fopen( fp, "w");
	if (f == NULL)
	    return FALSE;		/* fail; no harm done! */

	io_closeall(unit);
	io_units[unit].curr = io_units[unit].head = fp;
    }
    fp->flags |= FL_OUTPUT;

    return TRUE;
} /* io_openo */

int
io_include( dp, sp )
    struct descr *dp;			/* input unit */
    struct spec *sp;			/* file name (with quotes) */
{
    int l;
    char fname[MAXFNAME];		/* XXX */
    struct file *fp;
    int unit;

    spec2str( sp, fname, sizeof(fname) );

    /* seach includes list to see if file already included!! */
    for (fp = includes; fp; fp = fp->next)
	if (strcmp(fname, fp->fname) == 0) /* found it!!! */
	    return TRUE;

    /* strip off trailing spaces after uniqueness test */
    l = S_L(sp);
    while (l > 0 && fname[l-1] == ' ') {
	l--;
    }
    fname[l] = '\0';

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;

    if (io_fopen( fp, "r") == NULL) {
	char *snolib;
	char fn2[MAXFNAME];		/* XXX */
	extern char *getenv();

	free(fp);

	/* try again, in SNOLIB dir */
	snolib = getenv("SNOLIB");
	if (!snolib)
	    snolib = SNOLIB_DIR;
	strcpy(fn2, snolib);
	strcat(fn2, "/");
	strcat(fn2, fname);

	fp = io_newfile(fn2);
	if (fp == NULL)
	    return FALSE;

	if (io_fopen( fp, "r") == NULL) {
	    free(fp);
	    return FALSE;
	}
    }

    unit = D_A(dp);
    unit--;				/* make zero-based */

    /* push new file onto top of input list */
    fp->next = io_units[unit].curr;
    io_units[unit].curr = fp;

    /* add base filename to list of files already included */
    fp = io_newfile(fname);		/* reuse struct file!! */
    if (fp) {
	fp->next = includes;
	includes = fp;			/* XXX keep per unit? nah. */
    }
    return TRUE;
}

/*
 * retrieve file currently associated with a unit
 * used by compiler to pick up filenames from command line
 */
int
io_file( dp, sp )
    struct descr *dp;			/* IN: unit number */
    struct spec *sp;			/* OUT: filename */
{
    int unit;
    struct file *fp;

    unit = D_A(dp);
    unit--;				/* make zero-based */
    if (BADUNIT(unit) || (fp = io_units[unit].curr) == NULL)
	return FALSE;

    S_A(sp) = (int_t) fp->fname;	/* OY! */
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(fp->fname);
    CLR_S_UNUSED(sp);
    
    return TRUE;
}

/*
 * support for SPITBOL SET() function
 *
 * problems on systems (like bsd44) where sizeof(off_t) > sizeof(int_t)
 * on most hardware...
 */
int
io_seek(dunit, doff, dwhence)
    struct descr *dunit, *doff, *dwhence;
{
    int unit, whence;
    off_t off;
    struct file *fp;
    FILE *f;

    unit = D_A(dunit);
    unit--;				/* make zero-based */
    if (BADUNIT(unit) || (fp = io_units[unit].curr) == NULL)
	return FALSE;

    off = D_A(doff);
    whence = D_A(dwhence);
    if (whence < 0 || whence > 2)
	return FALSE;

    /* translate n -> SEEK_xxx using switch stmt (if SEEK_xxx available)? */

    f = fp->f;
    if (f == NULL)
	return FALSE;

#ifndef NO_UNBUF_RW
    if (fp->flags & FL_UNBUF) {
	off_t pos;

	/* optimized stdio libraries might try to optimize away
	 * the lseek if they've seen no read/write ops!
	 */
	pos = lseek(fileno(f), off, whence);
	if (pos != (off_t)-1)
	    return FALSE;
	D_A(doff) = pos;		/* XXX truncation possible! */
    }
    else
#endif /* NO_UNBUF_RW */
    if (fseek(f, off, whence) == 0)
	D_A(doff) = ftell(f);		/* XXX truncation possible! */
    else
	return FALSE;

    fp->last = LAST_NONE;		/* reset last I/O type */

    return TRUE;
}

/* flush all pending output before system(), exec(), or death */
int
io_flushall(dummy)
    int dummy;
{
    int i;

    for (i = 0; i < NUNITS; i++) {
	struct file *fp;

	fp = io_units[i].curr;
	if (fp) {
	    FILE *f;

	    f = io_units[i].curr->f;
	    if (f) {
		if (fp->flags & FL_TTY)
		    tty_mode(f, 0, 0, 0); /* restore tty settings */
		fflush(io_units[i].curr->f); /* keep err count?? */
	    } /* have f */
	} /* have fp */
    } /* foreach unit */
    return TRUE;
}

/*
 * for PML functions; return a free unit number, returns -1 on failure
 * (use io_mkfile() to attach open file to unit)
 */

#define MINFIND 20			/* minimum unit to return */
#define MAXFIND NUNITS			/* maximum unit to return */

int
io_findunit()
{
    static int finger;
    int start;

    for (;;) {
	if (finger < MINFIND)
	    finger = MAXFIND;

	start = finger;
	while (finger >= MINFIND) {
	    if (io_units[finger-1].curr == NULL &&
		io_units[finger-1].head == NULL)
		return finger--;	/* found a free unit */
	    finger--;
	}

	/*
	 * if we didn't find anything, and we started from scratch,
	 * then we're out of luck.  Only REALLY need to search from
	 * start0 downto MINFIND, then from MAXFIND to start0+1,
	 * but this code is ugly enough
	 */
	if (start == MAXFIND)
	    return -1;

	/* if we didn't start from scratch, then do that */
    }
} /* io_findunit */

/* for PML functions; get current fp on a unit */
FILE *
io_getfp(unit)
    int unit;				/* "external" unit */
{
    unit--;				/* make zero-based */
    if (BADUNIT(unit) || io_units[unit].curr == NULL)
	return NULL;
    return io_units[unit].curr->f;
} /* io_getfp */

/*
 * new 9/9/97
 * Pad listing line out to input record length for "-LIST RIGHT"
 * Not strictly an "I/O" function, but here because the work used
 * to be done in io_read() for all compiler input, regardless of
 * listing on/off and left/right.
 */

int
io_pad(sp, len)
    struct spec *sp;
    int len;
{
    register char *cp;
    register int i;

    cp = S_SP(sp) + S_L(sp);
    for (i = len - S_L(sp); i > 0; i--)
	*cp++ = ' ';
    S_L(sp) = len;

    return 1;				/* for XCALLC */
}

/* new 9/21/97 called from endex() */
int
io_finish() {
    int i;
    
    /* should visit from most recently opened to least recent? */
    for (i = 0; i < NUNITS; i++)
	io_closeall(i);

    return TRUE;
}
