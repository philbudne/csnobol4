/* $Id$ */
 
/*
 * popen()/pclose() using system() and redirection
 * September 24, 1997
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#ifdef HAVE_STDLIB_H			/* before stdio */
#include <stdlib.h>			/* for malloc */
#else  /* HAVE_STDLIB_H not defined */
extern void *malloc();
#endif /* HAVE_STDLIB_H not defined */

#include <stdio.h>

struct pipe {
    struct pipe *next;
    FILE *file;
    char mode;
    char command[1024];			/* XXX */
    char *tempfile;
    int status;
};

static struct pipe *pipes;

FILE *
popen(file, mode)
    char *file, *mode;
{
    struct pipe *pp;

    if (!file || !mode)
	return NULL;

    if (*mode != 'r' && *mode != 'w')
	return NULL;

    pp = (struct pipe *) malloc(sizeof(struct pipe));
    pp->next = pipes;
    pp->mode = *mode;
    pp->tempfile = tempnam(NULL, "sno");
    if (!pp->tempfile) {
	free(pp);
	return NULL;
    }

    if (*mode == 'r') {
	sprintf(pp->command, "%s > %s", file, pp->tempfile);
	pp->status = system(pp->command);
    }
    else {
	sprintf(pp->command, "%s < %s", file, pp->tempfile);
    }

    pp->file = fopen(pp->tempfile, mode);
    if (pp->file == NULL) {
	free(pp->tempfile);
	free(pp);
	return NULL;
    }

    /* XXX setup an onexit() handler (first time)? */
    pipes = pp;				/* link into list */
}

int
pclose(f)
    FILE *f;
{
    struct pipe *pp, *ppp;
    int ret;

    for (pp = pipes, ppp = NULL; pp; ppp = pp, pp = pp->next)
	if (pp->file == f)
	    break;

    if (pp == NULL)
	return -1;			/* not found */

    /* unlink entry */
    if (ppp == NULL)
	pipes = pp->next;
    else
	ppp->next = pp->next;

    fclose(pp->file);			/* XXX check status? */

    if (pp->mode == 'r') {
	ret = pp->status;
    }
    else {
	ret = system(pp->command);
    }
    unlink(pp->tempfile);
    free(pp->tempfile);
    free(pp);

    return ret;
}

#ifdef TEST
main() {
    FILE *f;

    f = popen("ls -l", "r");
    if (f) {
	char line[1024];

	while (fgets(line, sizeof(line), f))
	    printf("%s", line);

	pclose(f);
    }

    f = popen("cat > foo", "w");
    if (f) {
	fprintf(f, "hello world\n");
	pclose(f);
    }
}
#endif /* TEST defined */
