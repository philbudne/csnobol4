/*
 * $Id$
 * pipe.c
 * from Chris Janton's (chj) VMS Icon port.
 */

#include <stdio.h>
#include <types.h>
#include <dvidef>
#include <iodef>
#include <stsdef.h>
#include <clidef.h>

typedef struct _descr {
    int length;
    char *ptr;
} descriptor;

typedef struct _pipe {
    long pid;				/* process id of child */
    long status;			/* exit status of child */
    long flags;				/* LIB$SPAWN flags */
    int channel;			/* MBX channel number */
    int efn;				/* Event flag to wait for */
    char mode;				/* the open mode */
    FILE *fptr;				/* file pointer (for fun) */
    unsigned running : 1;		/* 1 if child is running */
} Pipe;

static Pipe _pipes[_NFILE];		/* one for every open file */

#define SFLAGS	(CLI$M_NOWAIT|CLI$M_NOKEYPAD|CLI$M_NOCONTROL)

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
    Pipe *pd;				/* _pipe database */
    descriptor mbxname;			/* name of mailbox */
    descriptor command;			/* command string descriptor */
    descriptor nl;			/* null device descriptor */
    char mname[65];			/* mailbox name string */
    int chan;				/* mailbox channel number */
    int status;				/* system service status */
    int efn;
    struct {
	short len;
	short code;
	char *address;
	char *retlen;
	int last;
    } itmlst;
    
    if (!cmd || !mode)
	return (0);
    LIB$GET_EF(&efn);
    if (efn == -1)
	return (0);
    if (_tolower(mode[0]) != 'r' && _tolower(mode[0]) != 'w')
	return (0);
    /* create and open the mailbox */
    status = SYS$CREMBX(0, &chan, 0, 0, 0, 0, 0);
    if (!(status & 1)) {
	LIB$FREE_EF(&efn);
	return (0);
    }
    itmlst.last = mbxname.length = 0;
    itmlst.address = mbxname.ptr = mname;
    itmlst.retlen = &mbxname.length;
    itmlst.code = DVI$_DEVNAM;
    itmlst.len = 64;
    status = SYS$GETDVIW(0, chan, 0, &itmlst, 0, 0, 0, 0);
    if (!(status & 1)) {
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
    pd = &_pipes[fileno(pfile)];	/* get Pipe pointer */
    pd->mode = _tolower(mode[0]);
    pd->fptr = pfile;
    pd->pid = pd->status = pd->running = 0;
    pd->flags = SFLAGS;
    pd->channel = chan;
    pd->efn = efn;
    /* fork the command */
    nl.length = strlen("_NL:");
    nl.ptr = "_NL:";
    command.length = strlen(cmd);
    command.ptr = cmd;
    status = LIB$SPAWN(&command,
		       (pd->mode == 'r') ? 0 : &mbxname, /* input file */
		       (pd->mode == 'r') ? &mbxname : 0, /* output file */
		       &pd->flags, 0, &pd->pid, &pd->status, &pd->efn,
		       0, 0, 0, 0);
    if (!(status & 1)) {
	LIB$FREE_EF(&efn);
	SYS$DASSGN(chan);
	return (0);
    } else {
	pd->running = 1;
    }
    return (pfile);
}

/*
 * pclose - close a pipe
 * Last modified 2-Apr-86/chj
 *
 */
pclose(pfile)
FILE *pfile;
{
    Pipe *pd;
    int status;
    int fstatus;
    
    pd = fileno(pfile) ? &_pipes[fileno(pfile)] : 0;
    if (pd == NULL)
	return (-1);
    /* XXX check pd->running? */
    fflush(pd->fptr);			/* flush buffers */
    fstatus = fclose(pfile);
    if (pd->mode == 'w') {
	status = SYS$QIOW(0, pd->channel, IO$_WRITEOF,
			  0, 0, 0, 0, 0, 0, 0, 0, 0);
	SYS$WFLOR(pd->efn, 1 << (pd->efn % 32));
    }
    SYS$DASSGN(pd->channel);
    LIB$FREE_EF(&pd->efn);
    pd->running = 0;
    return (fstatus);
}
