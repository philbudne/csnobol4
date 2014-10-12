/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "h.h"
#include "equ.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"

/*
 * TODO: chmod, fchmod, mkfifo, mkdir
 */

#ifdef S_IFWHT				/* OSX/BSD */
#define st_atimensec st_atimespec.tv_nsec
#define st_mtimensec st_mtimespec.tv_nsec
#define st_ctimensec st_ctimespec.tv_nsec
#endif
#ifdef linux
#define st_atimensec st_atim.tv_nsec
#define st_mtimensec st_mtim.tv_nsec
#define st_ctimensec st_ctim.tv_nsec
#endif

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
    ST_COUNT				/* MUST BE LAST */
};

static void
st2sno(st, dp)
    struct stat *st;
    struct descr *dp;
{
    int type;
    switch (st->st_mode & S_IFMT) {
    case S_IFIFO: type = 'p'; break;
    case S_IFCHR: type = 'c'; break;
    case S_IFDIR: type = 'd'; break;
    case S_IFREG: type = '-'; break;
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
    SETINT(dp,ST_BLKSIZE,st->st_blksize);
    SETINT(dp,ST_BLOCKS,st->st_blocks);
    SETINT(dp,ST_ATIME,st->st_atime);
    SETINT(dp,ST_ATIMENSEC,st->st_atimensec);
    SETINT(dp,ST_MTIME,st->st_mtime);
    SETINT(dp,ST_MTIMENSEC,st->st_mtimensec);
    SETINT(dp,ST_CTIME,st->st_ctime);
    SETINT(dp,ST_CTIMENSEC,st->st_ctimensec);
}

/*
 * LOAD("STAT_(STRING,ST)STRING", STAT_DL)
 */
int
STAT_( LA_ALIST ) LA_DCL
{
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
int
LSTAT_( LA_ALIST ) LA_DCL
{
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
    RETFAIL;				/* just call STAT_? */
#endif
}

/*
 * LOAD("FSTAT_(INTEGER,ST)STRING", STAT_DL)
 */
int
FSTAT_( LA_ALIST ) LA_DCL
{
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
