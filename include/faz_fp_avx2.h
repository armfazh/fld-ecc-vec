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

#ifndef FAZ_FP_AVX2_H
#define FAZ_FP_AVX2_H

#ifdef __cplusplus
namespace faz{
  namespace fp{
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <immintrin.h>

#define ALIGN_BYTES 32
#ifdef __INTEL_COMPILER
#define ALIGN __declspec(align(ALIGN_BYTES))
#else
#define ALIGN __attribute__ ((aligned (ALIGN_BYTES)))
#endif

#define argElement_1w uint64_t *
#define argElement_2w __m256i *
#define argElement_4w __m256i *
#define argElement_Nw __m256i *

/** Definitions for Fp, p = 2^255-19 */
#define SIZE_FP25519 32
#define BASE0_FP25519 26
#define BASE1_FP25519 25
#define NUM_DIGITS_FP25519 10
#define NUM_DIGITS_FP25519_X64 4

typedef ALIGN uint64_t EltFp25519_1w_fullradix[NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t EltFp25519_1w_fullradix_buffer[2 * NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t EltFp25519_2w_fullradix[2 * NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t EltFp25519_2w_fullradix_buffer[4 * NUM_DIGITS_FP25519_X64];
typedef ALIGN uint64_t EltFp25519_1w_redradix[4 * ((NUM_DIGITS_FP25519 + 3) / 4)];
typedef ALIGN __m256i EltFp25519_2w_redradix[(NUM_DIGITS_FP25519 / 2)];
typedef ALIGN __m256i EltFp25519_4w_redradix[NUM_DIGITS_FP25519];

/** Definitions for Fp, p = 2^448-2^224-1 */
#define SIZE_FP448 56
#define BASE_FP448 28
#define NUM_DIGITS_FP448 16
#define NUM_DIGITS_FP448_X64 7
typedef ALIGN uint64_t EltFp448_1w_fullradix[NUM_DIGITS_FP448_X64];
typedef ALIGN uint64_t EltFp448_1w_fullradix_buffer[2 * NUM_DIGITS_FP448_X64];
typedef ALIGN uint64_t EltFp448_1w_redradix[NUM_DIGITS_FP448];
typedef ALIGN __m256i EltFp448_2w_redradix[(NUM_DIGITS_FP448 / 2)];
typedef ALIGN __m256i EltFp448_4w_redradix[NUM_DIGITS_FP448];

#define Oper0Retr(NAME, TYPE) TYPE (*NAME)(void)
#define Oper1Void(NAME, TYPE) void (*NAME)(TYPE C)
#define Oper1File(NAME, TYPE) void (*NAME)(FILE* file, TYPE A)
#define Oper1ToBy(NAME, TYPE) void (*NAME)(uint8_t * C, TYPE A)
#define Oper1FrBy(NAME, TYPE) void (*NAME)(TYPE C, uint8_t * A)
#define Oper2Void(NAME, TYPE) void (*NAME)(TYPE C, TYPE A)
#define Oper2Retr(NAME, TYPE) int  (*NAME)(TYPE C, TYPE A)
#define Oper3Void(NAME, TYPE) void (*NAME)(TYPE C, TYPE A, TYPE B)
#define Oper3Vari(NAME, RET, TYPE0, TYPE1, TYPE2) RET (*NAME)(TYPE0 C, TYPE1 A, TYPE2 B)
#define Oper4Void(NAME, TYPE) void (*NAME)(TYPE C, TYPE D, TYPE A, TYPE B)

#define STRUCT_MISC(N,TYPE)  \
typedef struct               \
  _struct_misc ## N ## w {   \
    Oper0Retr(alloc, TYPE);  \
    Oper2Retr(cmp  , TYPE);  \
    Oper2Void(copy , TYPE);  \
    Oper1Void(free , void*); \
    Oper1File(print, TYPE);  \
    Oper1Void(rand , TYPE);  \
    Oper1ToBy(ser  , TYPE);  \
    Oper1FrBy(unser, TYPE);  \
    Oper1Void(zero , TYPE);  \
  } Misc_ ## N ## w

STRUCT_MISC(1,argElement_1w);
STRUCT_MISC(2,argElement_2w);
STRUCT_MISC(4,argElement_4w);

typedef struct _struct_1w {
  Oper3Void(add, argElement_1w);
  Oper2Void(inv, argElement_1w);
  Oper3Void(mul, argElement_1w);
  Oper1Void(neg, argElement_1w);
  Oper1Void(sqr, argElement_1w);
  Oper2Void(srt, argElement_1w);
  Oper3Void(sub, argElement_1w);
  Oper1Void(prime, argElement_1w);
  const Misc_1w misc;
} Arith_1w;

typedef struct _struct_arithex_1w_fullradix {
  Oper3Void(intmul,  argElement_1w);
  Oper3Void(intmul2, argElement_1w);
  Oper2Void(intsqr,  argElement_1w);
  Oper2Void(intsqr2, argElement_1w);
  Oper2Void(mula24,  argElement_1w);
  Oper2Void(reduce,  argElement_1w);
  Oper2Void(reduce2, argElement_1w);
} Arith_1w_fullradix;

typedef struct _struct_1w_fullradix {
  const Arith_1w arith;
  const Arith_1w_fullradix arithex;
} Fp_1w_fullradix;

typedef struct _struct_arithex_1w_redradix {
  Oper2Void(addsub,       argElement_1w);
  Oper3Void(intmul,       argElement_1w);
  Oper1Void(compress,     argElement_1w);
  Oper1Void(compressfast, argElement_1w);
} Arith_1w_redradix;

typedef struct _struct_1w_redradix {
  const Arith_1w arith;
  const Arith_1w_redradix arithex;
} Fp_1w_redradix;

typedef struct _struct_arithex_2w_redradix {
  Oper2Void(addsub,       argElement_2w);
  Oper3Vari(deinter,void,argElement_1w,argElement_1w,argElement_2w);
  Oper3Vari(inter  ,void,argElement_2w,argElement_1w,argElement_1w);
  Oper3Void(intmul,       argElement_2w);
  Oper1Void(intsqr,       argElement_2w);
  Oper1Void(compress,     argElement_2w);
  Oper2Void(compress2,    argElement_2w);
  Oper1Void(compressfast, argElement_2w);
} Arith_2w_redradix;

typedef struct _struct_2w {
  Oper3Void(add, argElement_2w);
  Oper3Void(mul, argElement_2w);
  Oper2Void(ngz, argElement_2w);
  Oper1Void(sqr, argElement_2w);
  Oper3Void(sub, argElement_2w);
  const Misc_2w misc;
} Arith_2w;

typedef struct _struct_2w_redradix {
  const Arith_2w arith;
  const Arith_2w_redradix arithex;
} Fp_2w_redradix;

typedef struct _struct_arithex_4w_redradix {
  Oper4Void(addsub,       argElement_4w);
  Oper2Void(addsublar,    argElement_4w);
  Oper3Void(intmul,       argElement_4w);
  Oper1Void(intsqr,       argElement_4w);
  Oper1Void(compress,     argElement_4w);
  Oper2Void(compress2,    argElement_4w);
  Oper1Void(compressfast, argElement_4w);
  Oper2Void(new_compressfast2, argElement_4w);
} Arith_4w_redradix;

typedef struct _struct_4w {
  Oper3Void(add, argElement_4w);
  Oper3Void(mul, argElement_4w);
  Oper2Void(ngz, argElement_4w);
  Oper1Void(sqr, argElement_4w);
  Oper3Void(sub, argElement_4w);
  const Misc_4w misc;
} Arith_4w;

typedef struct _struct_4w_redradix {
  const Arith_4w arith;
  const Arith_4w_redradix arithex;
} Fp_4w_redradix;

#undef Oper0Retr
#undef Oper1Void
#undef Oper1File
#undef Oper1ToBy
#undef Oper1FrBy
#undef Oper2Void
#undef Oper2Retr
#undef Oper3Void

typedef struct _struct_Fp_Arith {
  Fp_1w_fullradix _1w_full;
  Fp_1w_redradix _1w_red;
  Fp_2w_redradix _2w_red;
  Fp_4w_redradix _4w_red;
} PrimeField;

extern const PrimeField Fp25519, Fp448;

#ifdef __cplusplus
} /* extern C */
} /* namespace fp */
} /* namespace faz */
#endif /* __cplusplus */

#endif /* FAZ_FP_AVX2_H */
