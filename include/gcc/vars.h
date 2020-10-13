// $Id$

// defines to put VAR data declarations into own segment
// using gcc __attribute__

// name of loader section with tracepoints (unquoted)
// changing this requires recompiling trace.c
#define VARS_SECTION_NAME snobol4_vars

// UGH: need two levels in order to evaluate macros!
#define VARS_STR2(X) #X
#define VARS_STR(X) VARS_STR2(X)
#define VARS_SECTION VARS_STR(VARS_SECTION_NAME)

// two levels to allow defines as args
#define VARS_CONC2(X,Y) X##Y
#define VARS_CONC(X,Y) VARS_CONC2(X,Y)

#ifdef __APPLE__
// Mach-O (OS X) wants section("segment,section")
#define VARS_SEGMENT "__DATA"
#define VARS_SECTION_PREFIX VARS_SEGMENT ","

#include <mach-o/getsect.h>
#include <mach-o/dyld.h>
#include <stdlib.h>			/* exit */
#include <stdio.h>			/* fputs */

#ifdef __LP64__
#define mach_section section_64
#define getsectbynamefromheader getsectbynamefromheader_64
#define mach_header mach_header_64
#else
#define mach_section section
#endif

#define START_VARS_SECTION start_vars_section
#define VARS_SECTION_LEN len

#ifdef NEED_ZERO_VARS
static void
get_vars_section_limits(char **startp, size_t *lenp) {
    const struct mach_header *head;
    const struct mach_section *sect = NULL;
#if 0
    // deprecated:
    head = _dyld_get_image_header_containing_address(get_vars_section_limits);
    sect = getsectbynamefromheader(head, VARS_SEGMENT, VARS_SECTION);
#else
    unsigned i, count = _dyld_image_count();
    for (i = 0; i < count; i++) {
	head = (const struct mach_header *)_dyld_get_image_header(i);
	sect = getsectbynamefromheader(head, VARS_SEGMENT, VARS_SECTION);
	if (sect)
	    break;
    }
#endif
    if (!sect) { fputs("fooey! could not find section\n", stderr); exit(1); }
    *startp = ((intptr_t)sect->addr) + (char *)head; /* seems to work! */
    *lenp = sect->size;
    //printf("here2 start %p len %zd\n", *startp, *lenp);
}
#endif

// invoked inside ZERO_VARS do { .... } while (0)
#define GET_VARS_SECTION(START, LEN) \
    get_vars_section_limits(&START, &LEN)

#else /* not __APPLE__ */

#define VARS_SECTION_PREFIX

#define START_VARS_SECTION VARS_CONC(__start_,VARS_SECTION_NAME)
#define STOP_VARS_SECTION VARS_CONC(__stop_,VARS_SECTION_NAME)
extern char START_VARS_SECTION[1], STOP_VARS_SECTION[1];

// invoked inside ZERO_VARS do { .... } while (0)
#define GET_VARS_SECTION(START, LEN) \
    START = START_VARS_SECTION; \
    LEN = STOP_VARS_SECTION - START_VARS_SECTION

#endif

#define ZERO_VARS do { \
    char *start; \
    size_t len; \
    GET_VARS_SECTION(start, len); \
    bzero(start, len); \
} while (0)

#define VAR __attribute__((section (VARS_SECTION_PREFIX VARS_SECTION)))
