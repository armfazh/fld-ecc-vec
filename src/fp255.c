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

#if defined(FP255)

#include "faz_fp_avx2.h"
#include "simd_avx2.h"

#ifdef __cplusplus
namespace faz
{
namespace fp255
{
extern "C" {
#endif /* __cplusplus */

#define SUFFIX(NAME, OPER, IMPL) NAME##_Fp##255##_##OPER##w_##IMPL

#include "eltfp25519_1w_fullradix.c"
#include "eltfp25519_1w_redradix.c"
#include "eltfp25519_2w_redradix.c"
#include "eltfp25519_4w_redradix.c"
#if defined(ENABLED_AVX512)
#include "eltfp25519_2w_redradix_x2.c"
#endif /* defined(ENABLED_AVX512) */

const PrimeField Fp25519 = {
    ._1w_full = {
        .arith ={
            .add = SUFFIX(add, 1, fullradix),
            .cmp = SUFFIX(cmp, 1, fullradix),
            .cmv = SUFFIX(cmv, 1, fullradix),
            .inv = SUFFIX(inv, 1, fullradix),
            .mul = SUFFIX(mul, 1, fullradix),
            .neg = SUFFIX(neg, 1, fullradix),
            .sgn = SUFFIX(sgn, 1, fullradix),
            .sqr = SUFFIX(sqr, 1, fullradix),
            .srt = SUFFIX(srt, 1, fullradix),
            .sub = SUFFIX(sub, 1, fullradix),
            .prime = SUFFIX(prime,  1, fullradix),
            .misc = {
                .alloc = SUFFIX(alloc,  1, fullradix),
                .copy  = SUFFIX(copy,  1, fullradix),
                .free  = deallocate_bytes,
                .print = SUFFIX(print, 1, fullradix),
                .rand  = SUFFIX(rand, 1, fullradix),
                .ser   = SUFFIX(ser, 1, fullradix),
                .unser = SUFFIX(unser, 1, fullradix),
                .zero  = SUFFIX(zero, 1, fullradix),
            },
        },
        .arithex = {
            .intmul  = SUFFIX(intmul,  1, fullradix),
            .intmul2 = SUFFIX(intmul2, 1, fullradix),
            .intsqr  = SUFFIX(intsqr,  1, fullradix),
            .intsqr2 = SUFFIX(intsqr2, 1, fullradix),
            .mula24  = SUFFIX(mula24,  1, fullradix),
            .reduce  = SUFFIX(reduce,  1, fullradix),
            .reduce2 = SUFFIX(reduce2, 1, fullradix),
        },
    },
    ._1w_red = {
        .arith = {
            .add = SUFFIX(add, 1, redradix),
            .cmp = SUFFIX(cmp, 1, redradix),
            .cmv = SUFFIX(cmv, 1, redradix),
            .inv = SUFFIX(inv, 1, redradix),
            .mul = SUFFIX(mul, 1, redradix),
            .neg = SUFFIX(neg, 1, redradix),
            .sgn = SUFFIX(sgn, 1, redradix),
            .sqr = SUFFIX(sqr, 1, redradix),
            .srt = SUFFIX(srt, 1, redradix),
            .sub = SUFFIX(sub, 1, redradix),
            .prime = SUFFIX(prime, 1, redradix),
            .misc = {
                .alloc = SUFFIX(alloc,  1, redradix),
                .copy  = SUFFIX(copy,  1, redradix),
                .free  = deallocate_bytes,
                .print = SUFFIX(print, 1, redradix),
                .rand  = SUFFIX(rand, 1, redradix),
                .ser   = SUFFIX(ser, 1, redradix),
                .unser = SUFFIX(unser, 1, redradix),
                .zero  = SUFFIX(zero, 1, redradix),
            },
        },
        .arithex = {
            .addsub       = SUFFIX(addsub,  1, redradix),
            .intmul       = SUFFIX(intmul,  1, redradix),
            .compress     = SUFFIX(compress,  1, redradix),
            .compressfast = SUFFIX(compressfast,  1, redradix),
        },
    },
    ._2w_red = {
        .arith = {
            .add = SUFFIX(add,2,redradix),
            .cmp = SUFFIX(cmp,2,redradix),
            .cmv = SUFFIX(cmv,2,redradix),
            .mul = SUFFIX(mul,2,redradix),
            .neg = SUFFIX(neg,2,redradix),
            .ngz = SUFFIX(ngz,2,redradix),
            .sgn = SUFFIX(sgn,2,redradix),
            .sqr = SUFFIX(sqr,2,redradix),
            .sub = SUFFIX(sub,2,redradix),
            .misc = {
                .alloc = SUFFIX(alloc,  2, redradix),
                .copy  = SUFFIX(copy,  2, redradix),
                .free  = deallocate_bytes,
                .print = SUFFIX(print, 2, redradix),
                .rand  = SUFFIX(rand, 2, redradix),
                .ser   = SUFFIX(ser, 2, redradix),
                .unser = SUFFIX(unser, 2, redradix),
                .zero  = SUFFIX(zero, 2, redradix),
            },
        },
        .arithex = {
            .addsub    = SUFFIX(addsub,2,redradix),
            .deinter   = SUFFIX(deinter,2,redradix),
            .inter     = SUFFIX(inter,2,redradix),
            .intmul    = SUFFIX(intmul,2,redradix),
            .intsqr    = SUFFIX(intsqr,2,redradix),
            .compress  = SUFFIX(compress,2,redradix),
            .compress2 = SUFFIX(compress2,2,redradix),
            .compressfast = SUFFIX(compressfast,2,redradix),
        },
    },
#if defined(ENABLED_AVX512)
    ._2w_red_x2 = {
        .arith = {
            .add = SUFFIX(add,2,redradix_x2),
            .cmp = SUFFIX(cmp,2,redradix_x2),
            .cmv = SUFFIX(cmv,2,redradix_x2),
            .mul = SUFFIX(mul,2,redradix_x2),
            .neg = SUFFIX(neg,2,redradix_x2),
            .ngz = SUFFIX(ngz,2,redradix_x2),
            .sgn = SUFFIX(sgn,2,redradix_x2),
            .sqr = SUFFIX(sqr,2,redradix_x2),
            .sub = SUFFIX(sub,2,redradix_x2),
            .misc = {
                .alloc = SUFFIX(alloc,  2, redradix_x2),
                .copy  = SUFFIX(copy,  2, redradix_x2),
                .free  = deallocate_bytes,
                .print = SUFFIX(print, 2, redradix_x2),
                .rand  = SUFFIX(rand, 2, redradix_x2),
                .ser   = SUFFIX(ser, 2, redradix_x2),
                .unser = SUFFIX(unser, 2, redradix_x2),
                .zero  = SUFFIX(zero, 2, redradix_x2),
            },
        },
        .arithex = {
            .addsub    = SUFFIX(addsub,2,redradix_x2),
            .deinter   = SUFFIX(deinter,2,redradix_x2),
            .inter     = SUFFIX(inter,2,redradix_x2),
            .intmul    = SUFFIX(intmul,2,redradix_x2),
            .intsqr    = SUFFIX(intsqr,2,redradix_x2),
            .compress  = SUFFIX(compress,2,redradix_x2),
            .compress2 = SUFFIX(compress2,2,redradix_x2),
            .compressfast = SUFFIX(compressfast,2,redradix_x2),
        },
    },
#endif /* defined(ENABLED_AVX512) */
    ._4w_red = {
        .arith = {
            .add = SUFFIX(add,4,redradix),
            .cmp = SUFFIX(cmp,4,redradix),
            .mul = SUFFIX(mul,4,redradix),
            .neg = SUFFIX(neg,2,redradix),
            .ngz = SUFFIX(ngz,4,redradix),
            .sqr = SUFFIX(sqr,4,redradix),
            .srt = SUFFIX(srt,4,redradix),
            .sub = SUFFIX(sub,4,redradix),
            .misc = {
                .alloc = SUFFIX(alloc,  4, redradix),
                .copy  = SUFFIX(copy,  4, redradix),
                .free  = deallocate_bytes,
                .print = SUFFIX(print,  4, redradix),
                .rand  = SUFFIX(rand,  4, redradix),
                .ser   = SUFFIX(ser,  4, redradix),
                .unser = SUFFIX(unser,  4, redradix),
                .zero  = SUFFIX(zero,  4, redradix),
            },
        },
        .arithex = {
            .addsub    = SUFFIX(addsub,4,redradix),
            .addsublar = SUFFIX(addsublar,4,redradix),
            .intmul = SUFFIX(intmul,4,redradix),
            .intsqr = SUFFIX(intsqr,4,redradix),
            .compress     = SUFFIX(compress,4,redradix),
            .compress2    = SUFFIX(compress2,4,redradix),
            .compressfast = SUFFIX(compressfast,4,redradix),
            .new_compressfast2 = SUFFIX(new_compressfast2,4,redradix),
        },
    },
};

#undef SUFFIX
#undef INTERNAL

#ifdef __cplusplus
} /* extern C */
} /* namespace fp255 */
} /* namespace faz */
#endif /* __cplusplus */

#else
extern int version;
#endif /* FP255 */
