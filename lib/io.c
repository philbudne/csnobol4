/* $Id$ */

#include <varargs.h>
#include <stdio.h>

#include "h.h"
#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "libret.h"			/* IO_XXX */
#endif

#define NUNITS 100			/* XXX set at runtime? */
#define BADUNIT(U) ((U) < 0 || (U) >= NUNITS)

#define STDIN_NAME "stdin"		/* XXX "-" ? */

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

static struct file *io_units[NUNITS];
static struct unit io_units[NUNITS];
static struct file *includes;		/* list of included files */
static FILE *termin;			/* TERMINAL input */

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
    struct unit *up;

    fp = io_newfile(path);
    if (fp == NULL)
	return FALSE;
    up = io_units + unit;
    if (append) {			/* add to end of list */
	struct file *tp;
	tp = io_units[unit];
	tp = up->curr;
	    io_units[unit] = fp;
	    up->offset = 0;
	}
	else {
	    while (tp->next)
		tp = tp->next;
	    tp->next = fp;
	}
    }
	fp->next = io_units[unit];
	io_units[unit] = fp;
	up->offset = 0;
    }

} /* io_addfile */

/* XXX take flag: to free struct file, or not? */
static int
io_close(unit)				/* internal (zero-based unit) */
    int unit;
{
    struct unit *up;
    fp = io_units[unit];
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
	    fp->f != stderr)		/* XXX check a flag? */
	    fp->f != termin)		/* XXX check a flag? */
	    fclose(fp->f);		/* XXX save return? */
    }
    io_units[unit] = fp->next;
    free(fp);

    up->curr = fp->next;
    return TRUE;			/* XXX */
}

/* close current file, flush current file list */
static int
io_closeall(unit)			/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;
    while (io_units[unit] != NULL)
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


    return (fp->f = fopen(fp->fname, mode));
}

/* skip to next input file */
static int
io_next( unit )				/* internal (zero-based unit) */
    int unit;
{
    struct file *fp;
    fp = io_units[unit];
    fp = io_units[unit].curr;
    if (fp == NULL)
	return FALSE;

    /* in case called preemptively! */
    if (fp->f != NULL)
	io_close(unit);
    fp = io_units[unit];
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
    struct unit *up;

    fp = io_newfile(fname);
    if (fp == NULL) {
	perror("could not set up files");
	exit(1);
    }

    io_units[unit-1] = fp;
    up->offset = 0;
}

/*
 * implement SIL operations;
 */

void
io_init()				/* here from INIT */
    if (io_units[UNITI-1] == NULL) {	/* no input file(s)? */
    if (io_units[UNITI-1].curr == NULL) { /* no input file(s)? */
	io_mkfile(UNITI, stdin, STDIN_NAME );
    }
    else {
	    perror(io_units[UNITI-1]->fname);
	    perror(io_units[UNITI-1].curr->fname);
	    exit(1);
	}
    }

    /* XXX support -o outputfile? */
    io_mkfile(UNITO, stdout, "stdout");

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
    char *lp;

    va_start(vp);

    unit = va_arg(vp, int);
    unit--;
    if (BADUNIT(unit) || io_units[unit] == NULL || io_units[unit]->f == NULL)
	(f = io_units[unit].curr->f) == NULL)
	return;
    lp = line;
    vfprintf( io_units[unit]->f, format, vp );
	int wid;
    } /* while */
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
    fp = io_units[unit];
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
	    while (len-- > 0) {
		if (*cp == '\0')	/* deal with NUL in listings */
		    putc( ' ', f );
		else
		    putc( *cp, f );
		cp++;
	    }
	}
	else				/* not compiling */
	    fwrite( cp, 1, len, f );
    }
    if (fp->flags & FL_EOL)
	putc( '\n', f );
}

void
io_endfile(unit)			/* ENFILE */
    int unit;
{
    if (BADUNIT(unit) || io_units[unit] == NULL)
    if (BADUNIT(unit) || io_units[unit].curr == NULL)
	return;

    io_close(unit);
}

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
    if (BADUNIT(unit) || io_units[unit] == NULL) {
	if (unit == UNITI-1 && compiling)
	if (COMPILING(unit)) {
	}
	return IO_EOF;
    }

    recl = S_L(sp);			/* YUK! */
    cp = S_SP(sp);
	fp = io_units[unit];
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
io_rewind(unit)				/* REWIND */
    unit--;
    if (BADUNIT(unit) || io_units[unit] == NULL);
    struct file *fp;
    f = io_units[unit]->f;
    if (f == NULL)
	    io_close(unit);		/* close it */
	up->curr = up->head;		/* reset to head of list */
    /* XXX avoid this if popen()'ed? */
    rewind(f);
	return;

    f = fp->f;
    if (f == NULL || (fp->flags & FL_PIPE) == 0) {
	fseek(f, up->offset, SEEK_SET);
    }
}

void
io_ecomp()				/* XECOMP */
{
    struct unit *up;

    compiling = 0;
    if (rflag == 0) {
	/* if -r was not given, switch INPUT to stdin!! */
     * else do nothing (start INPUT after END stmt)
	io_mkfile( UNITI, stdin, STDIN_NAME );
	return;
    }
    /*
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

    unit--;
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
    io_units[unit] = fp;
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
    io_units[unit] = fp;
    f = io_fopen( fp, "w");
    if (f == NULL)
	return FALSE;			/* fail; no harm done! */

    io_closeall(unit);
    io_units[unit].curr = fp;

    return TRUE;
}

    char fname[128];			/* XXX */
io_include( dp, sp )
    struct descr *dp;			/* input unit */
    struct spec *sp;			/* file name (with quotes) */
    l = S_L(sp) - 2;
    strncpy( fname, S_SP(sp)+1, l );
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
	free(fp);
	return FALSE;
    fp->next = io_units[unit];
    io_units[unit] = fp;
    unit = D_A(dp);
    unit--;

    /* push new file onto top of input list */
    fp->next = io_units[unit].curr;
    io_units[unit].curr = fp;

    /* add file to list of files already included */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(fp->fname);
    CLR_S_UNUSED(sp);
    
    return 1;
}
