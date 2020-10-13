// $Id$

// defines to put VAR data declarations into own segment
// using Visual C __declspec(allocate("NAME"))


#ifdef NEED_ZERO_VARS
#pragma section(".snob4$a", read)
__declspec(allocate(".snob4$a")) char start_vars[1];

#pragma section(".snob4$z",read)
__declspec(allocate(".snob4$z")) char end_vars[1];

#pragma section(".snob4", read)
#define ZERO_VARS bzero(start_vars, end_vars - start_vars)
#endif /* NEED_ZERO_VARS */

// ruh-roh needs to be before storage class?
// if so, define STATIC_VAR and EXTERN_VAR??
#define VAR __declspec(allocate(".snob4"))
