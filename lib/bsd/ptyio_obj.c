/*
 * $Id$
 * ptyio -- using BSD forkpty
 * Phil Budne
 * 2020-08-20
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/wait.h>			/* waitpid() */
#include <stdio.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>			/* _exit */
#endif /* HAVE_UNISTD_H defined */

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>			/* FreeBSD: forkpty */
#endif
#ifdef HAVE_UTIL_H
#include <util.h>			/* NetBSD: forkpty */
#endif
#ifdef HAVE_PTY_H
#include <pty.h>			/* linux: forkpty() */
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>			/* _PATH_BSHELL */
#endif

#include "h.h"				/* for lib.h */
#include "snotypes.h"			/* for lib.h */
#include "lib.h"			/* closefrom */
#include "io_obj.h"
#include "stdio_obj.h"			/* stdio_wrap, flags2mode */

struct ptyio_obj {
    struct stdio_obj sio;
    pid_t pid;
};

static int
ptyio_close(struct io_obj *iop) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;
    pid_t pid;
    int ret;

    ret = fclose(piop->sio.f) == 0;

    // issue kill() if needed????
    do {
	pid = waitpid(piop->pid, NULL, 0);
    } while (pid == -1 && errno == EINTR);

    /* only fail because of I/O errors! */
    return ret;
}

#define ptyio_read NULL
#define ptyio_write NULL
#define ptyio_seeko NULL
#define ptyio_tello NULL
#define ptyio_flush NULL
#define ptyio_eof NULL
#define ptyio_clearerr NULL
#define ptyio_read_raw NULL

MAKE_OPS(ptyio, &stdio_ops);

struct io_obj *
ptyio_open(char *path, int flags, int dir) {
    int master;
    pid_t pid;

    if (path[0] != '|' || path[1] != '|')
	return NOMATCH;

    pid = forkpty(&master, NULL, NULL, NULL);
    if (pid == 0) {
	closefrom(3);
	execl(_PATH_BSHELL, "sh", "-c", path+2, NULL);
	_exit(1);
    }
    else if (pid > 0) {
	char mode[MAXMODE];
	struct ptyio_obj *piop;
	FILE *f;

	/* XXX need stdio_wrapfd!! */
	flags2mode(flags, mode, dir);
	f = fdopen(master, mode);
	if (!f) {
	    close(master);
	    /* XXX kill child & wait for it? */
	    return NULL;
	}
	piop = (struct ptyio_obj *)
	    stdio_wrap(path, f, sizeof(*piop), &ptyio_ops, flags);
	if (!piop) {
	    fclose(f);
	    /* XXX kill child & wait for it? */
	    return NULL;
	}
	piop->pid = pid;
	return &piop->sio.io;
    }
    else
	return NULL;			/* forkpty failed */
}
