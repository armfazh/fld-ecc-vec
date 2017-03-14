#ifndef _FAZ_FP_AVX2_H_
#define _FAZ_FP_AVX2_H_

#include "util.h"

/**
 * Structures for Field Arithmetic
 */

#define argElement_1w uint64_t *
#define argElement_2w __m256i *
#define argElement_4w __m256i *
#define argElement_Nw __m256i *

/** For FP25519 */

/** One way */
#define SIZE_FP25519 32
#define BASE0_FP25519 26
#define BASE1_FP25519 25
#define NUM_DIGITS_FP25519 10
typedef ALIGN uint64_t Element_1w_Fp25519[4*((NUM_DIGITS_FP25519+3)/4)];

#define NUM_DIGITS_FP25519_X64 4
typedef ALIGN uint64_t Element_1w_x64[NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t Element_1w_Buffer_x64[2*NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t Element_2w_x64[2*NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t Element_2w_Buffer_x64[4*NUM_DIGITS_FP25519_X64];

/** Two way */
typedef ALIGN __m256i Element_2w_Fp25519[(NUM_DIGITS_FP25519/2)];

/** Four way */
typedef ALIGN __m256i Element_4w_Fp25519[NUM_DIGITS_FP25519];

/** For FP448 */

/** One way */
#define SIZE_FP448 56
#define BASE_FP448 28
#define NUM_DIGITS_FP448 16
typedef ALIGN uint64_t Element_1w_Fp448[NUM_DIGITS_FP448];

/** Two way */
typedef ALIGN __m256i Element_2w_Fp448[(NUM_DIGITS_FP448/2)];

/** Four way */
typedef ALIGN __m256i Element_4w_Fp448[NUM_DIGITS_FP448];


typedef void (*ThreeOperand)(argElement_1w C, argElement_1w A, argElement_1w B);
typedef void (*TwoOperand)(argElement_1w C, argElement_1w A);
typedef int  (*TwoOperandReturn)(argElement_1w C, argElement_1w A);
typedef void (*OneOperand)(argElement_1w C);
typedef argElement_1w (*ZeroOperandReturn)();

typedef void (*ThreeOperandVector)(argElement_2w C, argElement_2w A, argElement_2w B);
typedef void (*TwoOperandVector)(argElement_2w C, argElement_2w A);
typedef int (*TwoOperandReturnVector)(argElement_2w C, argElement_2w A);
typedef void (*OneOperandVector)(argElement_2w C);
typedef argElement_2w (*ZeroOperandReturnVector)();

struct _struct_Fp_1way {
	ThreeOperand add,sub,mul;
	TwoOperand inv,sqrt;
	TwoOperandReturn cmp;
	OneOperand sqr,cred,rand,print;
	OneOperandGeneric clean;
	ZeroOperandReturn new;
};

struct _struct_Fp_Nway {
	ThreeOperandVector add,sub,mul;
	TwoOperandReturnVector cmp;
	OneOperandVector sqr,cred,rand,print;
	OneOperandGeneric clean;
	ZeroOperandReturnVector new;
};

struct _struct_Fp {
	struct _struct_Fp_Arith {
		struct _struct_Fp_1way _1way,_1way_x64;
		struct _struct_Fp_Nway _2way,_4way;
	} fp25519;
	struct _struct_Fp_Arith_ext {
		struct _struct_Fp_1way _1way;
		struct _struct_Fp_Nway _2way,_4way;
	} fp448;
};

extern const struct _struct_Fp Fp;

#endif /* _FAZ_FP_AVX2_H_ */

