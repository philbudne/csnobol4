/* $Id$ */

/*
 * O/S initialization for DXE on DJGPP/DOS
 * Burkhard Meissner 2/21/2001
 *
 * seperated into osinit.c 2/17/2002 -phil
 */

/* all this stuff may increase base executable size
 * for stuff that won't be used; put includes/EXPORT groups
 * under "#ifdef EXPORT_FOO"??
 */

#include <dlfcn.h>
#include <dxe2.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <locale.h>
#include <io.h>
#include <unistd.h>
#include <fcntl.h>
#include <process.h>
#include <dos.h>
#include <conio.h>

#ifdef HAVE_WATTCP
#include <sys/socket.h>
int rresvport(int portno);		/* lib/dummy/rresvport.c  */
#endif

#include <string.h>

#include "h.h"				/* __P */
#include "snotypes.h"			/* struct descr */
#include "lib.h"			/* io_flushall */
#include "load.h"			/* getstring, retstring, etc */

/*
 * list of symbols exportable to external modules
 *
 * This table lists all those symbols of the global symbol space
 * which can be reached by an external function. This works well
 * for all C-library functions, but may fail for SNOBOL-specific
 * functions and structures.
 */

DXE_EXPORT_TABLE(exports)
	DXE_EXPORT(malloc)
	DXE_EXPORT(free)
	DXE_EXPORT(bcopy)
	DXE_EXPORT(perror)
	DXE_EXPORT(printf)
	DXE_EXPORT(sprintf)

	/* load.h */
	DXE_EXPORT(getstring)
	DXE_EXPORT(retstring)
	DXE_EXPORT(io_findunit)
	DXE_EXPORT(io_closeall)
	DXE_EXPORT(io_getfp)
	DXE_EXPORT(io_mkfile)

	/* stdio.h */
	DXE_EXPORT(__dj_stderr)
	DXE_EXPORT(__dj_stdin)
	DXE_EXPORT(__dj_stdout)
	DXE_EXPORT(clearerr)
	DXE_EXPORT(fclose)
	DXE_EXPORT(feof)
	DXE_EXPORT(ferror)
	DXE_EXPORT(fflush)
	DXE_EXPORT(fgetc)
	DXE_EXPORT(fgetpos)
	DXE_EXPORT(fgets)
	DXE_EXPORT(fileno)
	DXE_EXPORT(fopen)
	DXE_EXPORT(fprintf)
	DXE_EXPORT(fputc)
	DXE_EXPORT(fputs)
	DXE_EXPORT(fread)
	DXE_EXPORT(freopen)
	DXE_EXPORT(fscanf)
	DXE_EXPORT(fseek)
	DXE_EXPORT(fsetpos)
	DXE_EXPORT(ftell)
	DXE_EXPORT(fwrite)
	DXE_EXPORT(getc)
	DXE_EXPORT(getchar)
	DXE_EXPORT(gets)
	DXE_EXPORT(putc)
	DXE_EXPORT(putchar)
	DXE_EXPORT(puts)
	DXE_EXPORT(remove)
	DXE_EXPORT(rename)
	DXE_EXPORT(rewind)
	DXE_EXPORT(scanf)
	DXE_EXPORT(setbuf)
	DXE_EXPORT(setlinebuf)
	DXE_EXPORT(setvbuf)
	DXE_EXPORT(sscanf)
	DXE_EXPORT(tempnam)
	DXE_EXPORT(tmpfile)
	DXE_EXPORT(tmpnam)
	DXE_EXPORT(ungetc)
	DXE_EXPORT(vfprintf)
	DXE_EXPORT(vprintf)
	DXE_EXPORT(vsprintf)

	/* unistd.h */
	DXE_EXPORT(close)
	DXE_EXPORT(unlink)
	DXE_EXPORT(usleep)

	/* fcntl.h  */
	DXE_EXPORT(creat)
	DXE_EXPORT(fork)
	DXE_EXPORT(open)

	/* math.h */
	DXE_EXPORT(sin)
	DXE_EXPORT(cos)
	DXE_EXPORT(tan)
	DXE_EXPORT(asin)
	DXE_EXPORT(acos)
	DXE_EXPORT(atan)
	DXE_EXPORT(atan2)
	DXE_EXPORT(sinh)
	DXE_EXPORT(cosh)
	DXE_EXPORT(tanh)
	DXE_EXPORT(pow)
	DXE_EXPORT(sqrt)
	DXE_EXPORT(exp)
	DXE_EXPORT(frexp)
	DXE_EXPORT(ldexp)
	DXE_EXPORT(log)
	DXE_EXPORT(log10)
	DXE_EXPORT(ceil)
	DXE_EXPORT(floor)
	DXE_EXPORT(fabs)
	DXE_EXPORT(fmod)
	DXE_EXPORT(abs)
	DXE_EXPORT(labs)
	DXE_EXPORT(div)
	DXE_EXPORT(ldiv)
	DXE_EXPORT(rand)
	DXE_EXPORT(srand)
	DXE_EXPORT(modf)

	/* ctype.h */
	DXE_EXPORT(isalpha)
	DXE_EXPORT(islower)
	DXE_EXPORT(isupper)
	DXE_EXPORT(isdigit)
	DXE_EXPORT(isxdigit)
	DXE_EXPORT(isalnum)
	DXE_EXPORT(isprint)
	DXE_EXPORT(isgraph)
	DXE_EXPORT(isspace)
	DXE_EXPORT(ispunct)
	DXE_EXPORT(iscntrl)
	DXE_EXPORT(tolower)
	DXE_EXPORT(toupper)
	DXE_EXPORT(toascii)

	/* string.h */
	DXE_EXPORT(strlen)
	DXE_EXPORT(strcpy)
	DXE_EXPORT(strncpy)
	DXE_EXPORT(strcat)
	DXE_EXPORT(strncat)
	DXE_EXPORT(strcmp)
	DXE_EXPORT(strncmp)
	DXE_EXPORT(strcoll)
	DXE_EXPORT(strchr)
	DXE_EXPORT(strrchr)
	DXE_EXPORT(strcspn)
	DXE_EXPORT(strpbrk)
	DXE_EXPORT(strspn)
	DXE_EXPORT(strstr)
	DXE_EXPORT(strtok)
	DXE_EXPORT(strxfrm)

	DXE_EXPORT(memcpy)
	DXE_EXPORT(memmove)
	DXE_EXPORT(memcmp)
	DXE_EXPORT(memchr)
	DXE_EXPORT(memset)
	DXE_EXPORT(strerror)
	DXE_EXPORT(_doprnt)
	DXE_EXPORT(_doscan)
	DXE_EXPORT(_doscan_low)
	DXE_EXPORT(_rename)
	DXE_EXPORT(__dj_ctype_flags)

	/* stdlib.h */
	DXE_EXPORT(atoi)
	DXE_EXPORT(atol)
	DXE_EXPORT(strtol)
	DXE_EXPORT(strtoul)
	DXE_EXPORT(atof)
	DXE_EXPORT(strtod)
	DXE_EXPORT(mblen)
	DXE_EXPORT(mbtowc)
	DXE_EXPORT(wctomb)
	DXE_EXPORT(mbstowcs)
	DXE_EXPORT(wcstombs)
	DXE_EXPORT(qsort)
	DXE_EXPORT(bsearch)
	DXE_EXPORT(calloc)
	DXE_EXPORT(realloc)
	DXE_EXPORT(getenv)
	DXE_EXPORT(system)

	/*  time.h  */
	DXE_EXPORT(time)
	DXE_EXPORT(difftime)
	DXE_EXPORT(gmtime)
	DXE_EXPORT(localtime)
	DXE_EXPORT(ctime)
	DXE_EXPORT(mktime)
	DXE_EXPORT(strftime)
	DXE_EXPORT(asctime)
	DXE_EXPORT(clock)

	/* locale.h */
	DXE_EXPORT(setlocale)
	DXE_EXPORT(localeconv)

	/* signal.h */
	DXE_EXPORT(signal)
	DXE_EXPORT(raise)

	/* setjmp.h */
	DXE_EXPORT(setjmp)
	DXE_EXPORT(longjmp)		/* dangerous ... */

	/* fcntl.h */
	DXE_EXPORT(fcntl)

	/* unistd.h */
	DXE_EXPORT(execl)
	DXE_EXPORT(execle)
	DXE_EXPORT(execlp)
	DXE_EXPORT(execv)
	DXE_EXPORT(execve)
	DXE_EXPORT(execvp)

	/* stat.h */
	DXE_EXPORT(fstat)
	DXE_EXPORT(stat)

	/* process.h */
	DXE_EXPORT(spawnl)
	DXE_EXPORT(spawnle)
	DXE_EXPORT(spawnlp)
	DXE_EXPORT(spawnlpe)
	DXE_EXPORT(spawnv)
	DXE_EXPORT(spawnve)
	DXE_EXPORT(spawnvp)
	DXE_EXPORT(spawnvpe)
	DXE_EXPORT(execlpe)
	DXE_EXPORT(execvpe)

#ifdef HAVE_WATTCP
	/* socket.h */
	DXE_EXPORT(accept)
	DXE_EXPORT(bind)
	DXE_EXPORT(closesocket)
	DXE_EXPORT(connect)
	DXE_EXPORT(ioctlsocket)
	DXE_EXPORT(fcntlsocket)
	DXE_EXPORT(gethostid)
	DXE_EXPORT(getpeername)
	DXE_EXPORT(getsockname)
	DXE_EXPORT(getsockopt)
	DXE_EXPORT(listen)
	DXE_EXPORT(recv)
	DXE_EXPORT(recvfrom)
	DXE_EXPORT(send)
	DXE_EXPORT(sendto)
	DXE_EXPORT(setsockopt)
	DXE_EXPORT(shutdown)
	DXE_EXPORT(socket)
	DXE_EXPORT(rresvport)
#endif /* HAVE_WATTCP */

	/* SNOBOL lib.h */
	DXE_EXPORT(io_flushall)

	/* dos.h */
	DXE_EXPORT(int86)
	DXE_EXPORT(int86x)
	DXE_EXPORT(intdos)
	DXE_EXPORT(bdos)
	DXE_EXPORT(bdosptr)
	DXE_EXPORT(getcbrk)
	DXE_EXPORT(setcbrk)

	/* conio.h */
	DXE_EXPORT(getch)
	DXE_EXPORT(getche)
	DXE_EXPORT(kbhit)
	DXE_EXPORT(putch)
	DXE_EXPORT(ungetch)

	/* io.h */
	DXE_EXPORT(chmod)
	DXE_EXPORT(lseek)
	DXE_EXPORT(read)
	DXE_EXPORT(write)

	/* sys/movedata.h */
	DXE_EXPORT(dosmemget)
	DXE_EXPORT(dosmemput)
DXE_EXPORT_END

void
os_init()
{
    /* not needed if DXE_EXPORT_TABLE_AUTO worked (which it didn't) */
    dlregsym(exports);
}
