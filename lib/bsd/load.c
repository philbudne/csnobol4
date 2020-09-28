/* $Id$ */

/*
 * load and run external functions for systems using v7/BSD style a.out
 * -plb 11/9/93
 *
 * converted to loadx/os_load client 9/27/2020 -- not compiled
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
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <a.out.h>

#include <stdlib.h>			/* for malloc, getenv */
#include <stdio.h>

#include "h.h"
#include "snotypes.h"
#include "macros.h"
#include "path.h"
#include "load.h"
#include "lib.h"
#include "str.h"

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

/* keep list of loaded functions (for UNLOAD) */
struct func {
    struct func *next;
    struct func *self;
    loadable_func_t *entry;
    char *data;
    char name[1];
};

static struct func *funcs;

static int
ld(char *output, char *addr, char *func, char *input) {
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

/* "file" may include loader options (libs) after filename!! */
loadable_func_t *
os_load(char *func, char *file) {
    struct exec a;
    struct func *fp;
    char temp[PATHLEN];
    char *data;
    long len;				/* size of code+data */
    int f;

    sprintf( temp, "%s/snoXXXXXX", TMP_DIR);
    mktemp( temp );			/* exists in v6 */

    /* link once to get total size! */
    if (!ld( temp, 0, func, file)) {
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
	return NULL;			/* fail */
    }
    close(f);
    unlink(temp);

    len = N_SIZE(a);		      /* total size (code+data+bss) */

    /* fix here for NMAGIC or ZMAGIC;  use valloc? */
    data = malloc(len);
    if (data == NULL) {
	return NULL;
    }

    /* XXX need only zero bss! */
    bzero( data, len );

    /*
     * could chain all of the following together in one big if stmt,
     * but it would be a pain to debug!
     */

    /* re-link at new addr */
    if (!ld( temp, data, fp->name, file) || (f = open(temp, 0)) < 0)
	goto file_open_error;

    if (read( f, &a, sizeof(a)) != sizeof(a))
	goto data_read_error;

    if (N_GETMAGIC(a) != OMAGIC || a.a_entry == 0 || N_SIZE(a) > len)
	goto data_read_error;

    if (read(f, data, len) != len) {
    data_read_error:
	close(f);
    file_open_error:
	unlink(temp);
	free(data);
	return NULL;
    }
    close(f);

    fp = (struct func *) malloc( sizeof (struct func) + strlen(func) );
    if (fp == NULL) {
	free(data);
	return NULL;			/* fail */
    }
    strcpy(fp->name, func);
    fp->entry = (loadable_func_t *) a.a_entry;
    fp->data = data;
    fp->self = fp;			/* make valid */

    fp->next = funcs;			/* link into list (for unload) */
    funcs = fp;

    return fp->entry;
}

void
unload(struct spec *sp) {
    struct func *fp, *pp;
    char name[1024];			/* XXX */

    spec2str( sp, name, sizeof(name) );	/* XXX mspec2str */

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
    if (fp->data)
	free(fp->data);
    free(fp);				/* free name block */
}
