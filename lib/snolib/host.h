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
#define HOST_SYSCMD		1

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
#define HOST_CONFIG_GUESS	2000	/* obsolete */

/* CSNOBOL4: returns host where configure script was run */
#define HOST_CONFIG_HOST	2001

/* CSNOBOL4: returns date when configure script was run */
#define HOST_CONFIG_DATE	2002

/* CSNOBOL4: returns configure command line options (or fails) */
#define HOST_CONFIG_OPTIONS	2003

/* CSNOBOL4: version date (from configure script) */
#define HOST_VERSION_DATE	2004

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

/* CSNOBOL4: C Compiler used to build CSNOBOL4 */
#define HOST_CC			2202

/* CSNOBOL4: C Compiler optimizer flags used to build CSNOBOL4 */
#define HOST_COPT		2203

/* CSNOBOL4: Shared Object Library extension */
#define HOST_SO_EXT		2204

/* CSNOBOL4: C Compiler flags for Shared Objects */
#define HOST_SO_CFLAGS		2205

/* CSNOBOL4: Shared Object file load command */
#define HOST_SO_LD		2206

/* CSNOBOL4: Shared Object file load switches */
#define HOST_SO_LDFLAGS		2207

/* CSNOBOL4: Dynamic Loadable Library extension */
#define HOST_DL_EXT		2208

/* CSNOBOL4: C Compiler flags for Dynamic Loadables */
#define HOST_DL_CFLAGS		2209

/* CSNOBOL4: Dynamic Loadable file load command */
#define HOST_DL_LD		2210

/* CSNOBOL4: Dynamic Loadable file load switches */
#define HOST_DL_LDFLAGS		2211

/* CSNOBOL4: return system directory seperator character (may be empty) */
#define HOST_DIR_SEP		2212

/* CSNOBOL4: return system PATH seperator character */
#define HOST_PATH_SEP		2213

/* CSNOBOL4: default library search path */
#define HOST_DEF_SNOPATH	2214

/* CSNOBOL4: path for C headers */
#define HOST_INCLUDE_DIR	2215

/* CSNOBOL4: object file extension */
#define HOST_OBJ_EXT		2216

/* CSNOBOL4: setuputil system name */
#define HOST_SETUP_SYS		2217

/* CSNOBOL4: shared object subdir */
#define HOST_SHARED_OBJ_SUBDIR	2218

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

/* CSNOBOL4: number of bits used to represent SIL "descriptor" type */
#define HOST_DESCR_BITS		2304

/* CSNOBOL4: number of bits used to represent SIL "specifier" type */
#define HOST_SPEC_BITS		2305

/* CSNOBOL4: number of bits used to represent C char type */
#define HOST_CHAR_BITS		2306

/*****
 * integer variables;
 */

/* CSNOBOL4: size of "dynamic" storage in descriptors */
#define HOST_DYNAMIC_SIZE	2400

/* CSNOBOL4: size of pattern match stack in descriptors */
#define HOST_PMSTACK_SIZE	2401

/* CSNOBOL4: size of interpreter stack in descriptors */
#define HOST_ISTACK_SIZE	2402

/*****
 * string variables;
 */
/* CSNOBOL4: library base directory in use */
#define HOST_SNOLIB_BASE	2500

/* CSNOBOL4: local, version-independant files */
#define HOST_SNOLIB_LOCAL	2501

/* CSNOBOL4: distribution files (version-specific) */
#define HOST_SNOLIB_VLIB	2502

/* CSNOBOL4: local, version-specific files */
#define HOST_SNOLIB_VLOCAL	2503

/* CSNOBOL4: return n'th element in search directory list */
#define HOST_SNOPATH_DIR	2504

/*
 * NOTE!! All of the above 2xxx values are related to internals, and
 * the build environment.  Perhaps it should be kept that way, and
 * other values added in a different range?
 */

/*
 * if you MUST add something to host.c (instead of creating a new
 * dynamicly loaded extension), use codes starting here, to avoid
 * conflicts with future CSNOBOL4 extensions:
 */
#define HOST_USER_EXTENSIONS	10000
