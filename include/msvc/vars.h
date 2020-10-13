// $Id$

// defines to put VAR data declarations into own segment
// using Visual C __declspec(allocate("NAME"))


#pragma section(".snob4")

#ifdef NEED_ZERO_VARS
#pragma section(".snob4$a")
__declspec(allocate(".snob4$a")) char start_vars[1];

#pragma section(".snob4$z")
__declspec(allocate(".snob4$z")) char end_vars[1];

#define ZERO_VARS bzero(start_vars, end_vars - start_vars)
#endif /* NEED_ZERO_VARS */

// seems fine after data (storage) class, like C99 __thread
#define VAR __declspec(allocate(".snob4"))
