/* $Id$ */

#define HAVE_STRINGS_H 
#define HAVE_STDLIB_H 
#define HAVE_UNISTD_H 

/* DXE2 support */
#ifdef HAVE_DXE2
#define HAVE_OS_INIT
#define TRY_UNDERSCORE
#define SNOLIB_DIR "/snobol4"
#define SNOLIB_FILE "snolib.dxe"
#endif /* HAVE_DXE2 defined */

/* WATTCP support */
#ifdef HAVE_WATTCP
#define FOLD_HOSTNAMES
#define NEED_BINDRESVPORT
#define SOCKLEN_T int
#define udp_open myudp_open
#define tcp_open mytcp_open
#endif /* HAVE_WATTCP defined */

#define PATH_SEP	";"
#define DIR_SEP		"\\"
