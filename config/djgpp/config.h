/* $Id$ */

#define HAVE_UNISTD_H 			/* POSIX!! */
#define HAVE_STRFTIME
#define NEED_GETLINE

#define SNOLIB_BASE "/usr/lib/snobol4"
#define SNOLIB_DIR SNOLIB_BASE "/" VERSION "/lib"

/* DXE2 support */
#ifdef HAVE_DXE2
#define HAVE_OS_INIT
#define TRY_UNDERSCORE
#define SNOLIB_FILE "snolib.dxe"
#define SO_EXT ".dxe"
#define DL_EXT ".dxe"
/* XXX need more defines for SO/DL builds? */
#endif /* HAVE_DXE2 defined */

/* WATTCP support */
#ifdef HAVE_WATTCP
#define FOLD_HOSTNAMES
#define SOCKLEN_T int
#endif /* HAVE_WATTCP defined */

#define PATH_SEP	";"
#define DIR_SEP		"\\"
#define OBJECT_EXT	".o"
#define SETUP_SYS	"djgpp"
#define CC_IS		"gcc"
