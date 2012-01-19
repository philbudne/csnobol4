#include <stdio.h>
#include <string.h>

int debug = 0;

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_AMD64) || defined(__i386) || defined(__x86_64)
void
cpuid(int f, int v[4]) {
#if defined(__GNUC__) || defined(__SUNPRO_C)
    /* http://en.wikipedia.org/wiki/CPUID */
    __asm__ __volatile__ (
	"mov %%ebx, %%edi;"		/* 32bit PIC: save ebx */
	"cpuid;"
	"mov %%ebx, %%esi;"		/* save ebx output in esi */
	"mov %%edi, %%ebx;"		/* restore ebx */
	: "=a" (v[0]), "=S" (v[1]), "=c" (v[2]), "=d" (v[3]) /* out */
	: "a" (f)			/* in */
	: "edi");			/* clobber */
#endif /* GCC (and clang) or SUNPRO */
#ifdef _MSC_VER
    /* http://en.wikipedia.org/wiki/CPUID */
    __cpuid(v, f);			/* present in VS2005 */
#endif /* _MSC_VER */
    if (debug)
	printf("f %#x: %#x %#x %#x %#x\n", f, v[0],  v[1],  v[2],  v[3]);
}

int
main(int argc, char *argv[]) {
    int v[4];
    unsigned i;
    int *ip;
    char str[3*4*sizeof(int)+1];	/* three rounds of four ints + NUL */

    debug = argc > 1;			/* any arg enables debug */

#ifdef __clang__			/* also defines __GNUC__! */
    printf("CLANG: %d.%d.%d\n",
	   __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("GNUC: %d.%d.%d\n",
	   __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif
#ifdef __SUNPRO_C
    printf("__SUNPRO_C: %#x\n", __SUNPRO_C);
#endif
#ifdef _MSC_VER
    printf("_MSC_VER: %d\n", _MSC_VER);
#endif

    ip = (int *)str;
    cpuid(0, v);
    ip[0] = v[1];			/* ebx */
    ip[2] = v[2];			/* ecx */
    ip[1] = v[3];			/* edx */
    str[12] = 0;
    printf("cpuid vendor: %s\n", str);
    fflush(stdout);			/* in case we crash! */

    // Get Highest Extended Function Supported
    cpuid(0x80000000, v);
    if (v[0] >= 0x80000004) {
	ip = (int *) str;
	for (i = 0x80000002; i <= 0x80000004; i++) {
	    cpuid(i, ip);
	    ip += 4;
	}
	*(char *)ip = '\0';
	printf("cpuid model: %s\n", str);
	fflush(stdout);
    }

    // EAX=1: Processor Info and Feature Bits
    cpuid(1, v);
    if (v[2] & 0x80000000) {
	puts("running under hypervision!");
	fflush(stdout);
    }

#define HVBASE 0x40000000		/* "base for hypervisor leaves" */
    cpuid(HVBASE, v);
    if (v[0] >= HVBASE && v[0] <= HVBASE+0xffff) {
	ip = (int *) str;
	ip[0] = v[1];
	ip[1] = v[2];
	ip[2] = v[3];
	ip[3] = 0;
	printf("hypervisor: %s\n", str);
    }
    return 0;
}
#else
int
main(int argc, char *argv[]) {
    return 0;
}
#endif
