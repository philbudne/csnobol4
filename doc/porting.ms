.\" -*-nroff-*-
.\" $Id$
.TL
Porting The Macro Implementation of SNOBOL4 for Un*x
.AU
Philip L. Budne
.PP
.NH 1
Requirements
.PP
As the title suggests, the target machine is anything that runs Un*x,
or provides a Un*x-like (ie; POSIX) environment.  The target machine
should have;

.IP *
A large address space (allowing for approx 300K code and 300K data).
Virtual memory is desireable.
.IP *
An industrial strength optimizing C compiler;
GNU CC (gcc) is prefered, but no ANSI features are required.
.IP *
A reasonably complete C library, including stdio.
.PP
System dependancies are all isolated in library routines and a few C
preprocessor macros (for inline implementation of SIL operations).
The supplied code should be machine independant, and a binary of
snobol4 should not be needed to bootstrap the system.

.NH 1
Config files
.PP
A configuration file from the config directory must be linked (or
copied) to config.m4 in the top level directory.  The config.m4 file is
included into Makefile2.m4 using the m4 macro processor, and contains
make variable assignments (ie; VAR=VALUE).
.PP
The following are variables which may need to be set in some
enviroments;

.NH 2
CC
.PP
The path for the C compiler to use.  The default is gcc (GNU CC).

.NH 2
CCM
.PP
The command to use to create "make depend" output.  The default value
is "$(CC) -M".  If your C compiler (see CC, above) does not support
the -M flag, a script cc-M is supplied, and CCM=./cc-M. should be
added to your config.m4 file.

.NH 2
MATHLIB
.PP
The flags/path to use to link a C program with math routines (may be
needed for the pow() function, or other implementation of exp.c.  The
default value is '-lm'.

.NH 2
OPT
.PP
Optimization, debug, and profiling flags to pass to the C compiler.
The default is "-O -g".

.NH 2
CONFIG_CFLAGS
.PP
Additional flags passed to the C compiler for C source files.  May
include symbol defines (see the section on Macro Names).  The variable
SNOBOL4_C_CFLAGS can be se to additional C compiler flags to be used
on only snobol4.c (may be required due to thee size of the file).

.NH 2
CONFIG_LDFLAGS
.PP
Additional flags passed to the C compiler to perform the final load
command.  May include system libraries (ie; -lBSD) or linker switches
(ie; -Bstatic).

.NH 2
CONFIG_OBJ
.PP
Additional object files to link into the final image.  Typically one
or more of the support library routines (bcopy.o, bcmp.o, getopt.o,
vfprintf.o). 

.NH 2
CONFIG_SRC
.PP
Additional source files to include in dependancy calculation
(see CONFIG_OBJ, and section on source file path variables).

.NH 2
Source file path variables.
.PP
All library source file paths are defined in variables, so that
alternate versions may be substituted.  Typical variables of this type
are CONVERT_C, LOAD_C, EXP_C, MSTIME_C.  See the section on library
functions for descriptions of the files, and the alternate versions
included in the distribution.

.NH 1
Macro names
.PP
The following are cpp macro names which can be defined via -D<symbol>
or -D<symbol>=<value> flags in the CONFIG_CFLAGS Makefile2 variable.

.NH 2
#ifdef's in include files
.NH 3
DCMP_BYTES
.PP
DCMP_BYTES should be defined if sizeof(int_t) != sizeof(real_t).  If
DCMP_BYTES is defined, decriptor equality will be detwemined using
bcmp().
.PP
Example: -DDMP_BYTES.
See also INT_T and REAL_T.
.NH 3
INT_T
.PP
If INT_T is defined, it should be the C type to use for integer values
and addresses.  The default type used for int_t is "long".
.PP
Example: "-DINT_T=long long".  See also REAL_T.
.NH 3
NO_UFCHK
.PP
** TO BE SUPPLIED **
.NH 3
NO_BITFIELDS
.PP
** TO BE SUPPLIED **
.NH 3
REAL_T
.PP
If REAL_T is defined, it should be the C type to use for real
values.  The default type used for real_t is "float".
.PP
Example: "-DREAL_T=double".  See also INT_T.
.NH 3
VFLD_T
.PP
** TO BE SUPPLIED **

.NH 2
other #define's
.NH 3
SNOLIB_DIR
.PP
Directory for SNOBOL4 library files (default
"/usr/local/lib/snobol4").
.NH 3
TMP_DIR
.PP
Directory to use for temporary files (default "/tmp").
.NH 3
SNOLIB_A
.PP
Filename (in SNOLIB_DIR) for file to use if none supplied to LOAD()
function.
.NH 3
HZ
.PP
Used with ../generic/mstime.c; Number of ticks per second, as returned
by times() system call (default: 60).
.NH 3
SIGFUNC_T
.PP
Return type for signal handler functions (default: void).

.NH 1
Library functions
.PP
The remainder of this file describes the various system-dependant
library files, the functions they provide, and the C library functions
they require.

.NH 2
DCMP macro
.PP
** TO BE SUPPLIED **

.NH 2
LEXCMP macro
.PP
The default implementation uses bcmp(), but memcmp() may be
substituted (see config/generic.m4)

.NH 2
MOVBLK macro
.PP
Uses bcopy() to move blocks of descriptors. The source and destination
may overlap, a situation which many versions of memcpy() will not deal
with correctly!!
.PP
ANSI C memmove() handles overlap, but may be inefficient.
.PP
An efficient C bcopy routine is provided (see config/skel.inc)
in lib/bcopy.c

.NH 2
APDSP macro
.PP
Uses bcopy() to append strings.  See MOVBLK macro.

.NH 2
ZERBLK macro
.PP
Uses bzero() to clear blocks of deccriptors.
.PP
A generic C bzero routine is provided (see config/skel.inc)
in lib/bzero.c

.NH 2
convert.c
.PP
Convert from strings to numbers.  Two versions of convert.c
are provided: ansi/convert.c which uses strtol() and strtod(),
and generic/convert.c which uses sscanf().

.NH 2
date.c
.PP
Contains date() for formatting the current data and time for the
SNOBOL4 DATE() function.  Calls time(), localtime() and sprintf().

.NH 2
dynamic.c
.PP
Allocate dynamic storage region, and forewarn the operating system
that access behavior may be non-sequential (particularly during
strorage regeneration).  Two versions are provided: generic/dynamic.c,
which uses malloc(), and bsd/dynamic.c which calls malloc() and then
vadvise().

.NH 2
endex.c
.PP
Contains endex() which is called to end execution.  Calls exit() for
normal termination and abort() for abnormal termination!

.NH 2
exp.c
.PP
Perform integer and real exponentiation for SNOBOL4 exponentiation
operator.  Two versions are provided: generic/exp.c which uses the
pow() library function, and dummy/exp.c which causes fatal errors (as
specified by the SIL language manual) to be used if suitable library
functions are not available (or if the size of the library functions
is objectionable).
.PP
pow(x,y) is a venerable function and should be widely available.  If
it is not, it could be simulated (ie; in pow.c) using exp() and log(),
or similar functions (such as expm1() and log1p()).
.PP
pow() is used for integer exponentation.  It might be more efficient
to decompose y (bitwise) into powers of two and use repeated
multiplies from the series x, x^2, x^4 ...

.NH 2
init.c
.PP
Initialization functions init_args() and init().  init_args() is
called from main() to parse command line arguments, using getopt() and
sscanf().  A version of getopt is available in lib/getopt.c (see
config/skel.inc).
.PP
init() is called from by the SIL INIT macro (first line of v311.sil)
to initialize storage and signal handling.  init() calls bzero()
and signal().

.NH 2
intspc.c
.PP
Convert integers from binary to strings.

.NH 2
io.c
.PP
Provide I/O support (io_xxx) using the C standard I/O package.  Calls:
fopen, fclose, fgets, rewind, vfprintf, bzero (see above), malloc,
free, exit, strcpy, strncopy, strlen.

.NH 2
load.c
.PP
Provides functions load(), link() and unload() (SIL macros LOAD, LINK,
UNLOAD) for the SNOBOL4 LOAD() and UNLOAD() functions.  Two versions
of load.c are provided: dummy/load.c which causes fatal errors as
specified in the SIL language reference, and bsd/load.c which uses the
loader (ld) to provide a portable implementation for systems using
a.out object format.

.NH 2
mstime.c
.PP
Provides user runtime in milliseconds.  Four versions of mstime.c are
provided:
.IP bsd/mstime.c
uses getrusage() to provide accuracy up to (or better than) a
millisecond.  Use this version when native getrusage() is available
(ie; under SunOS 4.1), as sysconf() usually return a number smaller
than 1000!
.IP posix/mstime.c
uses times() and sysconf().  Use this version when sysconf() is available.
.IP generic/mstime.c
uses times() and #define'ed HZ (defaults to 60 ticks/sec, but may be
overridden).
.IP dummy/mstime.c
if neither getrusage() nor times() is available.

.NH 2
realst.c
.PP
Convert from binary floating point to string.  Should NOT resort to
use of scientific notation.  Uses sprintf() %f format which is usually
suboptimal (may have trailing zeroes).  Does not use %g format, since
it is less widely available, may resort to scientific notation, and
may omit period and ALL traling zeroes on whole numbers.
.PP
Better results may be possible by using fconvert(), and gconvert() (or
fcvt() and gcvt()) when available, along with a modicum of
intelligence and/or post-processing.

.NH 2
vprintf.c
.PP
Provides a varargs version of fprintf() using the old "_doprnt()"
entry point, for use on pre-SVID/XPG2/XPG3/ANSI-C systems (ie; BSD
4.2, BSD 4.3).




