/* $Id$ */

/*
 * O/S initialization for DXE on DJGPP/DOS
 * Burkhard Meissner 2/21/2001
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dxe2.h>
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

#ifdef WATTCP_DOS
#include <sys/socket.h>
#endif

#include <string.h>
#include <stdlib.h>			/* malloc(), getenv() */

int rresvport(int portno);		/* lib/dummy/rresvport.c  */

/*
 * list of symbols exportable to external modules
 *
 * This table lists all those symbols of the global symbol space
 * which can be reached by an external function. This works well
 * for all C-library functions, but may fail for SNOBOL-specific
 * functions and structures.
 */

/*
 * Add more, if functions are to be
 * interfaced. The first 5 and the
 * last one(exit) are essential!
 */

DXE_EXPORT_TABLE_AUTO(xst)
	DXE_EXPORT(malloc)
	DXE_EXPORT(free)
	DXE_EXPORT(bcopy)
	DXE_EXPORT(perror)
	DXE_EXPORT(printf)
	DXE_EXPORT(sprintf)

#if 0					/* does not work - why ? */
	DXE_EXPORT(getstring)
	DXE_EXPORT(retstring)
#endif

	/* stdio.h */
	DXE_EXPORT(fopen)
	DXE_EXPORT(freopen)
	DXE_EXPORT(fclose)
	DXE_EXPORT(fputc)
	DXE_EXPORT(putc)
	DXE_EXPORT(putchar)
	DXE_EXPORT(fgetc)
	DXE_EXPORT(getc)
	DXE_EXPORT(getchar)
	DXE_EXPORT(ungetc)
	DXE_EXPORT(fputs)
	DXE_EXPORT(puts)
	DXE_EXPORT(fgets)
	DXE_EXPORT(gets)
	DXE_EXPORT(fwrite)
	DXE_EXPORT(fread)
	DXE_EXPORT(ferror)
	DXE_EXPORT(fileno)
	DXE_EXPORT(fprintf)
	DXE_EXPORT(sscanf)
	DXE_EXPORT(vfprintf)
	DXE_EXPORT(vsprintf)
	DXE_EXPORT(vprintf)
	DXE_EXPORT(fscanf)
	DXE_EXPORT(scanf)
	DXE_EXPORT(ftell)
	DXE_EXPORT(fgetpos)
	DXE_EXPORT(fseek)
	DXE_EXPORT(fsetpos)
	DXE_EXPORT(rewind)
	DXE_EXPORT(feof)
	DXE_EXPORT(fflush)
	DXE_EXPORT(setbuf)
	DXE_EXPORT(setvbuf)
	DXE_EXPORT(setlinebuf)
	DXE_EXPORT(tmpfile)
	DXE_EXPORT(tmpnam)
	DXE_EXPORT(tempnam)
	DXE_EXPORT(rename)
	DXE_EXPORT(remove)
	DXE_EXPORT(chmod)
	DXE_EXPORT(clearerr)
	DXE_EXPORT(unlink)

	/* unistd.h */
	DXE_EXPORT(close)

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
	DXE_EXPORT(atexit)
	DXE_EXPORT(abort)

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
	DXE_EXPORT(longjmp)	/* dangerous ... */

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

#ifdef WATTCP_DOS
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
#endif

	/* SNOBOL stuff */
	DXE_EXPORT(io_flushall)
	DXE_EXPORT(*dynamic)

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
	DXE_EXPORT(lseek)
	DXE_EXPORT(read)
	DXE_EXPORT(write)

	DXE_EXPORT(exit)
DXE_EXPORT_END

void
os_init()
{
    dlregsym(xst);
}
