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
#include "data.h"			/* for FILENM */
#include "equ.h"			/* for BCDFLD (for X_LOCSP) */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET not defined */

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif /* SEEK_CUR not defined */

#ifndef SEEK_END
#define SEEK_END 2
#endif /* SEEK_END not defined */

#define NUNITS 100			/* XXX set at runtime? */
#define BADUNIT(U) ((U) < 0 || (U) >= NUNITS)

/* names associated with UNITI, UNITO, UNITP(!), UNITT */
#define STDIN_NAME  "stdin"		/* XXX "-" ? */
#define STDOUT_NAME "stdout"		/* XXX "-" ? */
#define STDERR_NAME "stderr"
#define TERMIN_NAME "termin"		/* terminal input */

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

#define FL_PIPE		01		/* file was popen'ed */
#define FL_EOL		02		/* strip EOL on input, add on output */
#define FL_BINARY	04		/* binary: no EOL; use recl */
#define FL_UPDATE	010		/* update: read+write */
#define FL_UNBUF	020		/* unbuffered write */
#define FL_APPEND	040		/* append */
#define FL_TTY		0100		/* is a tty */
#define FL_NOECHO	0200		/* tty: no echo */

/* XXX opened by INPUT/OUTPUT flag? */

/* XXX malloc at runtime? */
static struct unit io_units[NUNITS];

static struct file *includes;		/* list of included files */
static int compiling;			/* TRUE iff compiler still running */
static FILE *termin;			/* TERMINAL input */

extern int rflag;			/* from init.c */
extern FILE *term_input();		/* from <system>/term.c */
extern void *malloc();

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

} /* io_addfile */

/* close currently open file on a unit */
/* XXX take flag: to free struct file, or not? */
static int
io_close(unit)				/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;
    struct unit *up;

    up = io_units + unit;
    fp = up->curr;
    if (fp == NULL)
	return TRUE;

    if (fp->f) {
#ifndef NO_POPEN
	if (fp->flags & FL_PIPE) {
	    pclose(fp->f);		/* XXX save return? */
	    fp->f = NULL;
	}
	else
#endif /* NO_POPEN not defined */
	if (fp->f != stdin &&
	    fp->f != stdout &&
	    fp->f != stderr &&
	    fp->f != termin) {		/* XXX check a flag? */

	    /* XXX call close hook? */
	    if (fp->flags & FL_TTY)
		tty_close(fp->f);	/* advisory */

	    fclose(fp->f);		/* XXX save return? */
	    fp->f = NULL;
	}
    }

    up->curr = fp->next;
    return TRUE;			/* XXX */
}

/* close a unit, flush current file list */
int
io_closeall(unit)			/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;

    while (io_units[unit].curr != NULL)
	io_close(unit);			/* XXX check for error? */

    io_units[unit].curr = io_units[unit].head = NULL;

    return TRUE;
}

static FILE *
io_fopen2( fp, mode )
    struct file *fp;
    char *mode;
{
    char *mp;
    char buf[4];			/* x+b<NUL> */

    /* handle magic filenames (have a table (prefix or full str)??) */

#ifndef NO_POPEN
    /* filename with leading '|' opens a pipe! */
    /* SPITBOL: leading '!' means pipe, (with escaping?) */
    if (fp->fname[0] == '|') {
	fp->flags |= FL_PIPE;
	/* XXX set close hook? */
	return (popen(fp->fname+1, mode));
    }
#endif /* NO_POPEN not defined */

    /* filename "-" goes to stdin/out */
    if (strcmp(fp->fname,"-") == 0) {
	if (mode[0] == 'r')
	    return (stdin);
	return (stdout);
    }
    if (strcmp(fp->fname,"/dev/stdin") == 0)
	return (stdin);
    if (strcmp(fp->fname,"/dev/stdout") == 0)
	return (stdout);
    if (strcmp(fp->fname,"/dev/stderr") == 0)
	return (stderr);
    if (strncmp(fp->fname, "/tcp/", 5) == 0 ||
	strncmp(fp->fname, "/udp/", 5) == 0) {
	char fn2[1024];			/* XXX */
	char *host, *service, *cp;
	int priv;
	extern FILE *tcp_open(), *udp_open();

	priv = 0;
	strcpy( fn2, fp->fname + 5 );	/* XXX strdup()? */
	host = fn2;
	service = index(host, '/');
	if (service == NULL)
	    return NULL;
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
	    return udp_open( host, service, -1, priv );
	else
	    return tcp_open( host, service, -1, priv );
    }

    /* XXX more hacks here? /dev/fd/n? ??? */

    /* create full mode string for fopen() */
    mp = buf;
    if (mode[0] == 'w' && (fp->flags & FL_APPEND))
	*mp++ = 'a';
    else
	*mp++ = mode[0];
    if (fp->flags & FL_UPDATE)
	*mp++ = '+';
#ifndef NO_FOPEN_B
    if (fp->flags & FL_BINARY)
	*mp++ = 'b';
#endif /* NO_FOPEN_B not defined */
    *mp++ = '\0';

    return (fopen(fp->fname, buf));
}

static FILE *
io_fopen( fp, mode )
    struct file *fp;
    char *mode;
{
    fp->f = io_fopen2(fp,mode);
    if (fp->f == NULL)
	return NULL;

    if (fisatty(fp->f)) {
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
}

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
	io_close(unit);

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
int
io_mkfile( unit, f, fname )
    int unit;				/* external (1-based) unit */
    FILE *f;
    char *fname;			/* "filename" for error reports */
{
    struct file *fp;
    struct unit *up;

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;
    fp->f = f;
    if (fisatty(f)) {
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

/*
 * implement SIL operations;
 */

void
io_init()				/* here from INIT */
{
    if (io_units[UNITI-1].curr == NULL) { /* no input file(s)? */
	if (!io_mkfile(UNITI, stdin, STDIN_NAME )) {
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

    if (!io_mkfile(UNITO, stdout, STDOUT_NAME)) {
	perror("io_mkfile(stdout)");
	exit(1);
    }

    if (!io_mkfile(UNITP, stderr, STDERR_NAME)) {
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
	if (!io_mkfile(UNITT, termin, TERMIN_NAME)) {
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

    unit--;
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
	int wid;

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
io_print( iob, sp )			/* STPRNT */
    struct descr **iob;
    struct spec *sp;
{
    int unit;
    struct file *fp;
    FILE *f;
    /*
     * (*iob)[0]	title descr
     * (*iob)[1]	integer unit number
     * (*iob)[2]	pointer to natural var for format
     */

    unit = D_A(*iob + 1);
    unit--;

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
     * performed using read()/write() (see comment on UNBUF below)
     */

    if ((fp->flags & FL_UPDATE) && fp->last == LAST_INPUT) {
	fseek(f, 0, SEEK_CUR);		/* seek relative by zero */
    }
    fp->last = LAST_OUTPUT;

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

	/* XXX check FL_UNBUF; write(fileno(f), cp, recl)? */
	fwrite( cp, 1, len, f );
    } /* have string */
    if (fp->flags & FL_EOL)
	putc( '\n', f );

    if (fp->flags & FL_UNBUF)
	fflush(f);
} /* io_print */

void
io_endfile(unit)			/* ENFILE */
    int unit;
{
    unit--;
    if (BADUNIT(unit) || io_units[unit].curr == NULL)
	return;

    io_close(unit);
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
    unit--;
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

	if (fp->flags & FL_TTY) {
	    tty_mode( fp->f,
		     (fp->flags & FL_BINARY) != 0, /* raw (have FL_CHAR?) */
		     (fp->flags & FL_NOECHO) != 0, /* noecho */
		     recl
		     );
	}

	/*
	 * ANSI C requires that a file positioning function intervene
	 * between output and input. Would not be needed if UPDATE I/O
	 * performed using read()/write() (see comment on UNBUF below)
	 */

	if ((fp->flags & FL_UPDATE) && fp->last == LAST_OUTPUT) {
	    fseek(f, 0, SEEK_CUR);	/* seek relative by zero */
	}
	fp->last = LAST_INPUT;

	/* XXX check FL_UNBUF; read(fileno(f), cp, recl)? */
	if (fp->flags & FL_BINARY) {
	    len = fread(cp, 1, recl, f);
	    if (len > 0)
		break;
	}
	else {
	    if (fgets(cp, recl, f) != NULL) {
		len = strlen(cp);
		break;
	    }
	}

	/* here when read failed */
	if (feof(f)) {
	    if (!io_next(unit)) {
		/* XXX perror? */
		return IO_EOF;
	    }
	    if (COMPILING(unit)) {
		/* force call to INCCK to pop old FILENM and LNNOCL */
		return IO_EOF;
	    }
	    continue;			/* try again! */
	} /* feof */

	/* wasn't eof?! */
	return IO_ERR;
    } /* forever */

    if (fp->flags & FL_EOL) {
	/* strip off EOL */
	if (cp[len-1] == '\n') {
	    len--;
	    cp[len] = '\0';
	}
	/* XXX increment line number (fp->line) ? */
    } /* FL_EOL */

    if (compiling) {
	/*
	 * UGH: compiler ignores output length (it depends on us not
	 * touching it), so pad out to recl with spaces!
	 */
	cp += len;			/* skip over data */
	while (len++ < recl)
	    *cp++ = ' ';
    }
    else {
	S_L(sp) = len;
    }

    return IO_OK;
}

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

    compiling = 0;			/* turn off crocks for compiler */

    if (rflag == 0) {
	/* if -r was not given, switch INPUT to stdin!! */

	io_mkfile( UNITI, stdin, STDIN_NAME ); /* XXX check return?! */
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
    up->offset = ftell(up->curr->f);	/* save offset for rewind */
    /* XXX free source file blocks? */
    up->head = up->curr;		/* save file for rewind */

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
	    op++;
	    break;

	case 'Q':			/* SNOBOL4+/SPITBOL: update */
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
    char fname[1024];			/* XXX malloc(S_L(sfile)+1)? */
    char opts[1024];			/* XXX malloc(S_L(sopts)+1)? */
    struct file *fp;
    FILE *f;
    int unit;
    int recl;

    unit = D_A(dunit);
    unit--;				/* make zero based */
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    strncpy( fname, S_SP(sfile), S_L(sfile) );
    fname[S_L(sfile)] = '\0';

    strncpy( opts, S_SP(sopts), S_L(sopts) );
    opts[S_L(sopts)] = '\0';

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
    char fname[1024];			/* XXX malloc(S_L(sfile)+1)? */
    char opts[1024];			/* XXX malloc(S_L(sopts)+1)? */
    struct file *fp;
    FILE *f;
    int unit;

    unit = D_A(dunit);
    unit--;
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    strncpy( fname, S_SP(sfile), S_L(sfile) );
    fname[S_L(sfile)] = '\0';

    strncpy( opts, S_SP(sopts), S_L(sopts) );
    opts[S_L(sopts)] = '\0';

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

    return TRUE;
} /* io_openo */

int
io_include( dp, sp )
    struct descr *dp;			/* input unit */
    struct spec *sp;			/* file name (with quotes) */
{
    int l;
    char fname[1024];			/* XXX */
    struct file *fp;
    int unit;

    l = S_L(sp);
    if (l > sizeof(fname)-1)
	l = sizeof(fname)-1;		/* ?! */
    strncpy( fname, S_SP(sp), l );
    fname[l] = '\0';

    /* seach includes list to see if file already included!! */
    for (fp = includes; fp; fp = fp->next)
	if (strcmp(fname, fp->fname) == 0) /* found it!!! */
	    return TRUE;

    /* strip off trailing spaces after uniqueness test */
    while (l > 0 && fname[l-1] == ' ') {
	l--;
    }
    fname[l] = '\0';

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;

    if (io_fopen( fp, "r") == NULL) {
	char *snolib;
	char fn2[1024];			/* XXX */
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
    unit--;

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

/* retrieve file currently associated with a unit */
int
io_file( dp, sp )
    struct descr *dp;			/* IN: unit number */
    struct spec *sp;			/* OUT: filename */
{
    int unit;
    struct file *fp;

    unit = D_A(dp);
    unit--;
    if (BADUNIT(unit) || (fp = io_units[unit].curr) == NULL)
	return 0;

    S_A(sp) = (int_t) fp->fname;	/* OY! */
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(fp->fname);
    CLR_S_UNUSED(sp);
    
    return 1;
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
    unit--;
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

    if (fseek(f, off, whence) < 0)
	return FALSE;
    fp->last = LAST_NONE;		/* reset last I/O type */

    D_A(doff) = ftell(f);		/* XXX truncation possible! */
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

int
io_findunit()
{
    register i;

    /* XXX keep static counter so we rotate thru unit space? */
    for (i = NUNITS; i > 0; i--) {

	/* avoid preassigned units */
	switch (i) {
	case UNITI:
	case UNITO:
	case UNITP:
	case UNITT:
	    continue;
	}

	if (io_units[i-1].curr == NULL && io_units[i-1].head == NULL)
	    return i;
    }
    return -1;
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

