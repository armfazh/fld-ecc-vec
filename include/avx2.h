#ifndef _AVX2_H_ /* _AVX2_H_ */
#define _AVX2_H_

#define HASWELL 0x80
#define	SKYLAKE 0x40

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
