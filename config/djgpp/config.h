/* $Id$ */

#define HAVE_STDLIB_H 
#define HAVE_UNISTD_H 

/* DXE2 support */
#ifdef HAVE_DXE2
#define HAVE_OSINIT			/* may not be needed! */
#define TRY_UNDERSCORE
#define SNOLIB_DIR "/snobol"
#define SNOLIB_FILE "snolib.dxe"
#endif /* HAVE_DXE2 defined */

/* WATTCP support */
#ifdef HAVE_WATTCP
#define FOLD_HOSTNAMES
#define udp_open myudp_open
#define tcp_open mytcp_open
#endif /* HAVE_WATTCP defined */
