/* $Id$ */

/*
 * load and run external functions for systems using v7/BSD style a.out
 * -plb 11/9/93
 */

/*
 * How it works;
 *
 * uses ld to create an OMAGIC (impure) a.out file (which need
 * not load on a page boundary)
 *
 * runs ld twice; once to determine overall size, and a second time
 * after load address known.  This avoids needing to know about
 * relocation bits which tend to be CPU/port dependant.
 */
 
#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <a.out.h>
#include <strings.h>

extern char *malloc();
extern char *getenv();

/* is this right? -- ok for OMAGIC */
#undef N_SIZE				/* defined in NetBSD nlist.h */
#define N_SIZE(A) ((A).a_text + (A).a_data + (A).a_bss)

/* NetBSD compatibility */
#ifndef N_GETMAGIC
#define N_GETMAGIC(A) ((A).a_magic)
#endif /* N_GETMAGIC not defined */

#ifndef SYM_PREFIX
#define SYM_PREFIX "_"			/* XXX most (all?) a.out systems? */
#endif /* SYM_PREFIX not defined */

/* external function returning pointer to loaded function */
extern int (*pml_find())(LOAD_PROTO);

/* keep list of loaded functions (for UNLOAD) */
struct func {
    struct func *next;
    struct func *self;
    int (*entry)(LOAD_PROTO);
    char *data;
    char name[1];
};

static struct func *funcs;

static int
ld( output, addr, func, input )
    char *output;
    char *addr;
    char *func;
    char *input;
{
    char command[1024];			/* XXX */

    /*
     * -N		old, impure excutable (OMAGIC)
     * -o output	output file
     * -T addr		text addr (data follows)
     * -e name		entry point
     * input		relocatable object file (plus libs)!
     */

    /* XXX -A <path of mainbol executable??? */
    /* XXX -lm -lc ?? */

    sprintf( command, "%s -N -o %s -T %x -e %s%s %s",
	    LD_PATH, output, addr, SYM_PREFIX, func, input );

    /* XXX use direct execvp of ld? pass argv? */
    return system(command) == 0;
}

#define PATHLEN 256			/* XXX use MAXPATHLEN from param.h? */

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    struct func *fp; 
    int l1;

    l1 = S_L(sp1);
    fp = (struct func *) malloc( sizeof (struct func) + l1 );
    if (fp == NULL)
	return FALSE;			/* fail */

    strncpy( fp->name, S_SP(sp1), l1 );
    fp->name[l1] = '\0';
    fp->data = NULL;			/* assume internal! */

    /* try "poor mans load" first!!! */
    fp->entry = pml_find(fp->name);
    if (fp->entry == NULL) {		/* not found by pml */
	char path[PATHLEN];
	char temp[PATHLEN];
	struct exec a;
	char *snolib;
	long len;			/* size of code+data */
	int f;

	snolib = getenv("SNOLIB");
	if (snolib == NULL)
	    snolib = SNOLIB_DIR;

	if (sp2 && S_A(sp2) && S_L(sp2)) {
	    char temp2[sizeof(temp)];
	    char *tp;
	    struct stat st;

	    spec2str( sp2, temp, sizeof(temp) ); /* get libname [+ options] */
	    strcpy(temp2, temp);	/* save copy with options */
	    tp = index(temp, ' ');	/* look for space */
	    if (tp)
		*tp = '\0';		/* blot out space */

	    if (temp[0] != '/' && stat(temp, &st) < 0) {
		/* not absolute and no file; prepend libdir */
		/* XXX watch snolib length?? */
		sprintf( path, "%s/%s", snolib, temp2 );
	    }
	    else
		strcpy( path, temp2 );
	}
	else {				/* no path */
	    /* XXX watch snolib length */
	    sprintf( path, "%s/%s", snolib, SNOLIB_FILE );
	}

	sprintf( temp, "%s/snoXXXXXX", TMP_DIR);
	mktemp( temp );			/* exists in v6 */
	/* XXX check for error (empty string, or "/")?! */

	/* link once to get total size! */
	if (!ld( temp, 0, fp->name, path )) {
	    goto ld_error;
	}

	f = open(temp, O_RDONLY);
	if (f < 0) {
	    /* XXX error message? */
	    goto ld_error;

	}

	if (read( f, &a, sizeof(a)) != sizeof(a)) {
	    /* XXX error message? */
	    goto header_error;
	}

	if (N_GETMAGIC(a) != OMAGIC) {
	    /* XXX error message? */
	header_error:
	    close(f);
	ld_error:
	    unlink(temp);
	    free(fp);
	    return FALSE;		/* fail */
	}
	close(f);
	unlink(temp);

	len = N_SIZE(a);		/* total size (code+data+bss) */

	/* fix here for NMAGIC or ZMAGIC;  use valloc? */
	fp->data = malloc(len);
	if (fp->data == NULL) {
	    free(fp);
	    return FALSE;
	}

	/* XXX need only zero bss! */
	bzero( fp->data, len );

	/*
	 * could chain all of the following together in one big if stmt,
	 * but it would be a pain to debug!
	 */

	/* re-link at new addr */
	if (!ld( temp, fp->data, fp->name, path ) || (f = open(temp, 0)) < 0) {
	    goto file_open_error;
	}

	if (read( f, &a, sizeof(a)) != sizeof(a)) {
	data_read_error:
	    close(f);
	file_open_error:
	    unlink(temp);
	    free(fp->data);
	    free(fp);
	    return FALSE;
	}

	if (N_GETMAGIC(a) != OMAGIC || a.a_entry == 0 || N_SIZE(a) > len) {
	    goto data_read_error;
	}

	if (read(f, fp->data, len) != len) {
	    goto data_read_error;
	}

	fp->entry = (int (*)(LOAD_PROTO)) a.a_entry;
	close(f);
    } /* not found by pml */
    fp->self = fp;			/* make valid */

    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    D_A(addr) = (int_t) fp;
    return TRUE;			/* success */
}

/* support for SIL "LINK" opcode -- call external function */
int
link(retval, args, nargs, addr)
    struct descr *retval, *args, *nargs, *addr;
{
    struct func *fp;

    fp = (struct func *) D_A(addr);
    if (fp == NULL)
	return FALSE;

    if (fp->self != fp)			/* validate! */
	return FALSE;			/* fail */

    return (fp->entry)( retval, D_A(nargs), (struct descr *)D_A(args) );
}

void
unload(sp)
    struct spec *sp;
{
    struct func *fp, *pp;
    char name[1024];			/* XXX */

    spec2str( sp, name, sizeof(name) );

    for (pp = NULL, fp = funcs; fp != NULL; pp = fp, fp = fp->next) {
	if (strcmp(fp->name, name) == 0)
	    break;
    }

    if (fp == NULL)			/* not found */
	return;

    /* unlink from list */
    if (pp == NULL) {			/* first */
	funcs = fp->next;
    }
    else {				/* not first */
	pp->next = fp->next;
    }

    fp->self = 0;			/* invalidate self pointer!! */
    if (fp->data)			/* may be internal (PML) */
	free(fp->data);
    free(fp);				/* free name block */
}
