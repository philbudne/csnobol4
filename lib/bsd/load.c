/* $Id$ */

/*
 * load and run external functions for systems using a.out (v7/BSD) 11/9/93
 *
 * Unlikely to port easily to COFF systems where giving "ld" a load
 * address often involves creating a "command file"!
 */

/*
 * uses /bin/ld to create an OMAGIC (impure) a.out file (which need
 * not load on a page boundary) runs /bin/ld twice; once to determine
 * overall size, and a second time after load address known.  avoids
 * needing to know about cpu-dependant relocation bits which tend to
 * be CPU dependant.
 */
 
#include "h.h"
#include "types.h"
#include "macros.h"
#include "path.h"

#include <sys/types.h>
#include <a.out.h>

extern char *malloc();

#define N_SIZE(A) ((A).a_text + (A).a_data + (A).a_bss) /* ?? */

#define PREFIX "_"			/* XXX most (all?) a.out systems? */

/* keep list of loaded functions (for UNLOAD) */
struct func {
    struct func *next;
    struct func *self;
    func_t entry;
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
     * -N		old, impure excutable
     * -o output	output file
     * -T addr		text addr (data follows)
     * -e name		entry point
     * input		relocatable object file
     */

    /* XXX -A <path of mainbol executable??? */
    /* XXX -lm -lc ?? */
    sprintf( command, "%s -N -o %s -T %x -e %s%s %s",
	    LD_PATH, output, addr, PREFIX, func, input );

    return system(command) == 0;
}

int
load(addr, sp1, sp2)
    struct descr *addr;			/* OUT */
    struct spec *sp1, *sp2;		/* function, library */
{
    struct func *fp; 
    char path[128];			/* XXX max path */
    struct exec a;
    char *temp;
    int f;
    long len;				/* size of code+data */
    
    fp = (struct func *) malloc( sizeof (struct func) + S_L(sp1) );
    if (fp == NULL)
	return FALSE;			/* fail */

    strncpy( fp->name, S_SP(sp1), S_L(sp1) );
    fp->name[S_L(sp1)] = '\0';

    if (sp2 && S_A(sp2) && S_L(sp2)) {	/* XXX check if .le. sizeof(path)? */
	/* NOTE! no check if file exists!
	 * Could contain ld flags (ie; -l<libname>
	 */
	strncpy(path, S_SP(sp2), S_L(sp2) );
	path[S_L(sp2)] = '\0';
    }
    else {
	/* XXX SNOLIB_DIR "/" SNOLIB_A */
	strcpy(path, SNOLIB_A_PATH );
    }

    /* XXX hide this? (use make_temp(); copy to static storage */
    temp = (char *) tempnam("/usr/tmp", "sno");

    /* link once to get total size! */
    if (!ld( temp, 0, fp->name, path ) || (f = open(temp, 0)) < 0) {
	unlink(temp);			/* paranoia */
	free(temp);			/* XXX */
	free(fp);
	return FALSE;			/* fail */
    }

    if (read( f, &a, sizeof(a)) != sizeof(a) || a.a_magic != OMAGIC) {
	close(f);
	unlink(temp);
	free(temp);			/* XXX */
	free(fp);
	return FALSE;			/* fail */
    }
    close(f);
    unlink(temp);

    len = N_SIZE(a);			/* total size (code+data+bss) */
    fp->data = malloc(len);
    if (fp->data == NULL) {
	free(temp);
	free(fp);
	return FALSE;
    }

    /* XXX need only zero bss! */
    bzero( fp->data, len );

    /* re-link at new addr */
    if (!ld( temp, fp->data, fp->name, path ) || (f = open(temp, 0)) < 0) {
	unlink(temp);			/* paranoia */
	free(temp);
	free(fp->data);
	free(fp);
	return FALSE;
    }

    unlink(temp);			/* file now floating!! */
    free(temp);				/* XXX */

    if (read( f, &a, sizeof(a)) != sizeof(a)) {
    data_read_error:
	close(f);
	free(fp->data);
	free(fp);
	return FALSE;
    }

    if (a.a_magic != OMAGIC || a.a_entry == 0 || N_SIZE(a) > len) {
	/* XXX could tag onto above if, but would be harder to debug */
	goto data_read_error;
    }

    if (read(f, fp->data, len) != len) {
	/* XXX could tag onto above if, but would be harder to debug */
	goto data_read_error;
    }

    fp->entry = (func_t) a.a_entry;	/* XXX check if non-zero? */
    fp->self = fp;

    close(f);

    /* link into list (for unload) */
    fp->next = fp;
    funcs = fp;

    D_A(addr) = (int) fp;
    return TRUE;			/* success */
}

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

    return (fp->entry)( retval,
		       (struct decsr *)D_A(args),
		       D_A(nargs));
}

void
unload(sp)
    struct spec *sp;
{
    struct func *fp, *pp;
    char name[128];			/* XXX */

    strncpy( name, S_SP(sp), S_L(sp) );	/* XXX watch length? */
    name[S_L(sp)] = '\0';

    /* XXX search list, unlink, free fp->addr & struct func */
    for (pp = NULL, fp = funcs; fp != NULL; pp = fp, fp = fp->next) {
	if (strcmp(fp->name, name) == 0)
	    break;
    }

    if (fp == NULL)
	return;

    if (pp == NULL) {
	funcs = fp->next;
    }
    else {
	pp->next = fp->next;
    }

    free(fp->data);
    free(fp);
}
