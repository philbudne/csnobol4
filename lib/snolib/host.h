/* $Id$ */

/* this file used to generate SNOLIB/host.sno		@@@
 * coments are copied unless they contain "@@@"		@@@
 * end of line comments should NOT be used!		@@@
 * comments should mention the applicable environment(s) @@@*/

/*
 * HOST() function codes
 */

/*
**=pea
**=sect NAME
**snobol4host \- SNOBOL4 host O/S functions
**
**=sect SYNOPSIS
**=code
**-INCLUDE 'host.sno'
**=ecode
**=sect DESCRIPTION
**=cut
*/

/*
**=pea
**=item B<HOST()>
**Macro SPITBOL: returns ARCHITECTURE:OPERATING_SYSTEM:SNOBOL_VERSION
**The string contains three parts, separated by colons.  The first
**part describes the physical architecture, the second describes the
**operating system, and the third describes the language
**implementation name and version.  NOTE! Architecture names come from
**the B<uname>(3) library call, and may be different for the same
**hardware when running different operating systems. Examples:
**=break
**\~\~\~\~B<amd64:FreeBSD 12.1-RELEASE-p1:CSNOBOL4B 2.1>
**=break
**\~\~\~\~B<x86_64:Linux 5.4.0-12-generic:CSNOBOL4B 2.1>
**=cut
*/
#define HOST_SYSINFO		""

/*
**=pea
**=item B<HOST(0)>
**Macro SPITBOL: returns a string containing the command line parameter
**supplied to the B<-u> option, if any.  If no B<-u> option was given,
**B<HOST(0)> returns the concatenation of all user parameters following
**the input filename(s).
**=cut
*/
#define HOST_PARAMS		0

/*
**=pea
**=item B<HOST(1>, I<string>B<)>
**Catspaw SPITBOL: passes the string to the B<system>(3) C
**library function, and returns the subprocess exit status.
**=cut
*/
#define HOST_SYSCMD		1

/*
**=pea
**=item B<HOST(2,> I<n>B<)>
**Catspaw SPITBOL: for integer I<n> returns the I<n>'th command line
**argument (regardless of whether the argument was the command name, an
**option, a filename or a user parameter) as a string, or failure if
**I<n> is out of range.
**=cut
*/
#define HOST_ARGN		2

/*
**=pea
**=item B<HOST(3)>
**Catspaw SPITBOL: returns an integer for use with B<HOST(2)> indicating the
**first command line argument available as a user parameter.
**=cut
*/
#define HOST_FIRSTARG		3

/*
**=pea
**=item B<HOST(4, I<string>B<)>
**Catspaw SPITBOL: returns the value of the environment variable
**named by I<string>.
**=cut
*/
#define HOST_GETENV		4

/*****************
 * CSNOBOL4 Extensions
 */


/*****
 * configuration variables, from config.h (written by autoconf script)
 */

/* CSNOBOL4: returns "config.guess" output (arch-vendor-os) */
#define HOST_CONFIG_GUESS	2000	/* obsolete */

/*
**=pea
**=item B<HOST(HOST_CONFIG_HOST)>
**CSNOBOL4: returns host where configure script was run
**=cut
*/
#define HOST_CONFIG_HOST	2001

/*
**=item B<HOST(HOST_CONFIG_DATE)>
**CSNOBOL4: returns date when configure script was run
**=cut
*/
#define HOST_CONFIG_DATE	2002

/*
**=pea
**=item B<HOST(HOST_CONFIG_OPTIONS)>
**CSNOBOL4: returns configure command line options (or fails)
**=cut
*/
#define HOST_CONFIG_OPTIONS	2003

/*
**=pea
**=item B<HOST(HOST_VERSION_DATE)>
**CSNOBOL4: version date (from configure script)
**=cut
*/
#define HOST_VERSION_DATE	2004

/*
**=pea
**=item B<HOST(HOST_CC_IS)>
**CSNOBOL4: C compiler name/id
**=cut
*/
#define HOST_CC_IS		2005

/*****
 * variables from build.c, created before linking snobol4 executable
 */

/*
**=pea
**=item B<HOST(HOST_BUILD_DATE)>
**CSNOBOL4: returns date when snobol4 built
**=cut
*/
#define HOST_BUILD_DATE		2100

/*
**=pea
**=item B<HOST(HOST_BUILD_DIR)>
**CSNOBOL4: returns directory where snobol4 built
**=cut
*/
#define HOST_BUILD_DIR		2101

/*
**=pea
**=item B<HOST(HOST_BUILD_FILES)>
**CSNOBOL4: returns named of files ued to build snobol4
**=cut
*/
#define HOST_BUILD_FILES	2102

/*****
 * defines, from Makefile
 */

/*
**=pea
**=item B<HOST(HOST_SNOLIB_DIR)>
**CSNOBOL4: return default SNOLIB directory (for -INCLUDE, LOAD())
**=cut
*/
#define HOST_SNOLIB_DIR		2200

/*
**=pea
**=item B<HOST(HOST_SNOLIB_FILE)>
**CSNOBOL4: return default file for LOAD()
**=cut
*/
#define HOST_SNOLIB_FILE	2201

/*
**=pea
**=item B<HOST(HOST_CC)>
**CSNOBOL4: C Compiler used to build CSNOBOL4
**=cut
*/
#define HOST_CC			2202

/*
**=pea
**=item B<HOST(HOST_COPT)>
**CSNOBOL4: C Compiler optimizer flags used to build CSNOBOL4
**=cut
*/
#define HOST_COPT		2203

/*
**=pea
**=item B<HOST(HOST_SO_EXT)>
**CSNOBOL4: Shared Object Library extension
**=cut
*/
#define HOST_SO_EXT		2204

/*
**=pea
**=item B<HOST(HOST_SO_CFLAGS)>
**CSNOBOL4: C Compiler flags for Shared Objects
**=cut
*/
#define HOST_SO_CFLAGS		2205

/*
**=pea
**=item B<HOST(HOST_SO_LD)>
**CSNOBOL4: Shared Object file load command
**=cut
*/
#define HOST_SO_LD		2206

/*
**=pea
**=item B<HOST(HOST_SO_LDFLAGS)>
**CSNOBOL4: Shared Object file load switches
**=cut
*/
#define HOST_SO_LDFLAGS		2207

/*
**=pea
**=item B<HOST(HOST_DL_EXT)>
**CSNOBOL4: Dynamic Loadable Library extension
**=cut
*/
#define HOST_DL_EXT		2208

/*
**=pea
**=item B<HOST(HOST_DL_CFLAGS)>
**CSNOBOL4: C Compiler flags for Dynamic Loadables
**=cut
*/
#define HOST_DL_CFLAGS		2209

/*
**=pea
**=item B<HOST(HOST_DL_LD)>
**CSNOBOL4: Dynamic Loadable file load command
**=cut
*/
#define HOST_DL_LD		2210

/*
**=pea
**=item B<HOST(HOST_DL_LDFLAGS)>
**CSNOBOL4: Dynamic Loadable file load switches
**=cut
*/
#define HOST_DL_LDFLAGS		2211

/*
**=pea
**=item B<HOST(HOST_DIR_SEP)>
**CSNOBOL4: return system directory seperator character (may be empty)
**=cut
*/
#define HOST_DIR_SEP		2212

/*
**=pea
**=item B<HOST(HOST_PATH_SEP)>
**CSNOBOL4: return system PATH seperator character
**=cut
*/
#define HOST_PATH_SEP		2213

/*
**=pea
**=item B<HOST(HOST_DEF_SNOPATH)>
**CSNOBOL4: default library search path
**=cut
*/
#define HOST_DEF_SNOPATH	2214

/*
**=pea
**=item B<HOST(HOST_INCLUDE_DIR)>
**CSNOBOL4: path for C headers
**=cut
*/
#define HOST_INCLUDE_DIR	2215

/*
**=pea
**=item B<HOST(HOST_OBJ_EXT)>
**CSNOBOL4: object file extension
**=cut
*/
#define HOST_OBJ_EXT		2216

/*
**=pea
**=item B<HOST(HOST_SETUP_SYS)>
**CSNOBOL4: setuputil system name
**=cut
*/
#define HOST_SETUP_SYS		2217

/*
**=pea
**=item B<HOST(HOST_SHARED_OBJ_SUBDIR)>
**CSNOBOL4: shared object subdir
**=cut
*/
#define HOST_SHARED_OBJ_SUBDIR	2218

/*
**=pea
**=item B<HOST(HOST_CONFIG_CFLAGS)>
**CSNOBOL4: CFLAGS from configure
**=cut
*/
#define HOST_CONFIG_CFLAGS	2219

/*
**=pea
**=item B<HOST(HOST_CONFIG_CPPFLAGS)>
**CSNOBOL4: C preprocessor flags from configure
**=cut
*/
#define HOST_CONFIG_CPPFLAGS	2220

/*
**=pea
**=item B<HOST(HOST_CONFIG_LDFLAGS)>
**CSNOBOL4: loader flags from configure
**=cut
*/
#define HOST_CONFIG_LDFLAGS	2221

/*****
 * integer constants;
 */

/*
**=pea
**=item B<HOST(HOST_INTEGER_BITS)>
**CSNOBOL4: number of bits used to represent SNOBOL4 INTEGER type
**=cut
*/
#define HOST_INTEGER_BITS	2300

/*
**=pea
**=item B<HOST(HOST_REAL_BITS)>
**CSNOBOL4: number of bits used to represent SNOBOL4 REAL type
**=cut
*/
#define HOST_REAL_BITS		2301

/*
**=pea
**=item B<HOST(HOST_POINTER_BITS)>
**CSNOBOL4: number of bits used to represent C pointer type
**=cut
*/
#define HOST_POINTER_BITS	2302

/*
**=pea
**=item B<HOST(HOST_LONG_BITS)>
**CSNOBOL4: number of bits used to represent C long type
**=cut
*/
#define HOST_LONG_BITS		2303

/*
**=pea
**=item B<HOST(HOST_DESCR_BITS)>
**CSNOBOL4: number of bits used to represent SIL "descriptor" type
**=cut
*/
#define HOST_DESCR_BITS		2304

/*
**=pea
**=item B<HOST(HOST_SPEC_BITS)>
**CSNOBOL4: number of bits used to represent SIL "specifier" type
**=cut
*/
#define HOST_SPEC_BITS		2305

/*
**=pea
**=item B<HOST(HOST_CHAR_BITS)>
**CSNOBOL4: number of bits used to represent C char type
**=cut
*/
#define HOST_CHAR_BITS		2306

/*****
 * integer variables;
 */

/*
**=pea
**=item B<HOST(HOST_DYNAMIC_SIZE)>
**CSNOBOL4: size of "dynamic" storage in descriptors
**=cut
*/
#define HOST_DYNAMIC_SIZE	2400

/*
**=pea
**=item B<HOST(HOST_PMSTACK_SIZE)>
**CSNOBOL4: size of pattern match stack in descriptors
**=cut
*/
#define HOST_PMSTACK_SIZE	2401

/*
**=pea
**=item B<HOST(HOST_ISTACK_SIZE)>
**CSNOBOL4: size of interpreter stack in descriptors
**=cut
*/
#define HOST_ISTACK_SIZE	2402

/*****
 * string variables;
 */

/*
**=pea
**=item B<HOST(HOST_SNOLIB_BASE)>
**CSNOBOL4: library base directory in use
**=cut
*/
#define HOST_SNOLIB_BASE	2500

/*
**=pea
**=item B<HOST(HOST_SNOLIB_LOCAL)>
**CSNOBOL4: local, version-independant files
**=cut
*/
#define HOST_SNOLIB_LOCAL	2501

/*
**=pea
**=item B<HOST(HOST_SNOLIB_VLIB)>
**CSNOBOL4: distribution files (version-specific)
**=cut
*/
#define HOST_SNOLIB_VLIB	2502

/*
**=pea
**=item B<HOST(HOST_SNOLIB_VLOCAL)>
**CSNOBOL4: local, version-specific files
**=cut
*/
#define HOST_SNOLIB_VLOCAL	2503

/*
**=pea
**=item B<HOST(HOST_SNOPATH_DIR)>
**CSNOBOL4: return n'th element in search directory list
**=cut
*/
#define HOST_SNOPATH_DIR	2504

/*
**=pea
**=item B<HOST(HOST_SNOLIB_VERS)>
**CSNOBOL4: versioned base directory
**=cut
*/
#define HOST_SNOLIB_VERS	2505

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

/*
**=pea
**=sect SEE ALSO
**B<snobol4>(1),
**B<snobol4func>(1).
**=cut
*/
