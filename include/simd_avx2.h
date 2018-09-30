/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SIMD_AVX2_H
#define SIMD_AVX2_H

#include <immintrin.h>

/** AVX2 Definitions */
#define HASWELL 0x80
#define SKYLAKE 0x40

#define PROCESSOR SKYLAKE

#define ZEROUPPER         _mm256_zeroupper()
#define ZERO128           _mm_setzero_si128()
#define LOAD128(X)        _mm_load_si128((__m128i*)X)
#define STORE128(X,Y)     _mm_store_si128((__m128i*)X,Y)
#define ADD128(X,Y)       _mm_add_epi64(X,Y)
#define AND128(X,Y)       _mm_and_si128(X,Y)
#define MUL128(X,Y)       _mm_mul_epu32(X,Y)
#define SHL128(X,Y)       _mm_slli_epi64(X,Y)
#define SHR128(X,Y)       _mm_srli_epi64(X,Y)
#define SHR8N128(X,Y)     _mm_srli_si128(X,Y)
#define SET32(W,X,Y,Z)    _mm_set_epi32(W,X,Y,Z)
#define SHUF32128(X,Y)    _mm_shuffle_epi32(X,Y)
#define BLEND32128(X,Y,Z) _mm_blend_epi32(X,Y,Z)
#define SHLV128(X,Y)      _mm_sllv_epi64(X,Y)
#define SHRV128(X,Y)      _mm_srlv_epi64(X,Y)

#define UPKL64128(X, Y)  _mm_unpacklo_epi64(X,Y)
#define ZERO             _mm256_setzero_si256()
#define LOAD(X)          _mm256_load_si256((__m256i*) X)
#define STORE(X, Y)      _mm256_store_si256((__m256i*) X, Y)
//#define ALIGNR(X,Y)      _mm256_alignr_epi8(X,Y,8)
#define ALIGNR(X, Y)     _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(Y),_mm256_castsi256_pd(X),0x5))
#define ADD(X, Y)        _mm256_add_epi64(X,Y)
#define SUB(X, Y)        _mm256_sub_epi64(X,Y)
#define AND(X, Y)        _mm256_and_si256(X,Y)
#define ANDNOT(X, Y)     _mm256_andnot_si256(X,Y)
#define XOR(X, Y)        _mm256_xor_si256(X,Y)
#define OR(X, Y)         _mm256_or_si256(X,Y)
#define MUL(X, Y)        _mm256_mul_epu32(X,Y)
#define SHR(X, Y)        _mm256_srli_epi64(X,Y)
#define SHL(X, Y)        _mm256_slli_epi64(X,Y)
#define SHLV(X, Y)       _mm256_sllv_epi64(X,Y)
#define SHRV(X, Y)       _mm256_srlv_epi64(X,Y)
#define SHL8(X)          _mm256_slli_si256(X,8)
#define SHR8(X)          _mm256_srli_si256(X,8)
#define SHR8N(X, Y)      _mm256_srli_si256(X,Y)
#define CLEAN_LOW64(X)   _mm256_blend_epi32(X,ZERO,0x33)
#define CLEAN_HIGH64(X)  _mm256_blend_epi32(X,ZERO,0xCC)
#define UPKL64(X, Y)     _mm256_unpacklo_epi64(X,Y)
#define UPKH64(X, Y)     _mm256_unpackhi_epi64(X,Y)
#define SHUF32(X, Y)     _mm256_shuffle_epi32(X,Y)
#define SHUFPD(X, Y, Z)  _mm256_castpd_si256(_mm256_shuffle_pd(_mm256_castsi256_pd(X),_mm256_castsi256_pd(Y),Z))
#define PERM64(X, Y)     _mm256_permute4x64_epi64(X,Y)
#define PERM128(X, Y, Z) _mm256_permute2x128_si256(X,Y,Z)
//#define PERM128(X, Y, Z) _mm256_permute2f128_si256(X,Y,Z)
#define PERMV128(X, Y)   _mm256_permutevar8x32_epi32(X,Y)
#define BLEND32(X, Y, Z) _mm256_blend_epi32(X,Y,Z)
#define SET164(X)        _mm256_set1_epi64x(X)
#define SET64(W,X,Y,Z)   _mm256_set_epi64x(W,X,Y,Z)
#define EXTR(X,Y)        _mm256_extracti128_si256(X,Y)
#define INSR(X,Y,Z)      _mm256_inserti128_si256(X,Y,Z)
//#define INSR(X,Y,Z)        _mm256_insertf128_si256(X,Y,Z)
#define CAST256TO128(X)  _mm256_castsi256_si128(X)
#define CAST128TO256(X)  _mm256_castsi128_si256(X)
/**
 * This construction calls broadcast instruction
 * explicitly specifying a memory location Y, which
 * could or could not be aligned.
 */
#define BROADCASTQ(X, Y)     \
  __asm__ __volatile(        \
    "vpbroadcastq (%1), %0;" \
  :/* out  */ "=x" (X)       \
  :/* in   */ "r" (Y)        \
  :/* regs */);

#endif /* SIMD_AVX2_H */
