/*
 * $Id$
 * from Chris Janton's (chj) VMS Icon port. -pb 9/97
 */

#undef __HIDE_FORBIDDEN_NAMES		/* for _NFILE */
#include <stdio.h>
#include <types.h>
#include <dvidef>
#include <iodef>
#include <stsdef.h>
#include <ssdef.h>
#include <clidef.h>

#define SUCCESS(_STAT) ((_STAT) & STS$M_SUCCESS)
#define SETERR(_STAT) do { vaxc$errno = (_STAT); errno = EVMSERR; } while(0)

struct descr {
    int length;
    char *ptr;
};

/* XXX keep linked list? */
static struct pipe {
    long pid;				/* process id of child */
    long status;			/* exit status of child */
    int channel;			/* MBX channel number */
    int efn;				/* Event flag to wait for */
    char mode;				/* the open mode */
    char running;			/* 1 if child is running */
} pipes[_NFILE];			/* one for every open file */

/*
 * popen - open a pipe command
 * Last modified 2-Apr-86/chj
 *
 *	popen("command", mode)
 */

FILE *
popen(cmd, mode)
    char *cmd;
    char *mode;
{
    FILE *pfile;			/* the Pfile */
    int fd;				/* underlying fd */
    struct pipe *pd;			/* pipe database pointer */
    struct descr mbxname;		/* name of mailbox */
    struct descr command;		/* command string struct descr */
    struct descr *input, *output;
    char mname[65];			/* mailbox name string */
    int chan;				/* mailbox channel number */
    int status;				/* system service status */
    int efn;
    long flags;				/* LIB$SPAWN flags */
    struct {
	short len;
	short code;
	char *address;
	char *retlen;
	int last;
    } itmlst;
    char m;

    if (!cmd || !mode)
	return NULL;

    LIB$GET_EF(&efn);
    if (efn == -1)
	return NULL;

    m = mode[0];
    if (m != 'r' && m != 'w')
	return NULL;

    /* create and open the mailbox */
    status = SYS$CREMBX(0, &chan, 0, 0, 0, 0, 0);
    if (!SUCCESS(status)) {
	SERERR(status);
	LIB$FREE_EF(&efn);
	return (0);
    }

    /* get mailbox name for fopen */
    itmlst.last = mbxname.length = 0;
    itmlst.address = mbxname.ptr = mname;
    itmlst.retlen = &mbxname.length;
    itmlst.code = DVI$_DEVNAM;
    itmlst.len = 64;
    status = SYS$GETDVIW(0, chan, 0, &itmlst, 0, 0, 0, 0);
    if (!SUCCESS(status)) {
	SETERR(status);
	LIB$FREE_EF(&efn);
	return (0);
    }
    mname[mbxname.length] = '\0';
    pfile = fopen(mname, mode);
    if (!pfile) {
	LIB$FREE_EF(&efn);
	SYS$DASSGN(chan);
	return (0);
    }

    /* Save file information now */
    fd = fileno(pfile);
    if (fd < 0 || fd >= _NFILE) {
	LIB$FREE_EF(&efn);
	SYS$DASSGN(chan);
	return (0);
    }
    pd = pipes + fd;
    pd->mode = m;
    pd->pid = pd->status = pd->running = 0;
    pd->channel = chan;
    pd->efn = efn;

    /* fork the command */
    command.length = strlen(cmd);
    command.ptr = cmd;
    if (m == 'r') {
	input = NULL;			/* leave as is */
	output = &mbxname;
    }
    else {
	input = &mbxname;
	output = NULL;			/* leave as is */
    }
    flags = CLI$M_NOWAIT | CLI$M_NOKEYPAD | CLI$M_NOCONTROL;

    status = LIB$SPAWN(&command, input, output, &flags, 0, &pd->pid,
		       &pd->status, &pd->efn, 0, 0, 0, 0);

    if (!SUCCESS(status)) {
	SETERR(status);
	fclose(pfile);
	LIB$FREE_EF(&efn);
	SYS$DASSGN(chan);
	return (0);
    }
    pd->running = 1;
    return (pfile);
} /* popen */

/*
 * pclose - close a pipe
 * Last modified 2-Apr-86/chj
 *
 */
int
pclose(pfile)
    FILE *pfile;
{
    struct pipe *pd;
    int fd;

    fd = fileno(pfile);
    if (fd == 0)			/* ?? */
	return -1;
    if (fd >= _NFILE)
	return -1;

    pd = pipes + fd;
    if (!pd->running)
	return -1;

    if (pd->mode == 'w') {
	fflush(pfile);			/* flush buffers */
	SYS$QIOW(0, pd->channel, IO$_WRITEOF, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    fclose(pfile);
    SYS$WFLOR(pd->efn, 1 << (pd->efn % 32)); /* wait for termination */
    SYS$DASSGN(pd->channel);
    LIB$FREE_EF(&pd->efn);
    pd->running = 0;

    if ((pd->status & STS$M_SUCCESS) != STS$M_SUCCESS)
	return pd->status;
    return 0;
}

#ifdef TEST
main() {
    for (;;) {
	int l;
	FILE *f;
	int stat;
	char line[1024];

	printf("command: ");
	if (!fgets(line, sizeof(line), stdin))
	    break;
	l = strlen(line);
	if (line[l-1] == '\n') {
	    if (l == 1)
		break;
	    line[l-1] = '\0';
	}
	f = popen(line, "r");
	if (f) {
	    while (fgets(line, sizeof(line), f))
		printf("> %s", line);
	    stat = pclose(f);
	    printf("status %d\n", stat);
	}
	else
	    printf("popen failed");
    }
}
#endif
