.\" -*-nroff-*-
.\" $Id$
.TL
Porting The Macro Implementation of SNOBOL4 in C
.AU
Philip L. Budne
.AI
Consultant

.PP
.NH 1
Requirements

.PP
As the title suggests, the target machine is anything that has a C
compiler and provides provides a Un*x-like (ie; POSIX) environment.
The target machine should have;

.IP *
A large address space (allowing for approx 300K code and 300K data).
Virtual memory is desirable.

.IP *
An industrial strength optimizing C compiler;
GNU C (gcc) is preferred, but no ANSI features are required.

.IP *
A reasonably complete C library, including stdio.

.PP
System dependencies are all isolated in library routines and a few C
preprocessor macros (for inline implementation of SIL operations).
The supplied code should be machine independent, and a binary of
snobol4 should not be needed to bootstrap the system.

.NH 1
Config files

.PP
The "autoconf" script determines which file from the config directory
to use, and includes it in config.m4. The config.m4 file is included
into Makefile2.m4 using the m4 macro processor, and contains make
variable assignments (ie; VAR=VALUE) and m4 macro invocations.

.PP
Autoconf checks for the file "local-config" and if it exists includes
it at the end of config.m4.

.NH 2
make variables

.PP
The following are variables which may need to be set in some
environments;

.NH 3
CC

.PP
The path for the C compiler to use.
.NH 3
CCM

.PP
The command to use to create "make depend" output.  The default value
is "./cc-M" If your C compiler (see CC, above) supports this function
directly, CCM can be reset in system.m4.
.NH 3
MATHLIB

.PP
The flags/path to use to link a C program with math routines (may be
needed for the pow() function, or other implementation of exp.c.  The
default value is '-lm'.
.NH 3
OPT

.PP
Optimization, debug, and profiling flags to pass to the C compiler.
The default is "-O".
.NH 3
SNOBOL4

.PP
Name of the main source file to make and use.  Should be either
isnobol4 (the default), which has functions reordered so that gcc can
inline functions more effectively. If you are not using gcc, or not
using the inline-functions option, you can use the value "snobol4" and
avoid the function reordering steps in the Makefile.
.NH 3
PML_OBJS

.PP
Object files not in snolib.a for functions referenced from pml.h
(see load.doc).
.NH 3
Source file path variables.

.PP
All library source file paths are defined in variables, so that
alternate versions may be substituted.  Typical variables of this type
are CONVERT_C, LOAD_C, EXP_C, MSTIME_C.  See the section on library
functions for descriptions of the files, and the alternate versions
included in the distribution.

.NH 2
m4 macros

.PP
Each of the following macros can be used to add files or flags to a
make variable (in place of the += syntax available in some make
programs).  The m4 quote start and end characters have been changed
from accent grave and apostrophe to open and close brackets.  Values
passed to the m4 macros should be quoted for safety.
.NH 3
ADD_CPPFLAGS

.PP
Add argument to CPPFLAGS (used in make depend, .c file compilation).
.NH 3
ADD_OPT

.PP
Add argument to flags used to compile .c files.
.NH 3
ADD_SRCS

.PP
Add argument to list of source files processed in "make depend".
.NH 3
ADD_OBJS

.PP
Add argument to list of object files used to construct snolib.a
.NH 3
ADD_LDFLAGS

.PP
Add argument to flags passed to "cc" command used to link snobol4
executable.

.NH 1
Macro names

.PP
The following are cpp macro names which can be by adding
ADD_CPPFLAGS([-D<symbol>]) or ADD_CPPFLAGS([-D<symbol>=<value>]) to
system.m4 or local-config.

.NH 2
#ifdef's in include files
.NH 3
DCMP_BYTES

.PP
DCMP_BYTES should be defined if sizeof(int_t) != sizeof(real_t).  If
DCMP_BYTES is defined, descriptor equality will be determined using
bcmp().

.PP
Example: ADD_CPPFLAGS([-DDMP_BYTES]).
See also INT_T and REAL_T.
.NH 3
INT_T

.PP
If INT_T is defined, it should be the C type to use for integer values
and addresses.  INT_T must be large enough to hold a pointer. The
default type used for int_t is "long".

.PP
Example: ADD_CPPFLAGS([-DINT_T="long long"]).  See also REAL_T.
.NH 3
NO_BITFIELDS

.PP
Disable use of bitfields to store descriptor "v" (size) field.  Some
compilers have problems with bitfields, yields no storage benefits, or
causes performance penalties.  Bitfields are used to try and fit the
8-bit flags and 24-bit size fields into one 32-bit word.  On Alpha
systems the "a" field is 64-bits long (due to pointer size). Since the
combined "f" and "v" fields will be padded to 64-bits for alignment
there is no benefit to using bitfields in this situation.  See also
VFLD_T.
.NH 3
REAL_T

.PP
If REAL_T is defined, it should be the C type to use for real values.
The default type used for real_t is "float". On Alpha OSF/1 systems
the "a" field will be 64-bits (to hold pointers), so REAL_T is
declared as double to take advantage of the available space.

.PP
Example: ADD_CPPFLAGS([-DREAL_T=double]).  See also INT_T.
.NH 3
VFLD_T

.PP
The descriptor "v" field is used to hold the size of objects in bytes.
When NO_BITFIELDS is not defined, the "v" field is a 24-bit unsigned
bitfield.  When NO_BITFIELDS is defined VFLD_T is used (and defaults
to "int"). VFLD_T should be able to hold 31-bit unsigned quantities.
.NH 3
TMP_DIR

.PP
Directory to use for temporary files (default "/tmp").
.NH 3
HZ

.PP
Used with lib/generic/mstime.c; Number of ticks per second, as returned
by times() system call (default: 60).
.NH 3
SIGFUNC_T

.PP
Return type for signal handler functions (default: void).
.NH 3
NO_PML_EXIT

.PP
Prevents "poor-mans loading" of SPITBOL EXIT() function emulation.
There is a dummy function available for addition to snolib for the
execl() functions (ie; ADD_OBJS([execl.o])) in case execl() is not
available.  This may be preferable to eliminating EXIT() altogether
for compatibility reasons.
.NH 3
NO_PML_HOST

.PP
Prevents "poor-mans loading" of SPITBOL HOST() function emulation.
There are dummy functions available for addition to snolib for the
system() and getenv() functions (ie; ADD_OBJS([system.o])) in case
either or both functions are not available.
.NH 3
NO_PML_REAL

.PP
Prevents "poor-mans loading" of the following functions; SQRT(),
EXP(), LOG(), CHOP(), The functions can also be eliminated
individually by setting one of NO_PML_SQRT, NO_PML_EXP, NO_PML_LOG, or
NO_PML_CHOP.  Setting NO_PML_TRIG eliminates SIN(), COS() and TAN().
.NH 3
NO_PML_FILE

.PP
Prevents "poor-mans loading" of the SITBOL compatible FILE() predicate.
.NH 3
NO_PML_DELETE

.PP
Prevents "poor-mans loading" of the SITBOL compatible DELETE() function.
.NH 3
NO_PML_RENAME

.PP
Prevents "poor-mans loading" of the SITBOL compatible RENAME() function.

.NH 2
Other #ifdefs
.NH 3
NO_OFF_T

.PP
Should be set on systems where there is no system supplied "off_t"
typedef available in either <sys/types.h> or <unistd.h>.  The "long"
datatype will be used for file offsets in this case.

.NH 1
Macros

.PP
The are macros defined in macros.h which call may call library functions.

.NH 2
DCMP macro

.PP
DCMP compares two descriptors.  By default it compares the members one
at a time.  On systems where int_t (see INT_T) does not access the
entire "a" field, DCMP_BYTES may need to be defined, in which case
DCMP calls the "bcmp()" library function.

.NH 2
MOVBLK macro

.PP
Uses bcopy() to move blocks of descriptors. The source and destination
may overlap, a situation which many versions of memcpy() will not deal
with correctly!!

.PP
ANSI C memmove() handles overlap, but may be inefficient.

.PP
A portable, efficient C bcopy routine is provided in lib/auxil/bcopy.c

.NH 2
APDSP macro

.PP
Uses bcopy() to append strings.  See MOVBLK macro.

.NH 2
ZERBLK macro

.PP
Uses bzero() to clear blocks of descriptors.

.PP
A generic C bzero routine is provided lib/auxil/bzero.c

.NH 1
Library functions

.PP
The remainder of this file describes the various system-dependent
library files, the functions they provide, and the C library functions
they require.

.NH 2
convert.c

.PP
Convert from strings to numbers.  Two versions of convert.c are
provided: lib/ansi/convert.c which uses strtol() and strtod(), and
lib/generic/convert.c which uses sscanf().

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
storage regeneration).  Two versions are provided: generic/dynamic.c,
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
pow() is used for integer exponentiation.  It might be more efficient
to decompose y (bitwise) into powers of two and use repeated
multiplies from the series x, x^2, x^4 ...

.NH 2
inet.c

.PP
Provides tcp_open() function to establish a connection to a TCP
server.

.IP lib/bsd/inet.c
Uses sockets.

.IP lib/dummy/inet.c
Provides dummy entry points for systems with no TCP/IP

.IP lib/vms/term.c
NOT TESTED: performs fopen() on SYS$INPUT.

.NH 2
init.c

.PP
Initialization functions init_args() and init().  init_args() is
called from main() to parse command line arguments, using getopt() and
sscanf().  A version of getopt is available in lib/getopt.c.

.PP
init() is called from by the SIL INIT macro (first line of v311.sil)
to initialize storage and signal handling.  init() calls bzero()
and signal().

.NH 2
intspc.c

.PP
Convert integers from binary to strings using sprintf.

.NH 2
io.c

.PP
Provide I/O support (io_xxx) using the C standard I/O package.  Calls:
fopen, fclose, fgets, fprintf, rewind, bzero (see above), malloc,
free, exit, strcpy, strncpy, strlen.

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
Provides user runtime in milliseconds.  Six (!!) versions of mstime.c are
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

.IP ansi/mstime.c
uses clock(), and CLK_TCK, if present.  Uses CLOCKS_PER_SEC, if present,
else defaults to 1000000 (may be overridden by defining CLK_TCK).

.IP borland/mstime.c
Uses clock() returning a wall-clock time not zeroed at program start.

.IP vms/mstime.c
Uses VMS version of times().

.IP dummy/mstime.c
if none of getrusage(), times() or clock() is available.

.NH 2
realst.c

.PP
Convert from binary floating point to string.  Should NOT resort to
use of scientific notation.  Uses sprintf() %f format which is usually
suboptimal (may have trailing zeroes).  Does not use %g format, since
it is less widely available, may resort to scientific notation, and
may omit period and ALL trailing zeroes on whole numbers.

.PP
Better results may be possible by using fconvert(), and gconvert() (or
fcvt() and gcvt()) when available, along with a modicum of
intelligence and/or post-processing.

.NH 2
term.c

.PP
Provides term_input() function which returns a stdio stream (FILE *)
connected to the terminal (typically via the stderr file descriptor)
for read. Three versions are provided;

.IP lib/posix/term.c
uses STDERR_FILENO from <unistd.h> and fdopen().

.IP lib/generic/term.c
uses fdopen() on file descriptor 2.

.IP lib/vms/term.c
NOT TESTED: performs fopen() on SYS$INPUT.

.NH 2
tty.c

.PP
Provides fisatty(), tty_save(), tty_restore(), tty_close(), and
tty_mode() functions for saving and manipulating terminal mode and
echo.

.IP lib/posix/tty.c
Uses termios.h (or SVID termio.h if USE_TERMIO defined).

.IP lib/bsd/tty.c
Uses sgtty.h

.IP lib/dummy/term.c
dummy functions, if none of the above facilities available.

.NH 2
sys.c

.PP
provides hwname() and osname() functions for use by SPITBOL HOST()
function emulation. Two versions are provided. Both check for HWNAME
and OSNAME defines, and use them if they exist.  HWNAME and/or OSNAME
can be set using the SYSDEFS make variable, ie;

.PP
[SYSDEFS=-DHWNAME="yoyodyne-2000" -DOSNAME="yoyo-os 4.0"]
.IP lib/posix/sys.c
Uses <sys/utsname.h> and uname(2) function.
.IP lib/generic/sys.c
Uses output from config.guess script run by autoconf.
