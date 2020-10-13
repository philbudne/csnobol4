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

#ifdef __LP64__
#define VARS_SECTION_TYPE section_64
#else
#define VARS_SECTION_TYPE section
#endif

#define START_VARS_SECTION start_vars_section
#define STOP_VARS_SECTION stop_vars_section

#define GET_VARS_SECTION_LIMITS do {
    sect = getsectbyname(VARS_SEGMENT, VARS_SECTION); \
    if (!sect) abort(); \
    char *START_VARS_SECTION = (char *) sect->addr; \
    char *STOP_VARS_SECTION = (char *) (sect->addr + sect->size); \
} while (0)
#else /* not __APPLE__ */

#define VARS_SECTION_PREFIX

#define START_VARS_SECTION VARS_CONC(__start_,VARS_SECTION_NAME)
#define STOP_VARS_SECTION VARS_CONC(__stop_,VARS_SECTION_NAME)
extern char START_VARS_SECTION[1], STOP_VARS_SECTION[1];

#define GET_VARS_SECTION_LIMITS (void) 0

#endif

#define ZERO_VARS do { \
    GET_VARS_SECTION_LIMITS; \
    bzero(START_VARS_SECTION, STOP_VARS_SECTION - START_VARS_SECTION); \
} while (0)
#define VAR __attribute__((section (VARS_SECTION_PREFIX VARS_SECTION)))
