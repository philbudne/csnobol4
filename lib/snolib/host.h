/* $Id$ */

/*
 * HOST() function codes
 */

/*
 * SPITBOL
 */
#define HOST_SYSINFO		""
#define HOST_PARAMS		0
#define HOST_SYSTEM		1
#define HOST_ARGN		2
#define HOST_FIRSTARG		3
#define HOST_GETENV		4

/*
 * CSNOBOL4 Extensions
 */

/* configuration variables, from config.h (written by autoconf script) */
#define HOST_CONFIG_GUESS	2000
#define HOST_CONFIG_HOST	2001
#define HOST_CONFIG_DATE	2002
#define HOST_CONFIG_OPTIONS	2003

/* variables from build.c, created before linking snobol4 executable */
#define HOST_BUILD_DATE		2100
#define HOST_BUILD_DIR		2101
#define HOST_BUILD_FILES	2102

/* defines, from Makefile */
#define HOST_SNOLIB_DIR		2200
#define HOST_SNOLIB_FILE	2201

/* integer constants; */
#define HOST_INTEGER_BITS	2300
#define HOST_REAL_BITS		2301
#define HOST_POINTER_BITS	2302
#define HOST_LONG_BITS		2303
#define HOST_DESCR_BITS		2304
#define HOST_SPEC_BITS		2305

/*
 * NOTE!! All of the above 2xxx values are related to internals, and
 * the build environment.  Perhaps it should be kept that way, and
 * other values added in a different range?
 */
