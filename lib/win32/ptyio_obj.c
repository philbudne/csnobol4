/*
 * $Id$
 * ptyio -- using Windows Pseudo-Console
 * Phil Budne
 * 2020-09-12 (compiles)
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

#include <Windows.h>
#include <process.h>
#include <io.h>			/* _open_osfhandle */

#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>		/* O_RDONLY */

#include "h.h"
#include "io_obj.h"
#include "stdio_obj.h"		/* stdio_wrap, flags2mode */

#define DEFAULT_X_SIZE 24
#define DEFAULT_Y_SIZE 80

struct ptyio_obj {
    struct stdio_obj sio;

    // no bi-directional unnamed pipes
    // (AF_UNIX, but no socketpair)
    // output (to child) is done without stdio
    HANDLE writeh;

    // pseudoconsole
    HPCON hpc;
};

// see above: no bidirectional pipes
static ssize_t
ptyio_write(struct io_obj *iop, char *buf, size_t len) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;

    if (len == 0)
	return 0;

    DWORD sent;
    if (!WriteFile(piop->writeh, buf, len, &sent, NULL))
	return -1;
    return sent;
}

static int
ptyio_flush(struct io_obj *iop) {
    struct stdio_obj *siop = (struct stdio_obj *)iop;

    // output is done direct w/ WriteFile
    return 0;
}

int
ptyio_close(struct io_obj *iop) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;

    puts("ptyio_close");

    // "A final painted frame may arrive on hOutput from the
    // pseudoconsole when (ClosePseudoConsole) is called. It is
    // expected that the caller will drain this information from the
    // communication channel buffer and either present it or discard
    // it. Failure to drain the buffer may cause the
    // Close(?) call to wait indefinitely until it is
    // drained or the communication channels are broken another way."
    //
    // EchoCon.cpp-- "will terminate child process if running"
    puts("ClosePseudoConsole");
    ClosePseudoConsole(piop->hpc);

    puts("close writeh");
    CloseHandle(piop->writeh);

    /* try draining: */
    char buf[1024];

    puts("before loop");
    int cc;
    while ((cc = fread(buf, 1, sizeof(buf), piop->sio.f)) > 0)
	printf("read %d\n", cc);
    puts("fclose");
    fclose(piop->sio.f);

    return 1;				/* OK */
}

#define ptyio_read NULL
#define ptyio_seeko NULL
#define ptyio_tello NULL
#define ptyio_eof NULL
#define ptyio_clearerr NULL
#define ptyio_read_raw NULL

MAKE_OPS(ptyio, &stdio_ops);


// https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
static int
PrepareStartupInformation(HPCON hpc, STARTUPINFOEX* psi) {
    // Prepare Startup Information structure
    ZeroMemory(psi, sizeof(*psi));
    psi->StartupInfo.cb = sizeof(STARTUPINFOEX);

    // Discover the size required for the list
    size_t bytesRequired;
    InitializeProcThreadAttributeList(NULL, 1, 0, &bytesRequired);
    
    // Allocate memory to represent the list
    psi->lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)malloc(bytesRequired);
    if (!psi->lpAttributeList)
	return -1;

    // Initialize the list memory location
    if (!InitializeProcThreadAttributeList(psi->lpAttributeList, 1, 0, &bytesRequired)) {
	free(psi->lpAttributeList);
	return -1;
    }

    // Set the pseudoconsole information into the list
    if (!UpdateProcThreadAttribute(psi->lpAttributeList,
                                   0,
                                   PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                   hpc,
                                   sizeof(hpc),
                                   NULL,
                                   NULL)) {
        free(psi->lpAttributeList);
	return -1;
    }

    return 0;
}

struct io_obj *
ptyio_open(path, flags, dir)
    char *path;
    int flags;
    int dir;				/* 'r' or 'w' */
{
    // XXX first time only: try finding necessary symbols dynamically
    // so executable more universal??
    COORD size = { DEFAULT_X_SIZE, DEFAULT_Y_SIZE };

#if 0 // from EchoCon.cpp:
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Enable Console VT Processing
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    (void) SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)

    // from EchoCon.cpp CreatePseudoConsoleAndPipes
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
	size.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	size.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    // XXX close hConsole?
#endif
	    
    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    // Create communication channels
    // "input" and "output" from child perspective

    // Child side:
    // - Closed after CreateProcess of child application with pseudoconsole object.
    HANDLE inputReadSide, outputWriteSide; 

    // Parent side:
    // - Held and used for communication with the child through the pseudoconsole. 
    HANDLE outputReadSide, inputWriteSide; 

    if (!CreatePipe(&inputReadSide, &inputWriteSide, NULL, 0))
        return NULL;

    if (!CreatePipe(&outputReadSide, &outputWriteSide, NULL, 0))
	goto close_input_pipes;

    HPCON hPC;
    if (CreatePseudoConsole(size, inputReadSide, outputWriteSide, 0, &hPC) < 0) {
	CloseHandle(outputReadSide);
	CloseHandle(outputWriteSide);
    close_input_pipes:
	CloseHandle(inputReadSide);
	CloseHandle(inputWriteSide);
	return NULL;
    }

    // EchoCon.cpp frees child side pipes here
    CloseHandle(inputReadSide);
    CloseHandle(outputWriteSide);

    // wrap the child output pipe in a stdio stream for line buffering
    // XXX _O_[W]TEXT?? based on flags???
    int fd = _open_osfhandle((long)outputReadSide, _O_RDONLY);
    if (fd < 0) {
	ClosePseudoConsole(hPC);
	CloseHandle(outputReadSide);
	CloseHandle(inputWriteSide);
	return NULL;
    }

    // from here down, outputReadSide is owned by fd: call _close
    char mode[MAXMODE];
    flags2mode(flags, mode, 'r');
    FILE *f = fdopen(fd, mode);
    if (!f) {
	_close(fd);
	CloseHandle(inputWriteSide);
	ClosePseudoConsole(hPC);
	return NULL;
    }

    // allocate ptyio_obj early to avoid late disappointment
    struct ptyio_obj *piop = (struct ptyio_obj *)
	stdio_wrap(path, f, sizeof(*piop), &ptyio_ops, flags);
    if (!piop)
	goto error1;

    piop->writeh = inputWriteSide;

    STARTUPINFOEXA si;	// STARTUPINFOA StartupInfo
			// LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
    if (PrepareStartupInformation(hPC, &si) < 0)
	goto error2;

    // here down from SetUpPseudoConsole
    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    // never used!!
    // HANDLE hProcess, hThread
    // DWORD dwProcessId, dwThreadId
    PROCESS_INFORMATION pi;

    // "ASCII" version does not require mutable command line?
    // process information never used: pass NULL?
    if (!CreateProcessA(NULL,	// app name
                        path+2,	// cmd line
                        NULL,	// process attrs
                        NULL,	// thread attrs
                        FALSE,	// inherit handles
                        EXTENDED_STARTUPINFO_PRESENT, // flags
                        NULL,	// use parent env
                        NULL,	// use current dir
                        &si.StartupInfo, // point to first (non-extended) member
                        &pi)) {	// process information
    error2:
	DeleteProcThreadAttributeList(si.lpAttributeList);
        free(si.lpAttributeList);
    error1:
	if (piop)
	    free(piop);
	fclose(f);
	ClosePseudoConsole(hPC);
	CloseHandle(inputWriteSide);
	return NULL;
    }

    // safe now?? else locate in piop until close.
    // never used, must be freed:
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    DeleteProcThreadAttributeList(si.lpAttributeList);
    free(si.lpAttributeList);

    return &piop->sio.io;
}
