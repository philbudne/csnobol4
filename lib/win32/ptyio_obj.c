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

#include <stdio.h>		/* NULL, size_t */
#include <malloc.h>

#include "h.h"
#include "io_obj.h"
#include "bufio_obj.h"

#define SUPER bufio_ops

#define DEFAULT_X_SIZE 24
#define DEFAULT_Y_SIZE 80

#define CMD "C:\\Windows\\system32\\cmd.exe"
#define CMD_OPT "cmd.exe /c "

struct ptyio_obj {
    struct bufio_obj bio;

    // no bi-directional unnamed pipes
    // (AF_UNIX, but no socketpair)
    // so no hope of using just stdio.
    HANDLE readh, writeh;

    // pseudoconsole
    HPCON hpc;

    int eof;
};

/*
 * do runtime lookups for C{reate,lose}PseudoConsole
 * (first supported in Windows 10 1809 (win10 nov. update) / Windows Server 2019)
 */
HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
VOID (WINAPI *pClosePseudoConsole)(HPCON);

static int
pty_init() {
    if (pCreatePseudoConsole && pClosePseudoConsole)
	return TRUE;

    HMODULE h = GetModuleHandle("kernel32.dll");
    if (h == INVALID_HANDLE_VALUE)
	return FALSE;

    FARPROC f = GetProcAddress(h, "CreatePseudoConsole");
    if (f == NULL)
	return FALSE;
    pCreatePseudoConsole = 
	(HRESULT (WINAPI *)(COORD, HANDLE, HANDLE, DWORD, HPCON*))f;

    f = GetProcAddress(h, "ClosePseudoConsole");
    if (f == NULL)
	return FALSE;
    pClosePseudoConsole = (VOID (WINAPI *)(HPCON))f;

    return TRUE;
}

/****************************************************************
 * ptyio methods
 */

static ssize_t
ptyio_read_raw(struct io_obj *iop, char *buf, size_t len) {
    struct ptyio_obj *piop = (struct ptyio_obj *)iop;
    DWORD rcvd;

    printf("ptyio_read_raw %zd\n", len);
    if (!ReadFile(piop->readh, buf, len, &rcvd, NULL)) {
        puts("ptyio_read_raw EOF");
	piop->eof = 1;
	return -1;
    }
    printf("ptyio_read_raw got %ld\n", rcvd);
#if 0
    buf[rcvd] = '\0';
    printf("<<<%s>>>\n", buf);
#endif
    return rcvd;
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

    SUPER.io_close(iop);		/* free buffer */

    // "A final painted frame may arrive on hOutput from the
    // pseudoconsole when (ClosePseudoConsole) is called. It is
    // expected that the caller will drain this information from the
    // communication channel buffer and either present it or discard
    // it. Failure to drain the buffer may cause the
    // Close(?) call to wait indefinitely until it is
    // drained or the communication channels are broken another way."
    //
    // EchoCon.cpp-- "will terminate child process if running"
    printf("ClosePseudoConsole %p\n", piop->hpc);
    (*pClosePseudoConsole)(piop->hpc);

    printf("close write %p\n", piop->writeh);
    CloseHandle(piop->writeh);

#if 0
    /* try draining: */
    char buf[1024];

    puts("before loop");
    int cc;
    while ((cc = ptyio_read_raw(iop, buf, sizeof(buf))) > 0)
	printf("read %d\n", cc);
#endif
    printf("close read %p\n", piop->readh);
    CloseHandle(piop->readh);

    return 1;				/* OK */
}

#define ptyio_read NULL			/* use bufio */
#define ptyio_seeko NULL		/* use bufio */
#define ptyio_tello NULL		/* use bufio */
#define ptyio_eof NULL			/* use bufio */
#define ptyio_clearerr NULL		/* use bufio */

MAKE_OPS(ptyio, &SUPER);


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
    if (!InitializeProcThreadAttributeList(psi->lpAttributeList,
					   1, 0, &bytesRequired)) {
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
    if (path[0] != '|' || path[1] != '|')
	return NOMATCH;

    if (!pty_init())
	return FALSE;

    // XXX first time only: try finding necessary symbols dynamically
    // so executable more universal??
    COORD size = { DEFAULT_X_SIZE, DEFAULT_Y_SIZE };

    // allocate ptyio_obj early to avoid late disappointment
    struct ptyio_obj *piop = (struct ptyio_obj *)
	io_alloc(sizeof(*piop), &ptyio_ops, flags);
    if (!piop)
	return NULL;

#if 0 // from EchoCon.cpp:
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Enable Console VT Processing (so child output interpreted correctly?)
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    (void) SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)

    // from EchoCon.cpp CreatePseudoConsoleAndPipes
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
	size.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	size.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    CloseHandle(hConsole);		/* added by PLB */
#endif
	    
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
        return NULL;
    printf("input pipes %p %p\n", inputReadSide, inputWriteSide);

    if (!CreatePipe(&outputReadSide, &outputWriteSide, NULL, 0))
	goto close_input_pipes;
    printf("output pipes %p %p\n", outputReadSide, outputWriteSide);

    HPCON hPC;
    HRESULT hr;
    hr = (*pCreatePseudoConsole)(size, inputReadSide, outputWriteSide, 0, &hPC);
    if (hr != 0) {
	printf("CreatePseudoConsole failed %#lx\n", hr);
	CloseHandle(outputReadSide);
	CloseHandle(outputWriteSide);
    close_input_pipes:
	CloseHandle(inputReadSide);
	CloseHandle(inputWriteSide);
	free(piop);
	return NULL;
    }

    printf("hpc %p\n", piop->hpc);

    // EchoCon.cpp frees child side pipes here
    CloseHandle(inputReadSide);
    CloseHandle(outputWriteSide);

    piop->writeh = inputWriteSide;
    piop->readh  = outputReadSide;

    int cmd_line_size = sizeof(CMD_OPT) + strlen(path+2) + 100;
    char *cmd_line = malloc(cmd_line_size);
    if (!cmd_line)
	goto close_parent_pipes_and_console;
    snprintf(cmd_line, cmd_line_size, "%s %s", CMD_OPT, path+2);
    printf("cmd_line: %s\n", cmd_line);

    STARTUPINFOEXA si;	// STARTUPINFOA StartupInfo
			// LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
    if (PrepareStartupInformation(hPC, &si) < 0) {
	puts("PrepareStartupInformation failed");
	goto free_cmd_line;
    }

    // here down from SetUpPseudoConsole
    // https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session

    // never used!! handles must be released!!
    // HANDLE hProcess, hThread
    // DWORD dwProcessId, dwThreadId
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(CMD,	// app name
                        cmd_line,
                        NULL,	// process attrs
                        NULL,	// thread attrs
                        FALSE,	// inherit handles
                        EXTENDED_STARTUPINFO_PRESENT, // flags
                        NULL,	// use parent env
                        NULL,	// use current dir
                        &si.StartupInfo, // point to first (non-extended) member
                        &pi)) {	// process information
	printf("CreateProcessA: %#lx\n", GetLastError());
	DeleteProcThreadAttributeList(si.lpAttributeList);
        free(si.lpAttributeList);
    free_cmd_line:
	puts("@free_cmd_line");
	free(cmd_line);
    close_parent_pipes_and_console:
	(*ClosePseudoConsole)(hPC);
	CloseHandle(inputWriteSide);
	CloseHandle(outputReadSide);
	free(piop);
	return NULL;
    }
    puts("CreateProcessA OK!");

    free(cmd_line);

    // safe now?? else locate in piop until close.
    // never used, must be freed:
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);		/* can use with _cwait */
    DeleteProcThreadAttributeList(si.lpAttributeList);
    free(si.lpAttributeList);

    return &piop->bio.io;
}
