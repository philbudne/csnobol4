#include <stdio.h>
#include <string.h>

// http://sam.zoy.org/blog/2007-04-13-shlib-with-non-pic-code-have-inlin
// http://softpixel.com/~cwright/programming/simd/cpuid.php
// http://en.wikipedia.org/wiki/CPUID

#if 0
#define cpuid(func,ax,bx,cx,dx)		\
	__asm__ __volatile__ ("cpuid":\
	"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));
#else
// works for PIC code
#define cpuid(func,ax,bx,cx,dx)			\
    __asm__ __volatile__(			\
	"pushl %%ebx;"	/* save %ebx */		\
	"cpuid;"				\
	"movl %%ebx, %1;" /* save what cpuid justn put in %ebx */ \
	"popl %%ebx" : /* restore the old %ebx */ \
	"=a" (ax), "=r" (bx), "=c" (cx), "=d"(dx) : "a" (func) : "cc");
#endif
#ifdef MSVC
#define cpuid(func,a,b,c,d)\
    asm {\
	mov eax, func\
	cpuid\
	mov a, eax\
	mov b, ebx\
	mov c, ecx\
	mov d, edx\
	}
#endif

main() {
    int a,b,c,d;
    unsigned i;
    char str[49];

    cpuid(0, a, b, c, d);
    memcpy(str+0, &b, 4);
    memcpy(str+4, &d, 4);
    memcpy(str+8, &c, 4);

    str[12] = 0;
    printf("cpuid vendor: %s\n", str);
    fflush(stdout);

    // EAX=2: Cache and TLB Descriptor information

    // Get Highest Extended Function Supported
    cpuid(0x80000000, a, b, c, d);

    if (a >= 0x80000004) {
	for (i = 0; i < 3; i++) {
	    cpuid(0x80000002+i, a, b, c, d);
	    memcpy(str + i*16, &a, 4);
	    memcpy(str + i*16 + 4, &b, 4);
	    memcpy(str + i*16 + 8, &c, 4);
	    memcpy(str + i*16 + 12, &d, 4);
	}
	str[48] = 0;
	printf("cpuid model: %s\n", str);
    }

    // EAX=80000005h: L1 Cache and TLB Identifiers
    // EAX=80000006h: Extended L2 Cache Features

    return 0;
}

