/* $Id$ */

/*
 * I/O support for CSNOBOL4
 *
 * FINALLY refactored in 2020 (last attempted in 2002, but never debugged)
 * this file is now an adaptation layer over 
 * I/O Objects (and some nastiness has moved to stdio_obj.c!)
 *
 * Still the largest file in the support library, and still has manu
 * ifdefs.  The complexity (and fragility) of the I/O support is
 * due to a number of factors:
 *
 * Assumptions built into the SIL source (both compiler and runtime) e.g.
 * * compiler insists lines end with a space, appear in designated buffer
 * * Multiple layers of I/O:
 *  A single disk file or device might be associated with one or more:
 *  + SNOBOL variable associations (w/ record length, unit number)
 *  + FORTRAN unit numbers
 *  + stdio FILE streams
 *  + POSIX file descriptors and/or C runtime system handles/channels
 *  + open file object in system space
 * * Interactions of (extensive/excessive) I/O options/flags
 * * Handling file lists.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdarg.h>
#include <stdlib.h>		       /* before stdio(?) */
#include <stdio.h>
#include <ctype.h>

#include "h.h"
#include "units.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "lib.h"
#include "inet.h"			/* INET_XXX */
#include "libret.h"			/* IO_xxx, INC_xxx */
#include "str.h"
#include "io_obj.h"			/* io_obj, FL_xxx */
#include "stdio_obj.h"			/* stdio_{wrap,obj} */
#include "globals.h"			/* rflag, lflag */

/* generated */
#include "equ.h"			/* for BCDFLD (for X_LOCSP), res.h */
#include "res.h"			/* needed on VAX/VMS for data.h */
#include "data.h"			/* for FILENM */
#include "proc.h"			/* UNDF() */

#ifdef COMPILER_READLINE		/* after proc.h */
#undef RETURN
#include <readline/readline.h>
#include <readline/history.h>
#endif /* COMPILER_READLINE */

#ifndef PRELOAD_FILENAME
#define PRELOAD_FILENAME "preload.sno"
#endif

#define ISTTY(FP) ((FP)->iop && ((FP)->iop->flags & FL_TTY))

/* GOAL: "struct unit" does not leave io.c */
struct unit {
    struct file *curr;			/* ptr to current file */
    /* for rewind; */
    struct file *head;			/* first file in list */
    io_off_t offset;			/* offset in "head" to rewind to */
    /*
     * PLB 2020-09-11 keep flags & recl HERE??
     * would only matter if INPUT/OUTPUT calls allowed lists of files
     * (like SITBOL)?
     */
};

/* GOAL: "struct file" does not leave io.c
 * Represents a named file from the command line, an include file,
 *	INPUT or OUTPUT call (or passed into a DLL)
 *
 * Open files are represented by io_obj.
 */
struct file {
    struct file *next;			/* next input file */
    struct io_obj *iop;
    int flags;				/* XXX temp? pull up?? */

    /* MUST BE LAST!! */
    char fname[1];
};

#define MAXFNAME	1024		/* XXX use MAXPATHLEN? POSIX?? */
#define MAXOPTS		1024

static VAR struct unit units[NUNITS];	/* XXX malloc at runtime? */
static VAR struct file *includes;	/* list of included files */
static VAR int finger;			/* for io_findunit */
static VAR struct file *lib_dirs;	/* list of include directories */
static VAR struct file *lib_dir_last;	/* tail of include directory list */

/*
 * private, r/o array of pointers to io_open functions
 * taking filename, flags, 'r' or 'w'
 * returning pointers to io_obj
 */
static struct io_obj *(*const io_open_funcs[])(char *fname, int flags, int rw) =
{
#ifdef OSDEPIO_OBJ
    osdepio_open,			/* local I/O that can't be wrapped */
#endif
    ptyio_open,				/* dummy ptyio_open available */
    pipeio_open,			/* dummy popen available */
#ifdef INET_IO
    inetio_open,			/* e.g. winsockets */
#endif
    stdio_open				/* LAST! Never returns NOMATCH!! */
};
#define N_OPEN_FUNCS (sizeof(io_open_funcs)/sizeof(io_open_funcs[0]))

/* convert to internal (zero based) unit number; */
#define INTERN(U) ((U)-1)

/* check a (zero based) unit number; */
#define BADUNIT(U) ((U) < 0 || (U) >= NUNITS)

/*
 * take internal (zero-based) unit number; return "struct unit *"
 * all access to units array hidden, so it can be made sparse
 */
#define FINDUNIT(N) (units + (N))

struct io_obj nomatch;			/* for NOMATCH */

/****************
 * io_obj wrappers
 *
 * for efficiency, could leave io_ops structs writable (non-const)
 * and drag up (swizzle?) the superclass method pointer on first use.
 * BUT, MOST I/O will be thru stdio_ops, which is fully populated.
 * Currently, only oddball things like pipes, ptys and winsock are layered.
 */

/* leaves input in iop->linebuf (expanded as needed) */
static ssize_t
ioo_getline(struct io_obj *iop) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_getline)
	    return (op->io_getline)(iop);

    return -1;
}

static ssize_t
ioo_read_raw(struct io_obj *iop, char *buf, size_t len) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_read_raw)
	    return (op->io_read_raw)(iop, buf, len);

    return -1;
}

static ssize_t
ioo_write(struct io_obj *iop, char *buf, size_t len) {
    const struct io_ops *op;

    if (!iop)
	return -1;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_write)
	    return (op->io_write)(iop, buf, len);

    return -1;
}

static int
ioo_flush(struct io_obj *iop) {
    const struct io_ops *op;
    for (op = iop->ops; op; op = op->io_super)
	if (op->io_flush)
	    return (op->io_flush)(iop);

    return FALSE;
}

static int
ioo_seeko(struct io_obj *iop, io_off_t off, int whence) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_seeko)
	    return (op->io_seeko)(iop, off, whence);

    /* in case not implemented: the expected response */
    return FALSE;
}

static io_off_t
ioo_tello(struct io_obj *iop) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_tello)
	    return (op->io_tello)(iop);

    /* in case not implemented: the expected response */
    return -1;
}

static int
ioo_eof(struct io_obj *iop) {
    const struct io_ops *op;

    for (op = iop->ops; op; op = op->io_super)
	if (op->io_eof)
	    return (op->io_eof)(iop);

    return FALSE;			/* treat as non-EOF I/O error */
}

static int
ioo_close(struct io_obj *iop) {
    const struct io_ops *op;
    int ret = TRUE;

    if (!iop)
	return FALSE;

    for (op = iop->ops; op; op = op->io_super) {
	if (op->io_close) {
	    ret = (op->io_close)(iop);
	    break;
	}
    }
    if (iop->linebuf)
	free(iop->linebuf);
    free(iop);
    return ret;
}

/* internal helper; take internal unit, return struct file *, or NULL */
static struct file *
findfile(int iunit) {
    struct unit *up;

    if (BADUNIT(iunit))
	return NULL;

    up = FINDUNIT(iunit);
    return up->curr;			/* may be NULL */
}

/*
 * this is the ONE place that allocates a "struct file"
 * path is saved at the end of the struct
 * (so only one free(fp) is needed)
 */
static struct file *
io_newfile(char *path) {
    struct file *fp;

    fp = (struct file *) malloc( sizeof( struct file ) + strlen(path) );
    if (fp == NULL)
	return NULL;

    bzero( (char *)fp, sizeof (struct file) );
    strcpy(fp->fname,path);
    return fp;
}

#ifdef SHARED
static struct file *
io_memfile(char *name, char *data, int len, int dir) {
    struct file *fp;

    fp = io_newfile(name);
    if (!fp)
	return NULL;

    fp->iop = memio_open(data, len, 0, dir);
    if (!fp->iop) {
	free(fp);
	return NULL;
    }
    fp->iop->fname = fp->fname;		/* "borrowed" */
    return fp;
}
#endif /* SHARED defined */

void
io_initvars(void) {
    /* XXX cleanup here? */
}

/* add file to input list */
/* calls made here BEFORE io_init() called! */
static int
io_addfile(int unit, struct file *fp, int append) {
    /* XXX check for commas in path? */
    struct unit *up;

    io_initvars();

    /* XXX allocate units array here? */

    up = FINDUNIT(unit);
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
io_close(int unit) {		      /* internal (zero-based unit) */
    struct file *fp;
    struct unit *up;
    int ret = TRUE;

    up = FINDUNIT(unit);
    fp = up->curr;
    if (fp == NULL)
	return TRUE;

    if (fp->iop) {
	ret = ioo_close(fp->iop);
	fp->iop = NULL;
    }
    up->curr = fp->next;
    return ret;
} /* io_close */

/* close a unit, flush current file list */
EXPORT(int)
io_closeall(int unit) {			/* internal (zero-based unit) */
    struct file *fp, *next;
    struct unit *up;
    int ret;

    /* close any/all open files on chain */
    ret = TRUE;
    up = FINDUNIT(unit);
    while (up->curr != NULL)
	if (!io_close(unit))
	    ret = FALSE;

    /* free up all file structs */
    fp = up->head;
    while (fp != NULL) {
	next = fp->next;
	free(fp);
	fp = next;
    }
    up->curr = up->head = NULL;

    return ret;
}

static struct io_obj *
io_fopen(struct file *fp,
	 int dir) {			/* 'r' or 'w' */
    int i;

    for (i = 0; i < N_OPEN_FUNCS; i++) {
	struct io_obj *iop = (io_open_funcs[i])(fp->fname, fp->flags, dir);
	if (iop == NOMATCH)
	    continue;
	fp->iop = iop;
	if (iop)
	    iop->fname = fp->fname;	/* borrow pointer */
	return fp->iop;
    }
    /* should not happen: stdio should never return NOMATCH */
    fp->iop = NULL;
    return NULL;
} /* io_fopen */

/* skip to next input file */
static int
io_next(int unit) {		      /* internal (zero-based unit) */
    struct file *fp;
    struct unit *up;

    up = FINDUNIT(unit);
    fp = up->curr;
    if (fp == NULL)
	return FALSE;

    /* in case called preemptively! */
    if (fp->iop != NULL)
	io_close(unit);			/* close, and advance */

    /* get new current file (io_close advances to next file in list) */
    fp = up->curr;
    if (fp == NULL)
	return FALSE;

    if (fp->iop != NULL)		/* already open? */
	return TRUE;

    /* XXX let io_read() do the work??? */
    /* XXX copy flags from previous file? */
    io_fopen( fp, 'r');

    return fp->iop != NULL;
} /* io_next */


/* skip to next file, for external use, takes external unit */
EXPORT(int)
io_skip(int unit) {
    return io_next(INTERN(unit));
}

/* here with filename from command line */
EXPORT(void)
io_input_file(char *path) {
    struct file *fp;

    fp = io_newfile(path);
    if (fp == NULL)
	return;

    io_addfile( INTERN(UNITI), fp, TRUE );	/* append to list! */
}

#ifdef SHARED
EXPORT(void)
io_input_string(char *name, char *str) {
    struct file *fp;

    fp = io_memfile(name, str, strlen(str), 'r');
    if (fp == NULL)
	return;

    io_addfile( INTERN(UNITI), fp, TRUE );	/* append to list! */
}
#endif /* SHARED defined */

/* attach a "struct file" to a unit (external) */
static void
io_setfile(int unit, struct file *fp) {
    struct unit *up;

    unit = INTERN(unit);
    io_closeall(unit);			/* close unit */

    up = FINDUNIT(unit);
    up->head = up->curr = fp;
    up->offset = 0;
}

/* setup a unit given an open stdio stream and a "filename" */
static int
io_mkfile2(int unit,			/* external (1-based) unit */
    FILE *f,
    char *fname,			/* "filename" for error reports */
    int flags) {
    struct file *fp;

    fp = io_newfile(fname);
    if (fp == NULL)
	return FALSE;
    fp->flags |= flags;
    fp->iop = stdio_wrap(fp->fname, f, 0, NULL, fp->flags);
    io_setfile(unit, fp);
    return TRUE;
}

EXPORT(int)
io_mkfile(int unit,			/* external (1-based) unit */
	  FILE *f,
	  char *fname) {		/* "filename" for error reports */
    return io_mkfile2( unit, f, fname, 0 );
}

EXPORT(int)
io_mkfile_noclose(int unit,		/* external (1-based) unit */
		  FILE *f,
		  char *fname) {	/* "filename" for error reports */
    return io_mkfile2( unit, f, fname, FL_NOCLOSE );
}

/* return true if unit attached */
EXPORT(int)
io_attached(int unit) {
    struct unit *up = FINDUNIT(INTERN(unit));
    return up->curr != NULL;
}

#ifdef SHARED
/*
 * create a memory based output file and attach for output
 * pass in a char ** to be filled with a malloced buffer?
 */
EXPORT(int)
io_output_string(int unit,		/* external (1-based) unit */
		 char *fname,		/* "filename" for error reports */
		 char *buf,
		 int len) {
    struct file *fp;

    fp = io_memfile(fname, buf, len, 'w');
    if (fp == NULL)
	return FALSE;
    io_setfile(unit, fp);
    return TRUE;
}
#endif /* SHARED defined */

/*
 * implement SIL operations;
 */

/* limited printf */

#define COPY(SRC,LEN) \
{ \
    size_t len = LEN; \
    if (LEN > space) \
	len = space; \
    memcpy(lp, SRC, len); \
    lp += len; \
    space -= len; \
    *lp = '\0'; \
}

#define COPYTEMP COPY(temp, strlen(temp))

/*
 * IOPRINT -- formatted stats/error output (SIL OUTPUT op)
 *	orignally "format" was in FORTRAN FORMAT format!!
 */
void
io_printf(int_t unit, ...) {
    va_list vp;
    char *format;
    register char c;
    char line[1024];			/* XXX */
    size_t space;
    char *lp;
    struct file *fp;

    va_start(vp,unit);
    fp = findfile((int)INTERN(unit));
    if (!fp || !fp->iop)
	return;

    /* keep output in line buffer, in case output unbuffered (ie; stderr) */
    lp = line;
    space = sizeof(line) - 1;
    format = va_arg(vp, char *);
    while ((c = *format++) != '\0' && space > 0) {
	struct descr *dp;
	struct spec *sp;
	char temp[32];			/* large enough for 2^64 */
	char *cp;

	/* scan forward until first %, and print all at once? */
	if (c != '%') {
	    *lp++ = c;
	    space--;
	    continue;
	}
	c = *format++;
	if (c == '\0')
	    break;
	switch (c) {
	case 'd':			/* plain decimal */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%ld", (long)D_A(dp)); /* XXX handle LP32LL64 int_t */
	    COPYTEMP;
	    break;
	case 'D':			/* padded decimal */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%15ld", (long)D_A(dp)); /* XXX handle LP32LL64 int_t */
	    COPYTEMP;
	    break;
	case 'F':			/* padded float */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%15.3f", D_RV(dp));
	    COPYTEMP;
	    break;
	case 'G':			/* padded g/float */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%15.3g", D_RV(dp));
	    COPYTEMP;
	    break;
	case 'g':			/* unpadded g/float */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%g", D_RV(dp));
	    COPYTEMP;
	    break;
	case 's':			/* c-string (from version.c) */
	    cp = va_arg(vp, char *);
	    COPY(cp, strlen(cp));
	    break;
	case 'S':			/* spec */
	    sp = va_arg(vp, struct spec *);
	    /* might contain NUL's... will stop short! */
	    COPY(S_SP(sp), (size_t)S_L(sp));
	    break;
	case 'v':			/* variable */
	    dp = va_arg(vp, struct descr *);
	    dp = (struct descr *) D_A(dp); /* get var pointer */
	    if (dp) {
		struct spec s[1];

		S_A(s) = 0;		/* try to keep gcc quiet */
		S_O(s) = 0;		/* try to keep gcc quiet */
		X_LOCSP(s, dp);		/* get specifier */

		/* might contain NUL's... will stop short! */
		COPY(S_SP(s), (size_t)S_L(s));
	    }
	    break;
	case 'A':			/* padded descriptor Addr */
	    dp = va_arg(vp, struct descr *);
	    if (D_V(dp) == I)		/* INTEGER */
		sprintf(temp, "%15ld", (long)D_A(dp)); /* XXX handle LP32LL64 int_t */
	    else if (D_V(dp) == R)	/* REAL */
		sprintf(temp, "%#15.3g", (double)D_RV(dp));
	    else			/* presumed to be pointer */
		sprintf(temp, "%#15lx", (unsigned long)D_A(dp)); /* XXX handle LP32LL64 int_t */
	    COPYTEMP;
	    break;
	case 'L':			/* padded descriptor fLags */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%#15o", (int)D_F(dp)); /* defined in octal!! */
	    COPYTEMP;
	    break;
	case 'V':			/* padded descriptor Value (type) */
	    dp = va_arg(vp, struct descr *);
	    sprintf(temp, "%15d", (int)D_V(dp));
	    COPYTEMP;
	    break;
	default:
	    *lp++ = c;
	    space--;
	    break;
	}
    } /* while */
    va_end(vp);
    *lp = '\0';

    ioo_write(fp->iop, line, strlen(line));	/* was fputs */
} /* io_printf */

static int
io_write(struct file *fp, char *cp, int_t len) {
    if (len == 0)
	return TRUE;

    return ioo_write(fp->iop, cp, len) == len;
}

static int				/* bool */
io_print_str(struct file *fp,
	     char *cp,
	     int_t len,
	     int needfill,
	     int eol) {
    int ret = TRUE;

    if (fp == NULL)
	return FALSE;

    if (cp && len) {
	if (needfill) {
	    char *ep;
	    int l2;
	    char *tp = cp;

	    /* trim trailing spaces & NUL's (without altering specifier) */
	    ep = tp + len - 1;
	    while (len > 0 && (*ep == ' ' || *ep == '\0')) {
		len--;
		ep--;
	    }

	    /* plug remaining NULs with spaces */
	    for (l2 = len; l2 > 0; l2--) {
		if (*tp == '\0')
		    *tp = ' ';
		tp++;
	    }
	} /* compiling */

	ret = io_write(fp, cp, len);
    } /* have string */

    /* XXX check ret first? */

    if (ret && eol && !(fp->flags & FL_KEEPEOL))
	ret = io_write(fp, "\n", 1);

#ifdef NO_UNBUF_RW
    if ((fp->flags & FL_UNBUF)) {
	/* simulate unbuffered I/O (noop now that setvbuf used) */
	ret = ioo_flush(f);
    }
#endif /* NO_UNBUF_RW defined */
    return ret;
} /* io_print_str */

void
io_print(struct descr *iokey, struct descr *iob, struct spec *sp) { /* STPRNT */
    /* IOB->
     * title descr
     * integer unit number
     * pointer to natural var for format
     */
    int xunit = D_A(D_A(iob) + DESCR);
    struct file *fp = findfile(INTERN(xunit));
    D_A(iokey) = io_print_str(fp, S_SP(sp), S_L(sp), (int)D_A(COMPCL), 1);
}

int
io_endfile(int_t unit) {		/* ENFILE */
    struct unit *up;

    unit = INTERN(unit);

    /* bad unit a fatal error in SPITBOL, but not in SNOBOL4+; */
    if (BADUNIT(unit))
	return TRUE;
    up = FINDUNIT(unit);
    if (up->curr == NULL && up->head == NULL)
	return TRUE;

    return io_closeall(unit);
}

#define COMPILING(UNIT) ((UNIT) == INTERN(UNITI) && D_A(COMPCL))

#ifdef COMPILER_READLINE
static VAR int readline_inited;
#ifdef HAVE_RL_SET_KEYMAP
static VAR Keymap initial_keymap, compile_keymap;
#endif

static void
init_readline(void) {
    rl_initialize();
#ifdef HAVE_RL_SET_KEYMAP
    initial_keymap = rl_get_keymap();
    compile_keymap = rl_copy_keymap(initial_keymap);
    rl_set_keymap(compile_keymap);
#endif
    /* disable TAB completion */
    rl_bind_key('\t', rl_insert);

    readline_inited = 1;
}

static void
restore_readline(void) {
    if (!readline_inited)
	return;
#ifdef HAVE_RL_SET_KEYMAP
    /* restore initial keymap */
    if (initial_keymap)
	rl_set_keymap(initial_keymap);
#if 0
    /* dies with free() of invalid pointer on Ubuntu. */
    if (compile_keymap)
	rl_discard_keymap(compile_keymap);
    compile_keymap = NULL;
#endif
#else
    /*
     * probably here with "editline" library (OS X, NetBSD?)
     */
    rl_initialize();			/* seems to work */
#endif
    clear_history();
}
#endif /* ifdef COMPILER_READLINE */

enum io_read_ret
io_read(struct descr *dp, struct spec *sp) {	/* STREAD */
    int unit;
    int_t recl;
    ssize_t len;
    char *cp;
    struct file *fp;
    struct unit *up;
    struct io_obj *iop;

    unit = INTERN(D_A(dp));
    if (BADUNIT(unit) || (up = FINDUNIT(unit)) == NULL || up->curr == NULL) {
	if (COMPILING(unit)) {
	    return IO_ERR;		/* compiler never quits!! */
	}
	return IO_EOF;
    }

    recl = S_L(sp);			/* YUK! */
    cp = S_SP(sp);
    for (;;) {
	fp = up->curr;
	iop = fp->iop;
	if (iop == NULL) {
	    iop = io_fopen( fp, 'r' );
	    if (iop == NULL)
		return IO_ERR;
	}

	if (iop->flags & FL_BINARY) {
	    if (recl == 0)
		return IO_ERR;

	    len = ioo_read_raw(iop, cp, recl);
	    if (len == recl)
		break;
	}
#ifdef COMPILER_READLINE
	else if (ISTTY(fp) && COMPILING(unit)) {
	    char *tp;

	    if (!readline_inited)
		init_readline();
	    tp = readline("snobol4> ");
	    if (!tp)
	       return IO_EOF;
	    if (*tp)
		add_history(tp);
	    len = strlen(tp);
	    if (len > recl-1)	/* leave room for space */
	        len = recl-1;
	    strncpy(cp, tp, len);
	    free(tp);
	    break;
	}
#endif /* COMPILER_READLINE defined */
	else {			/* normal, cooked (line) I/O */
	    len = ioo_getline(iop);
	    if (len > 0) {
		/* if normal EOL processing, discard newline */
		if (!(iop->flags & FL_KEEPEOL) && iop->linebuf[len-1] == '\n') {
		    len--;
		    if (len && iop->linebuf[len-1] == '\r')
			len--;
		}

		if (COMPILING(unit)) {
		    /* compiler expects data in-place, so copy it */
		    if (!recl)
			return IO_ERR;
		    if (len > recl)
			len = recl;
		    /* NOTE! truncates line (discards rest of record) */
		    memcpy(cp, iop->linebuf, len);
		    break;
		}
		else {			/* not compiling */
		    /*
		     * 2020-09-20: point at getline buffer!!!
		     * no more truncation (at the cost of another copy)
		     */
		    S_A(sp) = (int_t) iop->linebuf;
		    S_O(sp) = 0;	/* offset */
		    S_F(sp) = 0;	/* flags S_A not PTR!! */
		    /* S_L(sp) set below */
		    break;
		}
	    }
	} /* else (normal, cooked) */

	/* here when read failed; see if non-EOF error */
	if (!ioo_eof(iop) )
	    return IO_ERR;		/* error wasn't EOF */

	/* here with EOF */
	if (!io_next(unit)) {		/* skip to next file, if any */
	    return IO_EOF;		/* no more files */
	}
	if (COMPILING(unit)) {
	    /* force call to INCCK to pop old FILENM and LNNOCL */
	    return IO_EOF;
	}
	/* here with next file, if not compiling (command line -r?) */
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

/*
 * will never be implemented
 * I/O is not record oriented (no magtape support); use "SET" to seek 
 * (might as well just remove from the SIL code!)
 */
void
io_backspace(int_t unit) {		/* SIL BKSPCE op */
    (void) unit;
    UNDF(NORET);
}

void
io_rewind(int_t unit) {			/* SIL REWIND op */
    struct file *fp;
    struct unit *up;

    unit = INTERN(unit);
    if (BADUNIT(unit))
	return;

    up = FINDUNIT(unit);
    if (up->curr != up->head) {
	if (up->curr != NULL)		/* open file not first in list */
	    io_close((int)unit);	/* close it */
	up->curr = up->head;		/* reset to head of list */
	if (up->curr->iop == NULL)
	    io_fopen(up->curr, 'r');
    }
    fp = up->curr;
    if (fp == NULL)
	return;

    ioo_seeko(fp->iop, up->offset, SEEK_SET);
} /* io_rewind */

/* extensions; */

/* here at end of compilation */
void
io_ecomp(void) {			/* SIL XECOMP op */
    struct unit *up;
    struct file *fp;

#ifdef COMPILER_READLINE
    restore_readline();
#endif

    if (lflag) {
	/* if -l was given, switch OUTPUT to stdout! */

	/* XXX check return?! */
	io_mkfile2( UNITO, stdout, STDOUT_NAME, FL_NOCLOSE );
    }

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

    up = FINDUNIT(UNITI - 1);

    /* free source files... */
    fp = up->head;
    while (fp && fp != up->curr) {
	struct file *tp;

	tp = fp->next;
	free(fp);
	fp = tp;
    }

    up->head = up->curr;		/* save file for rewind */
    up->offset = ioo_tello(up->curr->iop); /* save offset for rewind */

    /* free list of included filenames */
    while (includes) {
	struct file *tp;

	tp = includes->next;
	free(includes);
	includes = tp;
    }

    /* loadx.c uses list of include directories, cannot free!! */
}

/* process I/O option strings for io_openi and io_openo */
static int
io_options(char *op,			/* IN: options */
	   int *rp,			/* OUT: recl (optional) */
	   int *fp) {			/* OUT: flags */
    int flags;
    int recl;

    flags = *fp;
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
	    while (isdigit((unsigned char)*op)) {
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
	    flags |= FL_BINARY|FL_KEEPEOL;
	    op++;
	    break;

	case 'C':			/* SITBOL/SPITBOL: character */
	case 'c':
	    flags |= FL_BINARY|FL_KEEPEOL;
#if 0
	    if (recl)			/* already have recl? */
		return FALSE;		/* fail */
#endif /* 0 */
	    recl = 1;
	    op++;
	    break;

	case 'E':			/* extension: close on Exec */
	case 'e':
	    flags |= FL_CLOEXEC;
	    op++;
	    break;

	case 'K':	    /* Local/experimental: breaK long lines */
	case 'k':
	    break;			/* dead in 2.2 */

	case 'T':			/* SITBOL: "terminal" (no EOL) */
	case 't':
	    flags |= FL_UNBUF|FL_KEEPEOL; /* force prompt output */
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

	case 'X':	   /* extension: eXclusive (fail if eXists) */
	case 'x':
	    flags |= FL_EXCL;
	    op++;
	    break;

	default:
	    op++;
	    break;
	}
    } /* while *op */

    *fp = flags;
    if (rp)
	*rp = recl;
    return TRUE;
}

/* here via XCALL IO_OPENI */
/* called from SNOBOL INPUT() */
int
io_openi(struct descr *dunit,		/* IN: unit */
	 struct spec *sfile,		/* IN: filename */
	 struct spec *sopts,		/* IN: options */
	 struct descr *drecl) {		/* OUT: rec len */
    char fname[MAXFNAME];		/* XXX */
    char opts[MAXOPTS];			/* XXX */
    struct file *fp;
    struct unit *up;
    int xunit, unit;
    int recl;

    xunit = D_A(dunit);			/* external unit number */
    unit = INTERN(xunit);		/* internal unit number */
    if (BADUNIT(unit))
	return FALSE;			/* fail */
    up = FINDUNIT(unit);

    /* XXX handle arbitrary length strings? */
    spec2str( sfile, fname, sizeof(fname) ); /* XXX mspec2str */
    spec2str( sopts, opts, sizeof(opts) );   /* XXX mspec2str */

    /* XXX if no sopts;
     * extract spitbol style options suffix (if any) from filename here?
     */

    if (fname[0]) {
	/*
	 * SITBOL takes comma seperated file list
	 * would need to keep flags per-unit
	 */
	fp = io_newfile(fname);
    }
    else {
	fp = up->curr;
    }
    if (fp == NULL)
	return FALSE;

    /* process options */
    if (!io_options(opts, &recl, &fp->flags)) {
	free(fp);
	return FALSE;
    }

    if (recl && !(fp->flags & FL_BINARY)) {
	static VAR char recl_ignored_warning = 0;
	/* just once per run: have an environment variable suppress this?? */
	if (!recl_ignored_warning) {
	    fprintf(stderr, "Ignoring record length %d on I/O unit %d\n",
		    recl, xunit);
	    recl_ignored_warning = 1;
	}
	recl = VLRECL;			/* Keep PUTIN from pre-allocating */
    }

    /* open it now, so we can return status! */
    if (fname[0]) {
	if (io_fopen( fp, 'r') == NULL) {
	    free(fp);
	    return FALSE;		/* fail; no harm done! */
	}
	io_closeall(unit);
	up->curr = up->head = fp;
    }

    /* pass recl back up */
    D_A(drecl) = recl;
    D_F(drecl) = 0;
    D_V(drecl) = I;

    return TRUE;
} /* io_openi */

/* here via XCALL IO_OPENO */
/* called from SNOBOL OUTPUT() */
int
io_openo(struct descr *dunit,		/* IN: unit */
	 struct spec *sfile,		/* IN: filename */
	 struct spec *sopts) {		/* IN: options */
    char fname[MAXFNAME];		/* XXX malloc(S_L(sfile)+1)? */
    char opts[MAXOPTS];			/* XXX malloc(S_L(sopts)+1)? */
    struct file *fp;
    struct unit *up;
    int unit;

    unit = INTERN(D_A(dunit));
    if (BADUNIT(unit))
	return FALSE;			/* fail */
    up = FINDUNIT(unit);

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
	fp = up->curr;
    }

    if (fp == NULL)
	return FALSE;			/* fail; no harm done! */

    /* process options */
    if (!io_options(opts, NULL, &fp->flags)) { /* XXX error if recl set?? */
	free(fp);
	return FALSE;
    }

    /* open it now, so we can return status! */
    if (fname[0]) {
	if (io_fopen( fp, 'w') == NULL) {
	    free(fp);
	    return FALSE;		/* fail; no harm done! */
	}
	io_closeall(unit);
	up->curr = up->head = fp;
    }
    return TRUE;
} /* io_openo */

static enum io_include_ret
io_include2(struct descr *dp,		/* input unit */
	    char *fname) {		/* file name (with quotes) */
    int l;
    struct file *fp;
    struct unit *up;
    char *fn2;

    /* search includes list to see if file already included!! */
    for (fp = includes; fp; fp = fp->next)
	if (strcmp(fname, fp->fname) == 0) /* found it!!! */
	    return INC_SKIP;		/* as you were! */

    /* strip off trailing spaces after uniqueness test */
    l = strlen(fname);
    while (l > 0 && fname[l-1] == ' ') {
	l--;
    }
    fname[l] = '\0';

    /* NOTE!!! No longer trying local directory: must be in path!!! */

    fn2 = io_lib_find(NULL, fname, NULL);
    if (!fn2)
	return INC_FAIL;		/* not found */

    fp = io_newfile(fn2);
    free(fn2);
    if (fp == NULL)
	return INC_FAIL;		/* alloc failure */

    if (io_fopen( fp, 'r') == NULL) {
	free(fp);
	return INC_FAIL;
    }

    up = FINDUNIT(INTERN(D_A(dp)));

    /* push new file onto top of input list */
    fp->next = up->curr;
    up->curr = fp;

    /* add base filename to list of files already included */
    fp = io_newfile(fname);		/* reuse struct file!! */
    if (fp) {
	fp->next = includes;
	includes = fp;		/* XXX keep per unit? nah. */
    }
    return INC_OK;
} /* io_include2 */

enum io_include_ret
io_include(struct descr *dp,		/* input unit */
	   struct spec *sp) {		/* file name (with quotes) */
    char *fname = mspec2str(sp);
    enum io_include_ret ret = io_include2(dp, fname);
    free(fname);
    return ret;
} /* io_include */

/*
 * retrieve file name currently associated with a unit, or NULL.
 * data only valid while current file open
 */
char *
io_fname(int unit) {			/* takes external (1-based) unit */
    struct unit *up;
    struct file *fp;

    unit = INTERN(unit);
    if (BADUNIT(unit))
	return NULL;

    up = FINDUNIT(unit);
    if (up == NULL)
	return NULL;

    fp = up->curr;
    if (fp == NULL)
	return NULL;

    return fp->fname;
}

/*
 * retrieve file currently associated with a unit
 * used by compiler to pick up filenames from command line
 */
int
io_file(struct descr *dp,		/* IN: unit number */
	struct spec *sp) {		/* OUT: filename */
    char *fname;

    fname = io_fname(D_A(dp));
    if (fname == NULL)
	return FALSE;

    S_A(sp) = (int_t) fname;		/* OY! */
    S_F(sp) = 0;			/* NOTE: *not* a PTR! */
    S_V(sp) = 0;
    S_O(sp) = 0;
    S_L(sp) = strlen(fname);
    CLR_S_UNUSED(sp);
    
    return TRUE;
}

/*
 * support for SPITBOL SET() function
 *
 * problems on modern 32-bit systems where sizeof(off_t) > sizeof(int_t)
 * called via IO_SEEK macro using XCALLC in SIL code
 */

int
io_seek(struct descr *dunit, struct descr *doff, struct descr *dwhence) {
    int unit, whence;
    io_off_t off;
    struct file *fp;
    struct unit *up;

    unit = INTERN(D_A(dunit));
    if (BADUNIT(unit))
	return FALSE;
    up = FINDUNIT(unit);
    fp = up->curr;
    if (fp == NULL)
	return FALSE;

    off = (io_off_t) D_A(doff);
    whence = D_A(dwhence);
    if (whence < 0 || whence > 2)
	return FALSE;

    /* translate n -> SEEK_xxx using switch stmt (if SEEK_xxx available)? */

    if (ioo_seeko(fp->iop, off, whence) == 0)
	D_A(doff) = (int_t)ioo_tello(fp->iop);	/* XXX truncation possible (on 32b)! */
    else
	return FALSE;

    return TRUE;
}

/*
 * new 3/12/99
 * Experimental "scaled SET" function
 * called as external function from snolib/sset.c
 * (not needed on 64-bit systems)
 */
int
io_sseek(int_t unit, int_t soff, int_t whence, int_t scale, int_t *oof ) {
    io_off_t off;
    struct file *fp;
    struct unit *up;
    struct io_obj *iop;

    unit = INTERN(unit);
    if (BADUNIT(unit))
	return FALSE;
    up = FINDUNIT(unit);
    fp = up->curr;
    if (fp == NULL)
	return FALSE;

    off = soff * (io_off_t)scale;
    if (whence < 0 || whence > 2)
	return FALSE;

    /* translate n -> SEEK_xxx using switch stmt (if SEEK_xxx available)? */

    iop = fp->iop;
    if (iop == NULL)
	return FALSE;

    if (ioo_seeko(iop, off, whence) == 0)
	*oof = ioo_tello(iop)/scale;
    else
	return FALSE;

    return TRUE;
}

/* flush all pending output before system(), exec(), or death */
int
io_flushall(int dummy) {		/* called w/ SIL XCALLC */
    int i;

    (void) dummy;
    for (i = 1; i <= NUNITS; i++) {
	struct file *fp;
	struct unit *up;

	up = FINDUNIT(INTERN(i));
	fp = up->curr;
	if (fp && fp->iop) {
	    /* keep err count?? */
	    ioo_flush(fp->iop);
	}
    } /* foreach unit */
    return TRUE;
}

/*
 * for PML functions; return a free unit number, returns -1 on failure
 * (use io_mkfile() to attach open file to unit)
 *
 * available in SNOBOL via snolib/findunit.c (PML'ed) as IO_FINDUNIT()
 */

#define MINFIND 20			/* minimum unit to return */
#define MAXFIND NUNITS			/* maximum unit to return */

EXPORT(int)
io_findunit(void) {
    int start;

    for (;;) {
	if (finger < MINFIND)
	    finger = MAXFIND;

	start = finger;
	while (finger >= MINFIND) {
	    int u;
	    struct unit *up;
	    int ret;

	    u = INTERN(finger);
	    up = FINDUNIT(u);
	    ret = finger--;
	    if (up->curr == NULL && up->head == NULL)
		return ret;		/* found a free unit */
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

/*
 * only stdio_obj subclass of io_obj has FILE pointer.
 * COULD implement a getfp io_obj method
 * and all files implementing io_obj interface already include <stdio.h>
 *	(for NULL and size_t)
 */
#if 0
/* for PML functions; get current fp on a unit */
EXPORT(FILE *)
io_getfp(int unit) {			/* "external" unit */
    struct unit *up;

    unit = INTERN(unit);
    if (BADUNIT(unit))
	return NULL;

    up = FINDUNIT(unit);
    if (up->curr == NULL)
	return NULL;

    return up->curr->f;
} /* io_getfp */
#endif

/*
 * new 9/9/97
 * Pad listing line out to input record length for "-LIST RIGHT"
 * Not strictly an "I/O" function, but here because the work used
 * to be done in io_read() for all compiler input, regardless of
 * listing on/off and left/right.
 */

int
io_pad(struct spec *sp, int len) {
    register char *cp;
    register int i;

    cp = S_SP(sp) + S_L(sp);
    for (i = len - S_L(sp); i > 0; i--)
	*cp++ = ' ';
    S_L(sp) = len;

    return 1;				/* for XCALLC */
}

/* new 9/21/97 called from lib/endex.c (which is called from main.c) */
int
io_finish(void) {
    int i;
    
    /* should visit from most recently opened to least recent? */
    for (i = 0; i < NUNITS; i++)
	io_closeall(i);

    while (lib_dirs) {
	struct file *tp;

	tp = lib_dirs->next;
	free(lib_dirs);
	lib_dirs = tp;
    }
    lib_dir_last = NULL;

    return TRUE;
}

/* new 1/12/2012 called to add a dir to include dir list (from init.c) */
int
io_add_lib_dir(char *dirname) {
    struct file *fp = io_newfile(dirname);
    if (!fp)
	return FALSE;
    if (lib_dir_last)
	lib_dir_last->next = fp;
    else
	lib_dirs = fp;		/* new list */
    lib_dir_last = fp;
    return TRUE;
}

/* new 1/12/2012 add a (PATH_SEP separated) path to include dir list
 * (called from init.c)
 */
int
io_add_lib_path(char *path) {
    char *p2 = strdup(path);
    char *pp = p2;
    if (!p2)
	return FALSE;

    while (pp) {
	/* XXX need strstr if sizeof(PATH_SEP) != 2 */
	char *tpp = strchr(pp, PATH_SEP[0]);
	if (tpp)
	    *tpp++ = '\0';		/* tie off and advance */
	io_add_lib_dir(pp);
	pp = tpp;
    }
    free(p2);
    return TRUE;
}

/* called from init.c to display paths (for -z option) */
void
io_show_paths(void)
{
    struct file *fp;
    for (fp = lib_dirs; fp; fp = fp->next)
	puts(fp->fname);
}

/*
 * search for a file, given a path, and optional subdir and extension
 * returns malloc'ed string or NULL
 */

/* helper */
static char *
trypath(char *dir,
	char *subdir,			/* optional: may be NULL */
	char *file,
	char *ext) {			/* optional: may be NULL */
    int l = strlen(dir) + strlen(file) + sizeof(DIR_SEP);
    char *path;

    if (subdir)
	l += strlen(subdir) + sizeof(DIR_SEP) - 1;
    if (ext)
	l += strlen(ext);
    path = malloc(l);
    if (!path)
	return NULL;
    strcpy(path, dir);
    strcat(path, DIR_SEP);
    if (subdir) {
	strcat(path, subdir);
	strcat(path, DIR_SEP);
    }
    strcat(path, file);
    if (ext)
	strcat(path, ext);
#if 0
    fprintf(stderr, "trypath: %s\n", path);
#endif
    if (exists(path))
	return path;
    free(path);
    return NULL;
}

/* used by io_include(), lib/loadx.c, -L option */
char *
io_lib_find(char *subdir, char *file, char *ext) {
    struct file *ip;

    if (abspath(file))
	return NULL;			/* absolute path */

    for (ip = lib_dirs; ip; ip = ip->next) {
	char *path;

	path = trypath(ip->fname, subdir, file, ext);
	if (path)
	    return path;

	if (ext) {
	    path = trypath(ip->fname, subdir, file, NULL);
	    if (path)
		return path;
	}

	/* if given subdir, try without it (for dynamic libraries) */
	if (subdir) {
	    path = trypath(ip->fname, NULL, file, ext);
	    if (path)
		return path;

	    if (ext) {
		path = trypath(ip->fname, NULL, file, NULL);
		if (path)
		    return path;
	    }
	}
    }
    return NULL;
}

/* 12/13/14 return n'th lib directory for HOST() */
char *
io_lib_dir(int n) {
    struct file *ip = lib_dirs;

    while (ip && n > 0) {
	n--;
	ip = ip->next;
    }
    if (ip)
	return ip->fname;
    return NULL;
}

/* helper for io_preload (adds to input file list) */
static void
try_preload(char *path) {
    if (!exists(path))
	return;

    if (isdir(path)) {
	int len = strlen(path) + sizeof(PRELOAD_FILENAME) + sizeof(DIR_SEP) - 1;
	char *fname = malloc(len);
	if (fname) {
	    strcpy(fname, path);
	    strcat(fname, DIR_SEP);
	    strcat(fname, PRELOAD_FILENAME);
	    if (exists(fname)) 
		io_input_file(fname);
	    free(fname);
	}
    }
    else
	io_input_file(path);
} /* try_preload */

/* called from init.c to preload source files */
void
io_preload(void) {
    char *env = getenv("SNOBOL_PRELOAD_PATH");
    if (env) {
	char *tmp, *tp;
	tmp = tp = strdup(env);
	strcpy(tmp, env);
	while (tp) {
	    char *np = strchr(tp, PATH_SEP[0]); /* strstr? */
	    if (np)
		*np++ = '\0';
	    try_preload(tp);
	    tp = np;
	}
	free(tmp);
    }
    else {				/* no SNOBOL_PRELOAD_PATH */
	struct file *ip;

	/* use include search path */
	for (ip = lib_dirs; ip; ip = ip->next)
	    try_preload(ip->fname);
    }
} /* io_preload */

#ifdef BLOCKS
/*
 * support for BLOCKS FASTPR macro
 * translated from the BAL in "cleanio"
 * 9/26/2013
 */
void
io_fastpr(struct descr *iokey, struct descr *unit, struct descr *ccfp,
	  struct spec *sp1, struct spec *sp2) {
    int_t len = S_L(sp1);
    char *src = S_SP(sp1);
    int xunit = D_A(unit);		/* external */
    int ccf = D_A(ccfp);		/* carriage control flag */
    struct file *fp = findfile(INTERN(xunit));
    int ret;

    /* NOTE!! CC doesn't get written in one write if unbuffered!! */
    if (ccf > 0) {			/* FORTRAN/ASA carriage control */
	ret = (io_print_str(fp, S_SP(sp2), 1, 0, 0) &&
	       io_print_str(fp, src, len, 1, 1));
    }
    else if (ccf < 0) {			/* EXT: ASCII carriage control */
	char ccc = *S_SP(sp2);		/* carriage control char */
	ret = TRUE;
	switch (ccc) {
	case '1':			/* form feed */
	    ret = io_write(fp, "\f", 1);
	    break;
	case '+':			/* overstrike */
	    ret = io_write(fp, "\r", 1);
	    break;
	case ' ':			/* fresh line */
	    ret = io_write(fp, "\n", 1);
	    break;
	}
	if (src && len && ret)
	    ret = io_print_str(fp, src, len, 1, 0); /* NO EOL! */
    }
    else				/* ccf == 0: no carriage control */
	ret = io_print_str(fp, src, len, 1, 1);
    D_A(iokey) = !ret;
} /* io_fastpr */
#endif /* BLOCKS */

/*
 * support for io_obj framework: the one place to allocate an io_obj
 */
struct io_obj *
io_alloc(int size, const struct io_ops *ops, int flags) {
    struct io_obj *iop = calloc(1, size); /* note zeroed! */
    if (iop) {
	iop->ops = ops;
	iop->flags = flags;
    }
    return iop;
}

/*
 * code excised from io_fopen2. called from stdio_obj.c (& winsock inetio)
 * NOTE!! path must be writable, and point AFTER the initial prefix
 * returned pointers will point inside path buffer.
 */
int
inet_parse(char *path, char **hostp, char **servicep, int *inet_flagp) {
    char *host, *service, *cp;
    int flags;

    flags = 0;

    host = path;
    service = strchr(host, '/');
    if (service == NULL)
	return -1;
    *service++ = '\0';

    /* look for option suffixes, ignore if unknown */
    cp = strchr(service, '/');
    if (cp) {
	char *op;

	*cp++ = '\0';
	do {
	    op = cp;
	    cp = strchr(cp, '/');
	    if (cp)
		*cp++ = '\0';

	    if (strcmp(op, "priv") == 0)
		flags |= INET_PRIV;
	    else if (strcmp(op, "broadcast") == 0)
		flags |= INET_BROADCAST;
	    else if (strcmp(op, "reuseaddr") == 0)
		flags |= INET_REUSEADDR;
	    else if (strcmp(op, "dontroute") == 0)
		flags |= INET_DONTROUTE;
	    else if (strcmp(op, "oobinline") == 0)
		flags |= INET_OOBINLINE;
	    else if (strcmp(op, "keepalive") == 0)
		flags |= INET_KEEPALIVE;
	    else if (strcmp(op, "nodelay") == 0)
		flags |= INET_NODELAY;

	    /* XXX more magic? non-booleans? linger?? */
	} while (cp);
    } /* have suffixes */

    if (flags & FL_CLOEXEC)
	flags |= INET_CLOEXEC;

    if (!(flags & FL_EXCL))
	flags |= INET_REUSEADDR;	/* NEW: reuse, unless exclusive */

    *hostp = host;
    *servicep = service;
    *inet_flagp = flags;

    return 0;
}
