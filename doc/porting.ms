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
The Macro Implementation of SNOBOL4 is the original Bell Labs
implementatiton of the SNOBOL4 language, with many SPITBOL and
SNOBOL4+ extensions.  As the title suggests, the target machine is
anything that has a C compiler and provides provides a Un*x-like (ie;
POSIX) environment.  The target machine should have;

.IP \(bu
A large address space (allowing for approx 300K code and 300K data).
Virtual memory is desirable.

.IP \(bu
An industrial strength optimizing C compiler;
GNU C (gcc) is preferred, but no ANSI features are required.

.IP \(bu
A reasonably complete C library, including stdio.

.PP
System dependencies are all isolated in library routines and a few C
preprocessor macros (for inline implementation of SIL operations).
The supplied code should be machine independent, and a binary of
snobol4 should not be needed to bootstrap the system.

.NH 1
Data models

.PP
CSNOBOL4 has been run under both the ILP32 data model (32-bit int/long
and pointer data types) common on Unix systems, as well as the LP64
data model (32-bit int, 64-bit long and pointer data types) available
as an option on recent versions of commercial Unix systems, and the
default on Alpha (and the largely mythical) IA64 architectures.

.PP
It may be possible to compile CSNOBOL4 on an LP32 system (16-bit int,
32-bit long and pointer types), but such environments are uncommon
(Win3.1 and old Macintosh C compilers may use the LP32 data model).

.PP
An experimental feature of the 1.0 release of CSNOBOL4 is to use the
"long long" datatype (available as an extension or with C99 support)
to represent SNOBOL4 INTEGERs, by using the configure --longlong option.

.NH 1
Config files

.PP
The configure script determines which include and library routines are
available, and which suppplied library routines to use.  The outputs
of configure are config.m4, which is included into Makefile2.m4 using
the m4 macro processor, and contains make variable assignments (ie;
VAR=VALUE) and m4 macro invocations, and config.h which is included by
all C sources.

.PP
If the file "local-config" exists, it will be interpolated at the end
of config.m4.  If the file "local-defines" exists, it will be
interpolated at the end of config.h

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
directly, CCM can be reset in local-config.
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
isnobol4 (the default), which has functions reordered so that your C
compilers can inline functions more effectively. If your C compiler
does not perform inlining, and gcc is not available you can use the
value "snobol4".  This may be useful if you are doing development and,
for some reason you cannot build isnobol4.c

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
are LOAD_C, EXP_C, MSTIME_C.  See the section on library
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
C Pre-Processor Macro names

.PP
The following are cpp macro names which can be by adding
#define lines to local-defines.

.NH 2
#ifdef's

The following cpp macro names are #ifdef'ed in source and header files;

.NH 3
HZ

.PP
Used with lib/generic/mstime.c; Number of ticks per second, as returned
by times() system call (default: 60).  Common values are 50 and 100.
.NH 3
INT_T

.PP
If INT_T is defined, it should be the C type to use for integer values
and addresses.  INT_T must be large enough to hold a pointer. The
default type used for int_t is "long", except when the configure --longlong
option is given, and then int_t is "long long".  See also REAL_T.
.NH 3
NO_BITFIELDS

.PP
If NO_BITFIELDS is defined, disable use of a C bitfield to store the
descriptor "v" (size) field.  Some compilers have problems with
bitfields, or cause server performance penalties for their use.
Bitfields are used to try and fit the 8-bit flags and 24-bit size
fields into one 32-bit word.  In LP64 environments (or when configure
--longlong is used) the "a" field is 64-bits long; Since the combined
"f" and "v" fields will be padded to 64-bits for alignment there is no
benefit to using bitfields in this situation.  See also VFLD_T.

.NH 3
NO_FDOPEN

.PP
Disable use of the fdopen() stdio function.  Magic pathname /dev/fd/n
will not be available.
.NH 3
NO_OFF_T

.PP
Should be set on systems where there is no system supplied "off_t"
typedef available in either <sys/types.h> or <unistd.h>.  The "long"
datatype will be used for file offsets in this case.

.NH 3
NO_PML_DELETE

.PP
Prevents "poor-mans loading" of the SITBOL compatible DELETE() function.
.NH 3
NO_PML_EXIT

.PP
Prevents "poor-mans loading" of SPITBOL EXIT() function emulation.
There is a dummy function available for addition to snolib for the
execute() function (ie; ADD_OBJS([execute.o])) in case execl() is not
available.  This may be preferable to eliminating EXIT() altogether
for compatibility reasons.
.NH 3
NO_PML_FILE

.PP
Prevents "poor-mans loading" of the SITBOL compatible FILE() predicate.
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
NO_PML_RENAME

.PP
Prevents "poor-mans loading" of the SITBOL compatible RENAME() function.

.NH 3
NO_UNBUF_RW

.PP
Disable direct use of read(), write() and lseek() for unbuffered
binary I/O.  The stdio fread(), fwrite() plus fflush(), and fseek()
functions will be used for all binary I/O.

.NH 3
OSDEP_OPEN

.PP
If defined, I/O code calls a function named osdep_open() with a
filename string, a mode string (as used by fopen()) and a pointer to a
FILE *.  If the local environment wishes to intercept the named file,
it should attempt (if necessary) to open the necessary file(s), and
fill in the FILE pointer pointer with either NULL (on failure), or a
FILE *.  In either case the osdep_open() call should return TRUE
(meaning the filename has been intercepted), if filename interception
is not attempted, the function should return FALSE.

.NH 3
PML_SERV

.PP
Enable "poor-mans loading" of SERV_LISTEN() function, which can used
to create Internet servers.  SERV_LISTEN() requires a working fork()
call, which completely replicates the state of the system in a new
process.

.NH 3
REAL_T

.PP
If REAL_T is defined, it should be the C type to use for SNOBOL4 REAL
values. The default type used for real_t is "float". In LP64
environments, or when configure --longlong is used the "a" field will
be 64-bits (to hold pointers), so REAL_T is declared as a C "double"
to take advantage of the available space.

.PP
Example: ADD_CPPFLAGS([-DREAL_T=double]).  See also INT_T.
.NH 3
SIGFUNC_T

.PP
Return type for signal handler functions (default: void).
.NH 3
SIZLIM

.PP
May be redefined when when NO_BITFIELDS is defined.  The maximum
object size that can be represented in the VFLD_T data type.  When
NO_BITFIELDS is defined, SIZLIM defaults to 0x7fffffff (maximum
positive 32-bit int).  SIZLIM must NOT appear to be negative when
stored in an "int_t".  When int_t is known to be 64-bits, configure
defines SIZLIM as 0xffffffff. See also INT_T, NO_BITFIELDS, VFLD_T.

.NH 3
TMP_DIR

.PP
Used in lib/bsd/load.c; Directory to use for temporary files (default
"/tmp").
.NH 3
TTY_READ_COOKED

.PP
Call tty_read() function (see tty.c below) to perform "cooked"
(non-binary) reads of terminal devices.
.NH 3
TTY_READ_RAW

.PP
Call tty_read() function (see tty.c below) to perform "raw" (binary)
reads of terminal devices. May be necessary on systems where character
at a time terminal I/O requires special calls.

.NH 3
VFLD_T

.PP
The descriptor "v" field is used to hold the size of objects in bytes.
When NO_BITFIELDS is not defined, the "v" field is always a 24-bit
unsigned bitfield.  When NO_BITFIELDS is defined VFLD_T is used (and
defaults to "unsigned int"). See also SIZLIM.

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
date.c

.PP
Contains date() for formatting the current data and time for the
SNOBOL4 DATE() function.  Calls time(), localtime() and sprintf().

.NH 2
dynamic.c

.PP
Allocate dynamic storage region, and forewarn the operating system
that access behavior may be non-sequential (particularly during
storage regeneration).  Three versions are provided:

.IP lib/posix/dynamic.c
Uses malloc and madvise()

.IP lib/bsd/dynamic.c
Uses malloc and vadvise()

.IP lib/generic/dynamic.c
Uses malloc()

.NH 2
endex.c

.PP
Contains endex() which is called to end execution.  Calls exit() for
normal termination and abort() for abnormal termination!

.NH 2
execute.c

.PP
Provides execute() function for SPITBOL HOST() simulation.

.IP lib/generic/execute.c
Uses execl() and /bin/sh or /usr/bin/sh to pass execute to
a shell command.

.IP lib/dummy/execute.c
Provides dummy execute() function.

.NH 2
exists.c

.PP
Test for file existance.

.IP lib/generic/exists.c
Uses POSIX stat() function.

.IP lib/vms/exists.c
Currently uses stat() function; use native call?

.IP lib/win32/exists.c
Uses WIN32 GetFileAttributesA() call.

.NH 2
exp.c

.PP
Perform integer and real exponentiation for SNOBOL4 exponentiation
operator.  Two versions are provided: generic/exp.c (which uses the
pow() library function for reals and repeated multiplies for
integers), and dummy/exp.c which causes fatal errors (as specified by
the SIL language manual) to be used if suitable library functions are
not available (or if the size of the library functions is
objectionable).

.PP
pow(x,y) is a venerable function and should be widely available.  If
it is not, it could be simulated (ie; in pow.c) using exp() and log(),
or similar functions (such as expm1() and log1p()).

.NH 2
finite.c

.PP
provide finite() function for systems which do not supply
a function to detect abnormal floating point numbers.

.IP lib/dummy/finite.c
Returns false.

.NH 2
inet.c

.PP
Provides tcp_open() function to establish a connection to a TCP
server.

.IP lib/bsd/inet.c
Uses BSD style sockets.

.IP lib/dummy/inet.c
Provides dummy entry points for systems with no TCP/IP

.NH 2
init.c

.PP
Initialization functions init_args() and init().  init_args() is
called from main() to parse command line arguments, using getopt() and
sscanf().  A version of getopt is available in lib/auxil/getopt.c.

.PP
init() is called from by the SIL INIT macro (first line of v311.sil)
to initialize storage and signal handling.  init() calls bzero()
and signal().

.NH 2
intspc.c

.PP
Convert integers from binary to strings.
Three versions are provided;

.IP lib/generic/intspc.c
Uses sprintf() "%ld" format

.IP lib/c99/intspc.c
Uses sprintf() "%lld" format.

.IP lib/longlong/intspc.c
Ad-hoc formatting.

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
UNLOAD) for the SNOBOL4 LOAD() and UNLOAD() functions.

.IP dummy/load.c
causes fatal errors as specified in the SIL language reference.

.IP bsd/load.c
Uses ld to provide a portable implementation for systems using
a.out object format.

.IP unix98/load.c
Uses sun/unix98 dlopen() call to attach shared libraries.

.IP lib/hpux/load.c
Use HP-UX shl_load() function to attach shared libraries.

.IP lib/win32/load.c
Use Win32 LoadLibrary() function to attach dynamic libraries.

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

.IP win32/mstime.c
Uses Win32 GetProcessTimes() or GetSystemTime() calls.

.IP dummy/mstime.c
if none of getrusage(), times() or clock() is available.

.NH 2
popen.c

.PP
Provide popen() and pclose() functions on systems where these
functions are not included in the C library.

.IP lib/generic/popen.c
Uses system() function and ">" and "<" style redirection to
simulate popen() functionality.

.IP lib/vms/popen.c
Uses VMS mailbox and LIB$SPAWN to provide functionality.
Recent versions of the DECC RTL may provide popen().

.IP lib/win32/popen.c
Use MSVC _popen() function.

.IP lib/dummy/popen.c
Dummy functions, always fail.

.NH 2
realst.c

.PP
Convert from binary floating point to string.
Uses "%g" format, will append trailing dot if needed.

.PP
Better results may be possible by using fconvert(), and gconvert() (or
fcvt() and gcvt()) when available, along with a modicum of
intelligence and/or post-processing.

.NH 2
spcint.c

.PP
Convert from strings to binary integers.  Four versions are provided;

.IP lib/ansi/spcint.c
Uses strtol()

.IP lib/generic/spcint.c
Uses sscanf()

.IP lib/c99/spcint.c
Uses strtoll() for support of long long on ILP32 systems.

.IP lib/longlong/spcint.c
Ad hoc support of long long on ILP32 systems.

.NH 2
spreal.c

.PP
Convert from strings to floating point.  Two versions are provided;

.IP lib/ansi/spreal.c
Uses strtod()

.IP lib/generic/spreal.c
Uses sscanf()

.NH 2
suspend.c

.PP
Cause the current process to suspend itself after reciept of a
SIGTSTP signal.

.IP lib/posix/suspend.c
Use sigprocmask() to unblock SIGTSTP.

.IP lib/bsd/dynamic.c
Use sigsetmask() to unblock SIGTSTP.

.IP lib/generic/suspend.c
Use SIGSTOP.

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
performs fopen() on SYS$COMMAND:.

.NH 2
tty.c

.PP
Provides fisatty(), tty_mode() and tty_suspend() functions for saving
and manipulating terminal mode and echo.  May optionally declare a
tty_read() function (see TTY_READ_RAW and TTY_READ_COOKED ifdefs) to
perform reads on terminal devices.

.IP lib/posix/tty.c
Uses termios.h (or SVID termio.h if USE_TERMIO defined).

.IP lib/bsd/tty.c
Uses sgtty.h

.IP lib/borland/tty.c
Provide tty_read() using getch() and getche().

.IP lib/vms/term.c
Declares tty_read() function using VMS SYS$QIOW to perform raw tty
reads.

.IP lib/win32/term.c
Implement tty_mode using SetConsoleMode().

.IP lib/dummy/term.c
dummy functions, if none of the above facilities available.

.NH 2
sys.c

.PP
provides hwname() and osname() functions for use by SPITBOL HOST()
function emulation.

.IP lib/posix/sys.c
Uses <sys/utsname.h> and uname(2) function.
Checks for HWNAME and OSNAME defines.

.IP lib/vms/sys.c
Use SYS$GETSYIW() calls.

.IP lib/win32/sys.c
Use GetSystemInfo() and GetVersionEx() calls.

.IP lib/generic/sys.c
Uses output from config.guess script run by autoconf.
Checks for HWNAME and OSNAME defines.
