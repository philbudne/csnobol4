#include <stdio.h>
#include <string.h>

main() {
#if defined(__i386__) || defined(__x86_64__) || defined(MSVC)

#ifdef __APPLE__
#define PIC
#endif

// http://sam.zoy.org/blog/2007-04-13-shlib-with-non-pic-code-have-inlin
// http://softpixel.com/~cwright/programming/simd/cpuid.php
// http://en.wikipedia.org/wiki/CPUID

#ifdef __GNUC__
#ifndef PIC
#define CPUID(FUNC,AX,BX,CX,DX)		\
    __asm__ __volatile__ (		\
	"cpuid" :			\
	"=a" (AX), "=b" (BX), "=c" (CX), "=d" (DX) : "a" (FUNC));
#else // works for PIC code (default on OSX)
#define CPUID(FUNC,AX,BX,CX,DX)			\
    __asm__ __volatile__(			\
	"pushl %%ebx;"	/* save %ebx */		\
	"cpuid;"				\
	"movl %%ebx, %1;" /* save new %ebx value from cpuid */ \
	"popl %%ebx" : /* restore the old %ebx */ \
	"=a" (AX), "=r" (BX), "=c" (CX), "=d"(DX) : "a" (FUNC) : "cc");
#endif /* PIC */
#endif /* gcc */

#ifdef MSVC
#define CPUID(func,a,b,c,d)\
    asm {\
	mov eax, func\
	cpuid\
	mov a, eax\
	mov b, ebx\
	mov c, ecx\
	mov d, edx\
	}
#endif /* MSVC */

    int a, b, c, d;
    unsigned i;
    volatile int *ip;
    char str[3*4*4+1];			/* three rounds of four ints */

    ip = (int *)str;
    CPUID(0, a, ip[0], ip[2], ip[1]);	/* !! */
    str[12] = 0;
    printf("cpuid vendor: %s\n", str);
    fflush(stdout);			/* in case we crash! */

    // EAX=2: Cache and TLB Descriptor information

    // Get Highest Extended Function Supported
    CPUID(0x80000000, a, b, c, d);

    if (a >= 0x80000004) {
	ip = (int *) str;
	for (i = 0x80000002; i <= 0x80000004; i++) {
	    CPUID(i, ip[0], ip[1], ip[2], ip[3]);
	    ip += 4;
	}
	*(char *)ip = '\0';
	printf("cpuid model: %s\n", str);
	fflush(stdout);
    }

    // EAX=80000005h: L1 Cache and TLB Identifiers
    // EAX=80000006h: Extended L2 Cache Features

    // http://msdn.microsoft.com/en-us/library/ff538624%28v=VS.85%29.aspx
    CPUID(1, a, b, c, d);
    // printf("a %x b %x c %x d %x\n", a, b, c, d);
    if (c & 0x80000000) {
	puts("running under hypervision!");
	fflush(stdout);
    }

#define HVBASE 0x40000000		/* "base for hypervisor leaves" */
    memset(str, 0, sizeof(str));
    ip = (int *) str;
    CPUID(HVBASE, a, ip[0], ip[1], ip[2]);
    if (str[0])
	printf("hypervisor: %s\n", str);
#endif

    return 0;
}
