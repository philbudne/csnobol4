/* $Id$ */

#include <varargs.h>
#include <stdio.h>
#ifdef NO_OFF_T
typedef long off_t;
#else
#include <sys/types.h>			/* off_t */
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "h.h"
#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "libret.h"			/* IO_XXX */

/* generated */
#include "data.h"			/* for FILENM */
#include "equ.h"			/* for BCDFLD (for X_LOCSP) */

#ifndef SEEK_SET
#ifdef L_SET
#define SEEK_SET L_SET
#else  /* L_SET not defined */
#define SEEK_SET 0
#endif /* L_SET not defined */
#endif /* SEEK_SET not defined */

#define NUNITS 100			/* XXX set at runtime? */
#define BADUNIT(U) ((U) < 0 || (U) >= NUNITS)

#define STDIN_NAME "stdin"		/* XXX "-" ? */

struct unit {
    struct file *curr;			/* ptr to current file */
    /* for rewind; */
    struct file *head;			/* first file in list */
    off_t offset;			/* offset in first file to rewind to */
};

struct file {
    struct file *next;			/* next input file */
    FILE *f;				/* may be NULL if not (yet) open */
    /* XXX current line number? */
    int flags;				/* FL_xxx */
    /* MUST BE LAST!! */
    char fname[1];
};

#define FL_PIPE	01			/* file was popen'ed */
#define FL_EOL	02			/* strip EOL on input, add on output */
/* XXX raw (binary?) + recl? */

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

/* close currently open file */
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
	if (fp->flags & FL_PIPE)
	    pclose(fp->f);		/* XXX save return? */
	else
#endif
	if (fp->f != stdin &&
	    fp->f != stdout &&
	    fp->f != stderr &&
	    fp->f != termin)		/* XXX check a flag? */
	    fclose(fp->f);		/* XXX save return? */
    }

    up->curr = fp->next;
    return TRUE;			/* XXX */
}

/* close current file, flush current file list */
static int
io_closeall(unit)			/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;

    while (io_units[unit].curr != NULL)
	io_close(unit);

    return TRUE;			/* ?! */
}

static FILE *
io_fopen( fp, mode )
    struct file *fp;
    char *mode;
{
    /*
     * Catspaw style extensions??
     *		parse flags inside []
     *		if fname[0] == '!' use popen!!
     * SITBOL style;
     *		take seperate format string
     *		INPUT() takes comma seperated list
     */
#ifndef NO_POPEN
    /* filename with leading '|' opens a pipe! */
    if (fp->fname[0] == '|') {
	fp->flags |= FL_PIPE;
	return (fp->f = popen(fp->fname+1, mode));
    }
#endif
    /* filename "-" goes to stdin/out */
    if (strcmp(fp->fname,"-") == 0) {
	if (mode[0] == 'r')
	    return (fp->f = stdin);
	return (fp->f = stdout);
    }
    if (strcmp(fp->fname,"/dev/stdin") == 0)
	return (fp->f = stdin);
    if (strcmp(fp->fname,"/dev/stdout") == 0)
	return (fp->f = stdout);
    if (strcmp(fp->fname,"/dev/stderr") == 0)
	return (fp->f = stderr);

    /* XXX more hacks here? /dev/fd/n? /tcp ??? */

    return (fp->f = fopen(fp->fname, mode));
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

/* support for io_init */
static void
io_mkfile( unit, f, fname )
    int unit;
    FILE *f;
    char *fname;
{
    struct file *fp;
    struct unit *up;

    fp = io_newfile(fname);
    if (fp == NULL) {
	perror("could not set up files");
	exit(1);
    }

    fp->f = f;
    up = io_units + unit - 1;
    up->head = up->curr = fp;
    up->offset = 0;
}

/*
 * implement SIL operations;
 */

void
io_init()				/* here from INIT */
{
    if (io_units[UNITI-1].curr == NULL) { /* no input file(s)? */
	io_mkfile(UNITI, stdin, STDIN_NAME );
    }
    else {
	if (!io_next(UNITI-1)) {
	    perror(io_units[UNITI-1].curr->fname);
	    exit(1);
	}
    }

    /* XXX support -o outputfile? */
    io_mkfile(UNITO, stdout, "stdout");

    io_mkfile(UNITP, stderr, "stderr");

    /*
     * tempting to overload UNITP for input/output
     * this might work on Unix (need to freopen stderr for update?)
     * but is bound to cause trouble on some other system.
     */
    termin = term_input();
    if (termin) {
	io_mkfile(UNITT, termin, "termin");
    }

    compiling = 1;
}

/* limited printf */
void
io_printf(va_alist)			/* OUTPUT */
    va_dcl
{
    va_list vp;
    int unit;
    char *format;
    register char c;
    FILE *f;
    char line[1024];
    char *lp;

    va_start(vp);

    unit = va_arg(vp, int);
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
}

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
	fwrite( cp, 1, len, f );
    }
    if (fp->flags & FL_EOL)
	putc( '\n', f );
}

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

	/* XXX check if binary mode, use fread & set len */
	if (fgets(cp, recl, f) != NULL)
	    break;

	/* here when read failed */
	if (feof(f)) {
	    if (!io_next(unit)) {
		/* XXX perror? */
		return IO_EOF;
	    }
	    if (COMPILING(unit)) {
#if 0
		/* let compiler know we've changed files */
		D_A(FILENM) = 0;	/* zap filename! */
		D_A(LNNOCL) = -1;	/* and source line number */
#endif
		/* force call to INCCK to pop old FILENM and LNNOCL */
		return IO_EOF;
	    }
	    continue;			/* try again! */
	}
	/* wasn't eof?! */
	return IO_ERR;
    }

    if (fp->flags & FL_EOL) {
	/* strip off EOL */
	len = strlen(cp);
	if (cp[len-1] == '\n') {
	    len--;
	    cp[len] = '\0';
	}
	/* XXX increment line number (fp->line) ? */
    }

    if (compiling) {
	/* UGH: compiler ignores output length
	 * (in fact it depends on us not touching it, so
	 * pad out to recl with spaces!
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
	if (up->curr != NULL)		/* file is openbut not first in list */
	    io_close(unit);		/* close it */
	up->curr = up->head;		/* reset to head of list */
	io_fopen(up->curr, "r");	/* XXX use original mode? */
    }
    fp = up->curr;
    if (fp == NULL)
	return;

    f = fp->f;
    if (f == NULL || (fp->flags & FL_PIPE) == 0) {
	fseek(f, up->offset, SEEK_SET);
    }
}

/* extensions; */

void
io_ecomp()				/* XECOMP */
{
    struct unit *up;

    compiling = 0;

    if (rflag == 0) {
	/* if -r was not given, switch INPUT to stdin!! */

	io_closeall(UNITI-1);
	io_mkfile( UNITI, stdin, STDIN_NAME );
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
    up->head = up->curr;		/* save file for rewind */
}

int
io_openi(dp, sp)			/* XOPENI */
    struct descr *dp;
    struct spec *sp;
{
    char fname[1024];			/* XXX */
    struct file *fp;
    FILE *f;
    int unit;

    if (S_L(sp) == 0) {			/* no file? */
	/* XXX use file (if any) from -UNIT=file from command line? */
	return 1;			/* true (success) */
    }

    unit = D_A(dp);
    unit--;				/* make zero based */
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    strncpy( fname, S_SP(sp), S_L(sp) );
    fname[S_L(sp)] = '\0';

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;

    /* XXX let io_read do the work??? */
    f = io_fopen( fp, "r");
    if (f == NULL) {
	free(fp);
	return FALSE;			/* fail; no harm done! */
    }

    io_closeall(unit);
    io_units[unit].curr = fp;

    return TRUE;
}

int
io_openo(dp, sp)			/* XOPENO */
    struct descr *dp;
    struct spec *sp;
{
    char fname[1024];			/* XXX */
    struct file *fp;
    FILE *f;
    int unit;

    if (S_L(sp) == 0) {			/* no file name? */
	/* XXX use file (if any) from -UNIT=file from command line? */
	return TRUE;
    }

    unit = D_A(dp);
    unit--;
    if (BADUNIT(unit))
	return FALSE;			/* fail */

    strncpy( fname, S_SP(sp), S_L(sp) );
    fname[S_L(sp)] = '\0';

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;			/* fail; no harm done! */

    f = io_fopen( fp, "w");
    if (f == NULL)
	return FALSE;			/* fail; no harm done! */

    io_closeall(unit);
    io_units[unit].curr = fp;

    return TRUE;
}

int
io_include( dp, sp )
    struct descr *dp;			/* input unit */
    struct spec *sp;			/* file name (with quotes) */
{
    int l;
    char fname[256];			/* XXX */
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
	char fn2[512];			/* XXX */
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
