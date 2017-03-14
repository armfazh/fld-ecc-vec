#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <immintrin.h>

#define ALIGN_BYTES 32
#ifdef __INTEL_COMPILER
#define ALIGN __declspec(align(ALIGN_BYTES))
#else
#define ALIGN __attribute__ ((aligned (ALIGN_BYTES)))
#endif

typedef uint8_t * (*ZeroOperandReturnKey)();
typedef void (*OneOperandGeneric)(void* C);


#endif /* _UTIL_H_ */

