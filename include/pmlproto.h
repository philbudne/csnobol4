/* declare extern/prototypes for functions in pml.h */

#ifndef PML_EXTERN
#define PML_EXTERN
#endif

#define PMPROTO(PROTO)
#define PMLFUNC(NAME) PMLFUNC2(#NAME,NAME)
#define PMLFUNC2(NAME,ADDR) PML_EXTERN loadable_func_t ADDR;

#include "pml.h"
