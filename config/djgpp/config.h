/* $Id$ */

#define HAVE_STDLIB_H 
#define HAVE_UNISTD_H 

#define HAVE_OS_INIT
#define TRY_UNDERSCORE
#define SNOLIB_DIR "/snobol"
#define SNOLIB_FILE "snolib.dxe"

#define finite(X) 1
#define isnan(X) 0

/* for generic/sys.c */
#define HWNAME "x86"
#define OSNAME "DOS"
