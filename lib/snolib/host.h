/* $Id$ */

/* this file used to generate SNOLIB/host.sno		@@@
 * coments are copied unless they contain "@@@"		@@@
 * end of line comments should NOT be used!		@@@
 * comments should mention the applicable environment(s) @@@*/

/*
 * HOST() function codes
 */

/* Macro SPITBOL: returns ARCHITECTURE:OPERATING_SYSTEM:SNOBOL_VERSION */
#define HOST_SYSINFO		""

/* Macro SPITBOL: returns argument from -u or entire command line */
#define HOST_PARAMS		0

/* Catspaw SPITBOL: execute argument as a shell command, return status */
#define HOST_SYSTEM		1

/* Catspaw SPITBOL: returns n'th token on command line (must be INTEGER) */
#define HOST_ARGN		2

/* Catspaw SPITBOL: returns index of first user command line argument token */
#define HOST_FIRSTARG		3

/* Catspaw SPITBOL: returns environment variable, or fails */
#define HOST_GETENV		4

/*****************
 * CSNOBOL4 Extensions
 */


/*****
 * configuration variables, from config.h (written by autoconf script)
 */
 
/* CSNOBOL4: returns "config.guess" output (arch-vendor-os) */   
#define HOST_CONFIG_GUESS	2000

/* CSNOBOL4: returns host where configure script was run */
#define HOST_CONFIG_HOST	2001

/* CSNOBOL4: returns date when configure script was run */
#define HOST_CONFIG_DATE	2002

/* CSNOBOL4: returns configure command line options (or fails) */
#define HOST_CONFIG_OPTIONS	2003

/*****
 * variables from build.c, created before linking snobol4 executable
 */

/* CSNOBOL4: returns date when snobol4 built */
#define HOST_BUILD_DATE		2100

/* CSNOBOL4: returns directory where snobol4 built */
#define HOST_BUILD_DIR		2101

/* CSNOBOL4: returns named of files ued to build snobol4 */
#define HOST_BUILD_FILES	2102

/*****
 * defines, from Makefile
 */

/* CSNOBOL4: return default SNOLIB directory (for -INCLUDE, LOAD()) */
#define HOST_SNOLIB_DIR		2200

/* CSNOBOL4: return default file for LOAD() */
#define HOST_SNOLIB_FILE	2201

/*****
 * integer constants;
 */

/* CSNOBOL4: number of bits used to represent SNOBOL4 INTEGER type */
#define HOST_INTEGER_BITS	2300

/* CSNOBOL4: number of bits used to represent SNOBOL4 REAL type */
#define HOST_REAL_BITS		2301

/* CSNOBOL4: number of bits used to represent C pointer type */
#define HOST_POINTER_BITS	2302

/* CSNOBOL4: number of bits used to represent C long type */
#define HOST_LONG_BITS		2303

/* CSNOBOL4: number of bits used to represent SIL descriptor type */
#define HOST_DESCR_BITS		2304

/* CSNOBOL4: number of bits used to represent SIL specified type */
#define HOST_SPEC_BITS		2305

/* CSNOBOL4: number of bits used to represent C char type */
#define HOST_CHAR_BITS		2306

/*****
 * integer variables;
 */

/* CSNOBOL4: size of "dynamic" storage in descriptors */
#define HOST_DYNAMIC_SIZE	2400

/* CSNOBOL4: size of pattern match storage in descriptors */
#define HOST_PMSTACK_SIZE	2401

/*
 * NOTE!! All of the above 2xxx values are related to internals, and
 * the build environment.  Perhaps it should be kept that way, and
 * other values added in a different range?
 */
