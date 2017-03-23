#ifndef _FAZ_FP_AVX2_H_
#define _FAZ_FP_AVX2_H_

//#include "util.h"

#include <stdint.h>
#include <immintrin.h>
//#include <avx2.h>


#ifndef _AVX2_H_ /* _AVX2_H_ */
#define _AVX2_H_

#define HASWELL 0x80
#define	SKYLAKE 0x40
#define PROCESSOR HASWELL 

#include <immintrin.h>

#define ZERO            _mm256_setzero_si256()
#define LOAD(X)         _mm256_load_si256((__m256i*) X)
#define STORE(X,Y)      _mm256_store_si256((__m256i*) X, Y)
/*
 * #define ALIGNR(X,Y)     _mm256_alignr_epi8(X,Y,8)
 * */
#define ALIGNR(X,Y)     _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(Y),_mm256_castsi256_pd(X),0x5))
#define ADD(X,Y)        _mm256_add_epi64(X,Y)
#define SUB(X,Y)        _mm256_sub_epi64(X,Y)
#define AND(X,Y)        _mm256_and_si256(X,Y)
#define ANDNOT(X,Y)     _mm256_andnot_si256(X,Y)
#define XOR(X,Y)        _mm256_xor_si256(X,Y)
#define OR(X,Y)         _mm256_or_si256(X,Y)
#define MUL(X,Y)        _mm256_mul_epu32(X,Y)
#define SHR(X,Y)        _mm256_srli_epi64(X,Y)
#define SHL(X,Y)        _mm256_slli_epi64(X,Y)
#define SHLV(X,Y)       _mm256_sllv_epi64(X,Y)
#define SHRV(X,Y)       _mm256_srlv_epi64(X,Y)
#define SHL_128(X)      _mm256_slli_si256(X,8)
#define SHR_128(X)      _mm256_srli_si256(X,8)
#define SHRi_128(X,Y)   _mm256_srli_si256(X,Y)
#define CLEAN_LOW64(X)  _mm256_blend_epi32(X,ZERO,0x33)
#define CLEAN_HIGH64(X) _mm256_blend_epi32(X,ZERO,0xCC)
#define UPKL64(X,Y)     _mm256_unpacklo_epi64(X,Y)
#define UPKH64(X,Y)     _mm256_unpackhi_epi64(X,Y)
#define SHUF(X,Y)       _mm256_shuffle_epi32(X,Y)
#define SHUFPD(X,Y,Z)   _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(X),_mm256_castsi256_pd(Y),Z))
#define PERM64(X,Y)     _mm256_permute4x64_epi64(X,Y)
#define PERM128(X,Y,Z)  _mm256_permute2x128_si256(X,Y,Z)
#define BLEND32(X,Y,Z)  _mm256_blend_epi32(X,Y,Z)
#define SET1_64(X)		_mm256_set1_epi64x(X)
/**
 * This construction calls broadcast instruction
 * explicitly specifying a memory location Y, which
 * could or could not be aligned.
 */
#define BROADCASTQ(X,Y)	__asm__ __volatile(\
"vpbroadcastq (%1), %0       ;"\
:/* out  */ "=x" (X)\
:/* in   */ "r" (Y)\
:/* regs */);

#endif /* _AVX2_H_ */



#define ALIGN_BYTES 32
#ifdef __INTEL_COMPILER
#define ALIGN __declspec(align(ALIGN_BYTES))
#else
#define ALIGN __attribute__ ((aligned (ALIGN_BYTES)))
#endif

#define ZeroOperandReturnKey(X) uint8_t * (*X)()
#define OneOperandGeneric(X) void (*X)(void* C)

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
typedef argElement_1w (*ZeroOperandReturn)(void);

typedef void (*ThreeOperandVector)(argElement_2w C, argElement_2w A, argElement_2w B);
typedef void (*TwoOperandVector)(argElement_2w C, argElement_2w A);
typedef int (*TwoOperandReturnVector)(argElement_2w C, argElement_2w A);
typedef void (*OneOperandVector)(argElement_2w C);
typedef argElement_2w (*ZeroOperandReturnVector)(void);

struct _struct_Fp_1way {
	ThreeOperand add,sub,mul;
	TwoOperand inv,sqrt;
	TwoOperandReturn cmp;
	OneOperand sqr,cred,rand,print;
	OneOperandGeneric(clear);
	ZeroOperandReturn init;
};

struct _struct_Fp_Nway {
	ThreeOperandVector add,sub,mul;
	TwoOperandReturnVector cmp;
	OneOperandVector sqr,cred,rand,print;
	OneOperandGeneric(clear);
	ZeroOperandReturnVector init;
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

