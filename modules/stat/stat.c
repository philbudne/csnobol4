/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * TODO: chmod, fchmod, mkfifo, mkdir
 */

#ifndef st_atimensec	/* defined on NetBSD 9 (& FBSD 13?) */
#if defined(HAVE_ST_MTIM_NSEC)
#define st_atimensec st_atim.tv_nsec
#define st_mtimensec st_mtim.tv_nsec
#define st_ctimensec st_ctim.tv_nsec
#define st_birthtimensec st_birthtim.tv_nsec /* not on Linux */
#elif defined(HAVE_ST_MTIMESPEC_NSEC)	/* OSX is behind the times */
#define st_atimensec st_atimespec.tv_nsec
#define st_mtimensec st_mtimespec.tv_nsec
#define st_ctimensec st_ctimespec.tv_nsec
#define st_birthtimensec st_birthtimespec.tv_nsec
#endif
#endif /* st_atimensec not defined */

#define SETINT(DP,N,VAL) (DP)[N].a.i = (VAL); (DP)[N].f = 0; (DP)[N].v = I
#define COUNT(DP) ((DP)->v/DESCR+1)

enum st_member {
    ST_DESCR,
    ST_DEV,
    ST_INO,
    ST_TYPE,
    ST_MODE,
    ST_NLINK,
    ST_UID,
    ST_GID,
    ST_RDEV,
    ST_SIZE,
    ST_BLKSIZE,
    ST_BLOCKS,
    ST_ATIME,
    ST_ATIMENSEC,
    ST_MTIME,
    ST_MTIMENSEC,
    ST_CTIME,
    ST_CTIMENSEC,
    ST_BTIME,
    ST_BTIMENSEC,
    ST_COUNT				/* MUST BE LAST */
};


static void
st2sno(struct stat *st, struct descr *dp) {
    int type;
    switch (st->st_mode & S_IFMT) {
#ifdef S_IFIFO
    case S_IFIFO: type = 'p'; break;
#endif
    case S_IFCHR: type = 'c'; break;
    case S_IFDIR: type = 'd'; break;
    case S_IFREG: type = '-'; break;
#ifdef S_IFBLK
    case S_IFBLK: type = 'b'; break;
#endif
#ifdef S_IFLNK
    case S_IFLNK: type = 'l'; break;
#endif
#ifdef S_ISOCK
    case S_IFSOCK: type = 's'; break;
#endif
#ifdef S_ISWHT
    case S_IFWHT: type = 'w'; break;
#endif
    default: type = '?'; break;
    }
    SETINT(dp,ST_DEV,st->st_dev);
    SETINT(dp,ST_INO,st->st_ino);
    SETINT(dp,ST_TYPE,type);
    SETINT(dp,ST_MODE,st->st_mode);
    SETINT(dp,ST_NLINK,st->st_nlink);
    SETINT(dp,ST_UID,st->st_uid);
    SETINT(dp,ST_GID,st->st_gid);
    SETINT(dp,ST_RDEV,st->st_rdev);
    SETINT(dp,ST_SIZE,st->st_size);
#ifndef _WIN32				/* UGH!!!! */
    SETINT(dp,ST_BLKSIZE,st->st_blksize);
    SETINT(dp,ST_BLOCKS,st->st_blocks);
#endif
    SETINT(dp,ST_ATIME,st->st_atime);
    SETINT(dp,ST_MTIME,st->st_mtime);
    SETINT(dp,ST_CTIME,st->st_ctime);
#ifdef st_atimensec
    SETINT(dp,ST_ATIMENSEC,st->st_atimensec);
    SETINT(dp,ST_MTIMENSEC,st->st_mtimensec);
    SETINT(dp,ST_CTIMENSEC,st->st_ctimensec);
#endif
#ifdef st_birthtime /* defined on FreeBSD 12, NetBSD 9, OSX 10.15, Cygwin64 */
/* born on FreeBSD as a timespec, (renamed from st_createtimespec) in 2002 */
    SETINT(dp,ST_BTIME,st->st_birthtime);
    SETINT(dp,ST_BTIMENSEC,st->st_birthtimensec);
#endif /* st_birthtime */
}

/*
 * LOAD("STAT_(STRING,ST)STRING", STAT_DL)
 */
lret_t
STAT_( LA_ALIST ) {
    char *path;
    struct descr *dp = LA_PTR(1);
    struct stat st;
    int ret;

    if (!dp || LA_TYPE(1) < DATSTA || COUNT(dp) != ST_COUNT)
	RETFAIL;

    path = mgetstring(LA_PTR(0));
    ret = stat(path, &st);
    free(path);

    if (ret < 0)
	RETFAIL;

    /* validate dp[ST_DESCR] */
    st2sno(&st, dp);
    RETNULL;
}

/*
 * LOAD("LSTAT_(STRING,ST)STRING", STAT_DL)
 */
lret_t
LSTAT_( LA_ALIST ) {
#ifdef HAVE_LSTAT
    char *path;
    struct descr *dp = LA_PTR(1);
    struct stat st;
    int ret;

    if (!dp || LA_TYPE(1) < DATSTA || COUNT(dp) != ST_COUNT)
	RETFAIL;

    path = mgetstring(LA_PTR(0));
    ret = lstat(path, &st);
    free(path);

    if (ret < 0)
	RETFAIL;

    /* validate dp[ST_DESCR] */
    st2sno(&st, dp);
    RETNULL;
#else
    RETFAIL;
#endif
}

/*
 * LOAD("FSTAT_(INTEGER,ST)STRING", STAT_DL)
 */
lret_t
FSTAT_( LA_ALIST ) {
    struct descr *dp = LA_PTR(1);
    struct stat st;
    int ret;

    if (!dp || LA_TYPE(1) < DATSTA || COUNT(dp) != ST_COUNT)
	RETFAIL;

    if (fstat(LA_INT(0), &st) < 0)
	RETFAIL;

    /* validate dp[ST_DESCR] */
    st2sno(&st, dp);
    RETNULL;
}
