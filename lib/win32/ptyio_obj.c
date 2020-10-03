//#define PTYIO_DEBUG // TEMP
/*
 * $Id$
 * ptyio I/O Object using Windows Pseudo-Console
 * Phil Budne
 * 2020-09-12
 *
 * NOTE!! PseudoConsole output stream filled with escape sequences
 * painting the state of the (virtual) console window.
 * Have a state machine to eat them??  In read_raw??????
 * <ESC>[2J	clear screen
 * <ESC>[?25h	hide cursor
 * <ESC>[?25l	show cursor
 * <ESC>[H	home
 * <ESC>[<ROW>;<COL>H	abs position
 * <ESC>[<N>X	erase <N>
 * <ESC>[<N>C	forward <N>
 * <ESC>[m	turn off char attrs
 * <ESC>[6n	query cursor position (expects <ESC>[<line>;<col>R)
 * <ESC>]0; ... window title .... <BEL>
 */

/*
 * N.B.
 * https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
 * says:
 *   To prevent race conditions and deadlocks, we highly recommend
 *   that each of the communication channels is serviced on a separate
 *   thread that maintains its own client buffer state and messaging
 *   queue inside your application. Servicing all of the pseudoconsole
 *   activities on the same thread may result in a deadlock where one
 *   of the communications buffers is filled and waiting for your
 *   action while you attempt to dispatch a blocking request on
 *   another channel.
 *
 * and:
 *   "File or I/O device handles like a file stream or pipe are
 *   acceptable as long as an OVERLAPPED structure is not required for
 *   asynchronous communication."
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#if defined(__CYGWIN_) || defined(__MINGW32__)
// works with w32api-headers 8.0.0-1
// and mingw-w64-x86-64-dev 7.0 headers
#define NTDDI_VERSION NTDDI_WIN10_RS5
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#include <windows.h>
#include <process.h>

#include <stdio.h>			/* NULL, size_t */
#include <malloc.h>

#include "h.h"
#include "io_obj.h"
#include "bufio_obj.h"

#define SUPER bufio_ops

#define DEFAULT_X_SIZE 80		/* width */
#define DEFAULT_Y_SIZE 24		/* height */

// Allow commands like "dir"
// do getenv("COMSPEC") in case default command interpreter changes??
#define CMD_OPT "cmd.exe /c "

#ifdef PTYIO_DEBUG
#define DPRINTF(X) printf X
#define DPUTS(X) puts(X)
#else
#define DPRINTF(X)
#define DPUTS(X)
#endif

// pty buffer (Seagate/KIP/FastPath had pbufs!)
#define PBUF_SIZE 512
struct pbuf {
    struct pbuf *next;
    char *rp;				/* read ptr */
    DWORD count;			/* bytes to be read */
    char buf[PBUF_SIZE];
};

struct ptyio_obj {
    struct bufio_obj bio;

    char reader_exited;			// reader thread exited
    
    HANDLE readh, writeh;		// pipes to pseudoconsole
    HPCON pconsoleh;			// pseudoconsole handle
    HANDLE processh;			// subprocess handle
    HANDLE sthreadh;			// subprocess main thread handle
    HANDLE rthreadh;			// pty_read_thread handle
    HANDLE pthreadh;			// pty_process_thread handle

    struct pbuf *threadbuf;		// being read into by read_thread
    struct pbuf *head, *tail;		// queue, guarded by mutex
    struct pbuf *rrbuf;			// currently owned by read_raw

    HANDLE mutex;			// guards head/tail/threadbuf pointers
    HANDLE readable;			// event: queue non-empty
};

#define GETMUTEX(PIOP) \
    (WaitForSingleObject((PIOP)->mutex, INFINITE) == WAIT_OBJECT_0)
#define RELMUTEX(PIOP) ReleaseMutex((PIOP)->mutex)

/*
 * perform runtime lookups for symbols that may not be available
 * (and keep executable from even running)
 */
#define SYMBOLS \
    SYM(HRESULT, CreatePseudoConsole, \
	(COORD, HANDLE, HANDLE, DWORD, HPCON*)); \
    SYM(VOID, ClosePseudoConsole, (HPCON)); \

/* create storage for pointers to functions */
#define SYM(RET, NAME, PROTO) RET (WINAPI *p##NAME) PROTO;
SYMBOLS
#undef SYM

// NOTE! same storage class as above symbol pointers!!
int pty_init_done = 0;			// 1 if done, -1 if failed

static int
pty_init(void) {
    if (pty_init_done)			// done this already?
	return pty_init_done;		// return old result

    // XXX cygwin checks if "legacy mode" set (and punts)

    pty_init_done = -1;			// assume the worst
    HMODULE kh = GetModuleHandle("kernel32.dll");
    if (kh == INVALID_HANDLE_VALUE)
	return pty_init_done;

    do {
	FARPROC f;

	// generate code for symbol lookups
#define SYM(RET, NAME, PROTO)			\
	f = GetProcAddress(kh, #NAME);		\
	if (!f) break;				\
	p##NAME = (RET (WINAPI *) PROTO) f

	SYMBOLS

	// here with all symbols
	pty_init_done = 1;
    } while (0);
    CloseHandle(kh);		// library handle
    return pty_init_done;
}

/****************
 * worker thread to read from PseudoConsole
 */

static DWORD WINAPI
pty_read_thread(LPVOID arg) {
    struct ptyio_obj *piop = (struct ptyio_obj *)arg;

    for (;;) {
	struct pbuf *p;

	if (!(p = piop->threadbuf)) {
	    // ptyio_close takes mutex before killing threads
	    // so this avoids leaking a buffer on close.
	    DPUTS("thread: getting mutex to malloc threadbuf");
	    if (!GETMUTEX(piop)) {
		puts("thread: threadbuf getmutex failed");
		break;
	    }
	    // check a freelist (if global will need another lock)
	    p = piop->threadbuf = malloc(sizeof(struct pbuf));
	    p->next = NULL;
	    p->count = 0;
	    p->rp = p->buf;
	    DPUTS("thread: releasing mutex");
	    RELMUTEX(piop);
	}
	// blocking read: can't hold mutex!!!
	DPUTS("thread: reading");
	if (!ReadFile(piop->readh, p->buf, sizeof(p->buf), &p->count, NULL)) {
	    DPUTS("thread: ReadFile failed ****************");
	    break;
	}
	DPRINTF(("thread: Read %u to %p (p@%p)\n", p->count, p->buf, p));
	if (!p->count)
	    continue;

#ifdef DEBUG_PTY_READ_THREAD
	{
	    int i = p->count;
	    char *cp = p->buf;
	    printf("thread: ");
	    while (i-- > 0) {
		char c = *cp++;
		if (c < ' ') printf(" %#o", c);
		else printf(" %c", c);
	    }
	    putchar('\n');
	}
#endif
	DPRINTF(("thread: waiting for mutex to enqueue %d p@%p\n",
		 p->count, p));
	if (!GETMUTEX(piop)) {
	    puts("thread: enqueue getmutex failed");
	    break;
	}

	if (piop->tail) {
	    DPRINTF(("thread: setting tail, buf %p next to %p\n",
		     piop->tail, p));
	    piop->tail->next = p;
	    piop->tail = p;
	}
	else {
	    DPRINTF(("thread: setting head & tail to %p\n", p));
	    piop->head = piop->tail = p;
	}
	piop->threadbuf = NULL;
	DPUTS("thread: setting readable");
	SetEvent(piop->readable);
	DPUTS("thread: releasing mutex");
	RELMUTEX(piop);
    }
    DPUTS("thread: exiting ********************************");
    piop->reader_exited = 1;
    SetEvent(piop->readable);
#ifdef PTYIO_DEBUG
    fflush(stdout);
#endif
    return 0;
}

/****************
 * worker thread to wait on process main thread
 * and shut down pseudoconsole
 */
static DWORD WINAPI
pty_process_thread(LPVOID arg) {
    struct ptyio_obj *piop = (struct ptyio_obj *)arg;

    DPUTS(("process_thread"));
    // wait on main thread of subprocess:
    if (WaitForSingleObject(piop->sthreadh, INFINITE) == WAIT_OBJECT_0) {
	DPUTS("process_thread: awake *************************************");
	// "A final painted frame may arrive from the
	// pseudoconsole when (ClosePseudoConsole) is called."
	if (piop->pconsoleh) {
	    DPRINTF(("process_thread: closing pseudoconsole %p\n",
		     piop->pconsoleh));
	    (*pClosePseudoConsole)(piop->pconsoleh);
	    piop->pconsoleh = NULL;
	}
	else
	    puts("process_thread: saw null pconsole handle"); /* XXX TEMP */
    }
    else
	puts("process_thread: wait failed?"); /* XXX TEMP? */
    return 0;
}

/****************************************************************
 * ptyio methods
 */

static ssize_t
ptyio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;
    struct pbuf *p;
    DPRINTF(("read_raw: len %zd\n", len));

    // NOTE: only copies one buffer's worth

    if (!(p = piop->rrbuf)) {
	while (!(p = piop->head)) {
	    if (piop->reader_exited) {
		DPUTS("read_raw: saw reader_exited1");
		return -1;
	    }
	    DPUTS("read_raw: waiting for readable");
	    WaitForSingleObject(piop->readable, INFINITE);
	}
	// here with p pointing to head of queue
	DPRINTF(("read_raw: got %d (p@%p)\n", p->count, p));
	if (!GETMUTEX(piop)) {
	    DPUTS("read_raw: getmutex failed");
	    return -1;
	}
	DPRINTF(("read_raw: got mutex. setting head to %p\n", p->next));
	piop->head = p->next;		// unlink from queue
	if (!piop->head) {
	    DPUTS("read_raw: clearing readable");
	    piop->tail = NULL;
	    ResetEvent(piop->readable);
	}
	piop->rrbuf = p;
	DPUTS("read_raw: releasing mutex");
	RELMUTEX(piop);
    }

    // buffer pointer in piop->rrbuf
    DPRINTF(("read_raw: %zd requested, p@%p p->count = %d\n", len, p, p->count));
    if (len > p->count)
	len = p->count;
    DPRINTF(("read_raw: copying %zd from %p (p@%p) to %p\n", len, p->rp, p, buf));
    //DPRINTF(("read_raw: copying <<%*.*s>>\n", (int)len, (int)len, p->rp));
    memcpy(buf, p->rp, len);
    p->count -= len;
    DPRINTF(("read_raw: %zd copied, p->count = %d\n", len, p->count));
    if (p->count == 0) {	// buffer drained
	free(p);		// put on a free list?
	piop->rrbuf = NULL;
    }
    else
	p->rp += len;

    return len;
}

static ssize_t
ptyio_write(struct io_obj *iop, char *buf, size_t len) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;
    DWORD sent;

    if (len == 0)
	return 0;

    if (!WriteFile(piop->writeh, buf, len, &sent, NULL))
	return -1;
    return sent;
}

static int
ptyio_flush(struct io_obj *iop) {
    // output is done direct w/ WriteFile
    return TRUE;
}

int
ptyio_close(struct io_obj *iop) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;

    // "A final painted frame may arrive on hOutput from the
    // pseudoconsole when (ClosePseudoConsole) is called. It is
    // expected that the caller will drain this information from the
    // communication channel buffer and either present it or discard
    // it. Failure to drain the buffer may cause the
    // Close(?) call to wait indefinitely until it is
    // drained or the communication channels are broken another way."
    //
    // EchoCon.cpp-- "will terminate child process if running"

    // kill process watcher to avoid race for ClosePC
    DPUTS("ptyio_close: terminating process_thread");
    TerminateThread(piop->pthreadh, 0);

    if (piop->pconsoleh) {
	DPRINTF(("ptyio_close: ClosePseudoConsole %p\n", piop->pconsoleh));
	(*pClosePseudoConsole)(piop->pconsoleh);
    }

    // ClosePseudoConsole should cause pipe thread & process to exit
    DPUTS("ptyio_close: waiting for process");
    // XXX do a WaitForMultipleObjects w/ rthread as well?
    if (WaitForSingleObject(piop->processh, 10000) != WAIT_OBJECT_0) {
	DPUTS("ptyio_close: process wait failed");
    }

    DPRINTF(("ptyio_close: close read pipe %p\n", piop->readh));
    CloseHandle(piop->readh);
    DPRINTF(("ptyio_close: close write pipe %p\n", piop->writeh));
    CloseHandle(piop->writeh);

    // Unless/until we know rthread has exited (see above)
    // get mutex before terminating reader thread to avoid
    // losing threadbuf:
    if (!piop->reader_exited) {
	DPUTS("ptyio_close: getting mutex");
	(void) GETMUTEX(piop);

	DPUTS("ptyio_close: terminating reader");
	TerminateThread(piop->rthreadh, 0);
    }

    DPUTS("ptyio_close: closing handles");
    CloseHandle(piop->rthreadh);
    CloseHandle(piop->pthreadh);
    CloseHandle(piop->mutex);
    CloseHandle(piop->readable);
    CloseHandle(piop->processh);
    CloseHandle(piop->sthreadh);

    DPUTS("ptyio_close: freeing pbufs");
    if (piop->threadbuf)
	free(piop->threadbuf);
    if (piop->rrbuf)
	free(piop->rrbuf);
    for (struct pbuf *next, *p = piop->head; p; p = next) {
	next = p->next;
	free(p);
    }

    if (piop->bio.buffer) {
	free(piop->bio.buffer);
	piop->bio.buffer = NULL;
    }
    return 1;				/* OK */
}

#define ptyio_getline NULL		/* use bufio */
#define ptyio_seeko NULL		/* use bufio */
#define ptyio_tello NULL		/* use bufio */
#define ptyio_eof NULL			/* use bufio */
#define ptyio_clearerr NULL		/* use bufio */

MAKE_OPS(ptyio, &SUPER);

struct io_obj *
ptyio_open(path, flags, dir)
    char *path;
    int flags;
    int dir;				/* 'r' or 'w' (ignored!) */
{
    if (path[0] != '|' || path[1] != '|')
	return NOMATCH;

    if (pty_init() < 0)
	return FALSE;

    COORD size = { DEFAULT_X_SIZE, DEFAULT_Y_SIZE };

    // allocate ptyio_obj early to avoid late disappointment
    struct ptyio_obj *piop = (struct ptyio_obj *)
	io_alloc(sizeof(*piop), &ptyio_ops, flags);
    if (!piop)
	return NULL;

    // must be able to hold a PBUF's worth of data
    // (_could_ have read_raw play pointer games instead of copying)
    piop->bio.buffer = malloc(piop->bio.buflen = PBUF_SIZE);
    if (!piop->bio.buffer) {
	free(piop);
	return NULL;
    }

    piop->mutex = CreateMutex(NULL, FALSE, NULL); /* defsec, unowned, no name */
    if (!piop->mutex)
	goto free_piop;

    // default security, manual-reset event, init: not signalling, no name
    piop->readable = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!piop->readable)
	goto close_mutex;

#if 0 // from EchoCon.cpp
    // [PLB: See comments below.  Copying actual console window size,
    // and enabling escape sequence (VT processing) makes sense
    // primarily if the output is going to be displayed in the window
    // that snobol4.exe was invoked in (as EchoCon does), which I'm
    // not convinced is the likely case.  BSD/POSIX/Linux/Un*x forkpty
    // call defaults to an unsized pseudo terminal.]

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Enable Console VT (escape) Processing
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    (void) SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // from EchoCon.cpp CreatePseudoConsoleAndPipes
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
	size.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	size.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif // code from EchoCon
	    
    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    // Create communication channels
    // "input" and "output" directions from child perspective

    // Child side:
    // - Closed after CreateProcess of child application with PC object.
    HANDLE inputReadSide, outputWriteSide; 

    // Parent side:
    // - Held and used for communication with the child through the PC
    HANDLE outputReadSide, inputWriteSide; 

    if (!CreatePipe(&inputReadSide, &inputWriteSide, NULL, 0))
	goto close_readable;
    DPRINTF(("input pipes %p %p\n", inputReadSide, inputWriteSide));

    if (!CreatePipe(&outputReadSide, &outputWriteSide, NULL, 0))
	goto close_input_pipes;
    DPRINTF(("output pipes %p %p\n", outputReadSide, outputWriteSide));

    // cygwin passes 1 (INHERIT_CURSOR) in flags
    // which causes pseudoconsole output of cursor query sequence <ESC>[6n
    // (and expects a response!!)

    // https://github.com/microsoft/terminal/issues/235#issuecomment-414375514
    // (ConPTY repeats character at top left of buffer during every operation)
    // where zadjii-msft commented on Aug 20, 2018
    //   You can however disable this feature by passing the
    //   INHERIT_CURSOR flag to CreatePseudoConsole - though be
    //   warned, you'll need to make sure that the console is first
    //   set to ENABLE_VIRTUAL_TERMINAL_PROCESSING, or your
    //   application is running in a terminal capable of responding to
    //   a DSR. That'll trick conpty into using the terminal's current
    //   cursor position as a starting place for it's own cursor.
    //
    // Not yet sure what this wins, but I have to say it seems to me
    // that MS is unclear on why pty's are useful: yes, you might
    // display the output in a (real console) window [if you're
    // implementing a "screen" program], but it's just as (or more)
    // likely that you're invoking a program under program control,
    // and the output isn't going to be displayed ANYWHERE!

    HRESULT hr =
	(*pCreatePseudoConsole)(size, inputReadSide, outputWriteSide, 0,
				&piop->pconsoleh);
    if (hr != 0) {
	DPRINTF(("CreatePseudoConsole failed %#x\n", hr));
	CloseHandle(outputReadSide);
	CloseHandle(outputWriteSide);
    close_input_pipes:
	CloseHandle(inputReadSide);
	CloseHandle(inputWriteSide);
    close_readable:
	CloseHandle(piop->readable);
    close_mutex:
	CloseHandle(piop->mutex);
    free_piop:
	if (piop->bio.buffer)
	    free(piop->bio.buffer);
	free(piop);
	return NULL;
    }

    DPRINTF(("pconsoleh %p\n", piop->pconsoleh));

    // EchoCon.cpp frees child side pipes here
    CloseHandle(inputReadSide);
    CloseHandle(outputWriteSide);

    piop->writeh = inputWriteSide;
    piop->readh  = outputReadSide;

    // truncates w/o extra byte (but sizeof should include NUL byte?!)??
    int cmd_line_size = sizeof(CMD_OPT) + strlen(path+2) + 1;
    char *cmd_line = malloc(cmd_line_size);
    if (!cmd_line)
	goto close_parent_pipes_and_console;
    snprintf(cmd_line, cmd_line_size, "%s %s", CMD_OPT, path+2);
    DPRINTF(("cmd_line: %s\n", cmd_line));

    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    // Prepare Startup Information structure
    STARTUPINFOEXA si;	// STARTUPINFOA StartupInfo +
			// LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList

    ZeroMemory(&si, sizeof(si));
    si.StartupInfo.cb = sizeof(STARTUPINFOEX);

    {
	// Discover the size required for the list
	SIZE_T bytesRequired;
	InitializeProcThreadAttributeList(NULL, 1, 0, &bytesRequired);
    
	// Allocate memory to represent the list
	si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)malloc(bytesRequired);
	if (!si.lpAttributeList)
	    goto free_cmd_line;

	// Initialize the list memory
	if (!InitializeProcThreadAttributeList(si.lpAttributeList,
					       1, 0, &bytesRequired))
	    goto free_si_attrlist;
    }
    // Set the pseudoconsole information in the attribute list
    if (!UpdateProcThreadAttribute(si.lpAttributeList,
                                   0,
                                   PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                   piop->pconsoleh,
                                   sizeof(piop->pconsoleh),
                                   NULL,
                                   NULL))
	goto free_si_attrlist;

    // here down from SetUpPseudoConsole
    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    PROCESS_INFORMATION pi;	// HANDLE hProcess, hThread;
				// DWORD dwProcessId, dwThreadId;

    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL,	// get executable from command line
                        cmd_line,
                        NULL,	// process attrs
                        NULL,	// thread attrs
                        FALSE,	// inherit handles
                        EXTENDED_STARTUPINFO_PRESENT, // flags
                        NULL,	// use parent env
                        NULL,	// use current dir
                        &si.StartupInfo, // point to first (non-extended) member
                        &pi)) {	// process information
	DPRINTF(("ptyio_open: CreateProcessA: %#x\n", GetLastError()));
    free_si_attrlist:
        free(si.lpAttributeList);
    free_cmd_line:
	DPUTS("@free_cmd_line");
	free(cmd_line);
    close_parent_pipes_and_console:
	(*pClosePseudoConsole)(piop->pconsoleh);
	CloseHandle(inputWriteSide);
	CloseHandle(outputReadSide);
	CloseHandle(piop->readable);
	CloseHandle(piop->mutex);
	free(piop->bio.buffer);
	free(piop);
	return NULL;
    }
    piop->processh = pi.hProcess;
    piop->sthreadh = pi.hThread;

    // safe now?? else locate in piop until close.
    free(cmd_line);

    // not called in cygwin:
    //DeleteProcThreadAttributeList(si.lpAttributeList);
    free(si.lpAttributeList);

    DWORD threadid;
    piop->rthreadh = CreateThread(
	NULL,              // default security
	0,                 // default stack size
	pty_read_thread,   // thread function
	piop,              // thread parameter
	0,                 // default startup flags
	&threadid);
    DPRINTF(("open: rthreadh %p id %u\n", piop->rthreadh, threadid));
    // XXX check return

    piop->pthreadh = CreateThread(
	NULL,              // default security
	0,                 // default stack size
	pty_process_thread, // thread function
	piop,              // thread parameter
	0,                 // default startup flags
	&threadid);
    DPRINTF(("open: pthreadh %p id %u\n", piop->pthreadh, threadid));
    // XXX check return

    return &piop->bio.io;
} // ptyio_open
