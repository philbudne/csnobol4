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
**** TALK ABOUT config/xxxx.h ****
.NH 2
#ifdef's in include files
.NH 3
DCMP_BYTES
.PP
** TO BE SUPPLIED **
.NH 3
INT_T
.PP
** TO BE SUPPLIED **
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
** TO BE SUPPLIED **
.NH 3
VFLD_T
.PP
** TO BE SUPPLIED **

.NH 2
other #define's
.NH 3
SNOLIB_DIR
.PP
** TO BE SUPPLIED **
.NH 3
SNOLIB_A
.PP
** TO BE SUPPLIED **

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
substituted (see config/generic.h)

.NH 2
MOVBLK macro
.PP
Uses bcopy() to move blocks of descriptors. The source and destination
may overlap, a situation which many versions of memcpy() will not deal
with correctly!!
.PP
ANSI C memmove() handles overlap, but may be inefficient.
.PP
An efficient C bcopy routine is provided (see config/generic.h)
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
A generic C bzero routine is provided (see config/generic.h)
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
config/generic.h).
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
uses times() and #define'ed HZ (assumes 60 ticks/sec).
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




