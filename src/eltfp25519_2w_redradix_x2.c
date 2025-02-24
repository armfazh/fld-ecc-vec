/**
 * Copyright 2025 Armando Faz Hernández
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

#if defined(FP255) && defined(ENABLED_AVX512)

#define FN(NAME) SUFFIX(NAME, 2, redradix_x2)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

DECL(void, copy)(argElement_2w_x2 c, argElement_2w_x2 a)
{
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = a[i];
    }
}

DECL(void, zero)(argElement_2w_x2 c)
{
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = ZERO_x2;
    }
}

/**
 *
 * @param a
 * @param a0
 * @param a1
 */
DECL(void, inter)(argElement_2w_x2 a, argElement_1w a0[2], argElement_1w a1[2])
{
    __m256i A0 = LOAD(a0[0] + 0);
    __m256i A4 = LOAD(a0[0] + 1);
    __m256i A8 = LOAD(a0[0] + 2);
    __m256i B0 = LOAD(a1[0] + 0);
    __m256i B4 = LOAD(a1[0] + 1);
    __m256i B8 = LOAD(a1[0] + 2);

    __m256i a0_0 = PERM128(A0, B0, 0x20);
    __m256i a1_0 = PERM128(A0, B0, 0x31);
    __m256i a2_0 = PERM128(A4, B4, 0x20);
    __m256i a3_0 = PERM128(A4, B4, 0x31);
    __m256i a4_0 = PERM128(A8, B8, 0x20);

    A0 = LOAD(a0[1] + 0);
    A4 = LOAD(a0[1] + 1);
    A8 = LOAD(a0[1] + 2);
    B0 = LOAD(a1[1] + 0);
    B4 = LOAD(a1[1] + 1);
    B8 = LOAD(a1[1] + 2);

    __m256i a0_1 = PERM128(A0, B0, 0x20);
    __m256i a1_1 = PERM128(A0, B0, 0x31);
    __m256i a2_1 = PERM128(A4, B4, 0x20);
    __m256i a3_1 = PERM128(A4, B4, 0x31);
    __m256i a4_1 = PERM128(A8, B8, 0x20);

    a[0] = INSR_x2(CAST256TO512(a0_0), a0_1, 1);
    a[1] = INSR_x2(CAST256TO512(a1_0), a1_1, 1);
    a[2] = INSR_x2(CAST256TO512(a2_0), a2_1, 1);
    a[3] = INSR_x2(CAST256TO512(a3_0), a3_1, 1);
    a[4] = INSR_x2(CAST256TO512(a4_0), a4_1, 1);
}

/**
 *
 * @param a0
 * @param a1
 * @param a
 */
DECL(void, deinter)(argElement_1w a0[2], argElement_1w a1[2], argElement_2w_x2 a)
{
    __m256i a0_0 = CAST512TO256(a[0]);
    __m256i a1_0 = CAST512TO256(a[1]);
    __m256i a2_0 = CAST512TO256(a[2]);
    __m256i a3_0 = CAST512TO256(a[3]);
    __m256i a4_0 = CAST512TO256(a[4]);

    STORE(a0[0] + 0, PERM128(a0_0, a1_0, 0x20));
    STORE(a1[0] + 0, PERM128(a0_0, a1_0, 0x31));
    STORE(a0[0] + 1, PERM128(a2_0, a3_0, 0x20));
    STORE(a1[0] + 1, PERM128(a2_0, a3_0, 0x31));
    STORE(a0[0] + 2, a4_0);
    STORE(a1[0] + 2, CAST128TO256(EXTR(a4_0, 1)));

    __m256i a0_1 = EXTR_x2(a[0], 1);
    __m256i a1_1 = EXTR_x2(a[1], 1);
    __m256i a2_1 = EXTR_x2(a[2], 1);
    __m256i a3_1 = EXTR_x2(a[3], 1);
    __m256i a4_1 = EXTR_x2(a[4], 1);

    STORE(a0[1] + 0, PERM128(a0_1, a1_1, 0x20));
    STORE(a1[1] + 0, PERM128(a0_1, a1_1, 0x31));
    STORE(a0[1] + 1, PERM128(a2_1, a3_1, 0x20));
    STORE(a1[1] + 1, PERM128(a2_1, a3_1, 0x31));
    STORE(a0[1] + 2, a4_1);
    STORE(a1[1] + 2, CAST128TO256(EXTR(a4_1, 1)));
}

DECL(void, cmv)(__m512i bit, argElement_2w_x2 c, argElement_2w_x2 a, argElement_2w_x2 b)
{
    int i = 0;
    const __m512i ONE = SET64_x2(0, 1, 0, 1, 0, 1, 0, 1);
    const __m512i mask = SHUF32_x2(SUB_x2(ZERO_x2, AND_x2(bit, ONE)), 0x44);
    const __mmask8 mask8 = _mm512_cmp_epi64_mask(mask, ZERO_x2, _MM_CMPINT_EQ);
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = _mm512_mask_blend_epi64(mask8, a[i], b[i]);
    }
}

/**
 *
 * @param c
 * @param a
 * @param b
 */
DECL(void, add)(argElement_2w_x2 c, argElement_2w_x2 a, argElement_2w_x2 b)
{
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = ADD_x2(a[i], b[i]);
    }
}

static const ALIGN uint64_t CONST_2P_2P_H0H5_x2[4 * NUM_DIGITS_FP25519] = {
    0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe
};

static const ALIGN uint64_t CONST_2P_00_H0H5_x2[4 * NUM_DIGITS_FP25519] = {
    0x7ffffda, 0x3fffffe, 0x0000000, 0x0000000, 0x7ffffda, 0x3fffffe, 0x0000000, 0x0000000,
    0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000,
    0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000, 0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000,
    0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000,
    0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000, 0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000
};

/**
 *
 * @param c
 * @param a
 * @param b
 */
DECL(void, sub)(argElement_2w_x2 c, argElement_2w_x2 a, argElement_2w_x2 b)
{
    argElement_2w_x2 _2P = (argElement_2w_x2)CONST_2P_2P_H0H5_x2;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = ADD_x2(a[i], SUB_x2(_2P[i], b[i]));
    }
}

/**
 * Given
 * 		A = [X | Y]
 * Cumputes:
 * 	if permutation == 0
 * 		C = [X|Y] + [2p|0] + [-Y|X]
 * 		C = [ X+2p-Y | Y+X ]
 * 	if permutation == 1
 * 		C = [Y|X] + [2p|0] + [-X|Y]
 * 		C = [ Y+2p-X | Y+X ]
 * 	if permutation != {0,1}
 * 		undefined
 * @param c
 * @param a
 * @param permutation
 */
DECL(void, subadd)(argElement_2w_x2 c, argElement_2w_x2 a, const int permutation)
{
    const __m512i mask_subadd = SET64_x2(0, 0, -1, -1, 0, 0, -1, -1);
    argElement_2w_x2 _2P_00 = (argElement_2w_x2)CONST_2P_00_H0H5_x2;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        __m512i M = ZERO_x2, N = ZERO_x2, PA;
        PA = PERM64_x2(a[i], 0x4E);
        if (permutation == 0) {
            M = a[i];
            N = PA;
        } else if (permutation == 1) {
            N = a[i];
            M = PA;
        }
        c[i] = ADD_x2(ADD_x2(M, _2P_00[i]), SUB_x2(XOR_x2(N, mask_subadd), mask_subadd));
    }
}

/**
 * Given
 * 		A = [X |Y ]
 * Returns
 * 		C = [2P-X | Y]
 * @param c
 * @param a
 */
DECL(void, ngz)(argElement_2w_x2 c, argElement_2w_x2 a)
{
    const __m512i mask_subadd = SET64_x2(0, 0, -1, -1, 0, 0, -1, -1);
    argElement_2w_x2 _2P_00 = (argElement_2w_x2)CONST_2P_00_H0H5_x2;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = ADD_x2(_2P_00[i], SUB_x2(XOR_x2(a[i], mask_subadd), mask_subadd));
    }
}

DECL(void, addsub)(argElement_2w_x2 a, argElement_2w_x2 b)
{
    argElement_2w_x2 _2P = (argElement_2w_x2)CONST_2P_2P_H0H5_x2;
    __m512i d, c;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        d = ADD_x2(a[i], b[i]);
        c = ADD_x2(a[i], SUB_x2(_2P[i], b[i]));
        a[i] = d;
        b[i] = c;
    }
}

static const ALIGN uint64_t GLOB_times_19_x2[8] = {19, 19, 19, 19, 19, 19, 19, 19};
static const ALIGN uint64_t GLOB_sh_0_x2[8] = {1, 0, 1, 0, 1, 0, 1, 0};
static const ALIGN uint64_t GLOB_sh_1_x2[8] = {0, 1, 0, 1, 0, 1, 0, 1};

void FN(intmul2)(
    argElement_2w_x2 d, argElement_2w_x2 c,
    argElement_2w_x2 a, argElement_2w_x2 b)
{
    argElement_2w_x2 const times_19 = (argElement_2w_x2)GLOB_times_19_x2;
    argElement_2w_x2 const sh_0 = (argElement_2w_x2)GLOB_sh_0_x2;
    argElement_2w_x2 const sh_1 = (argElement_2w_x2)GLOB_sh_1_x2;

    __m512i ai, aj, ak, al;
    __m512i b0, b1, b2, b3, b4;
    __m512i d0, d1, d2, d3, d4;
    __m512i e0, e1, e2, e3, e4;
    __m512i c0, c1, c2, c3, c4;
    __m512i p0, p1, p2, p3, p4;
    __m512i k0, k1, k2, k3, k4;
    __m512i m0, m1, m2, m3, m4;

    b0 = b[0];
    b1 = b[1];
    b2 = b[2];
    b3 = b[3];
    b4 = b[4];

    e0 = MUL_x2(b0, *times_19);
    d0 = ALIGNR_x2(b0, e0);
    e1 = MUL_x2(b1, *times_19);
    d1 = ALIGNR_x2(b1, e1);
    e2 = MUL_x2(b2, *times_19);
    d2 = ALIGNR_x2(b2, e2);
    e3 = MUL_x2(b3, *times_19);
    d3 = ALIGNR_x2(b3, e3);
    e4 = MUL_x2(b4, *times_19);
    d4 = ALIGNR_x2(b4, e4);

    ai = SHUF32_x2(a[0], 0x44);
    ak = PERM64_x2(a[0], 0x0A);
    c0 = MUL_x2(b0, ai);
    p0 = MUL_x2(b0, ak);
    c1 = MUL_x2(b1, ai);
    p1 = MUL_x2(b1, ak);
    c2 = MUL_x2(b2, ai);
    p2 = MUL_x2(b2, ak);
    c3 = MUL_x2(b3, ai);
    p3 = MUL_x2(b3, ak);
    c4 = MUL_x2(b4, ai);
    p4 = MUL_x2(b4, ak);

    ai = SHUF32_x2(a[2], 0x44);
    ak = PERM64_x2(a[2], 0x0A);
    c0 = ADD_x2(c0, MUL_x2(d3, ai));
    p0 = ADD_x2(p0, MUL_x2(d3, ak));
    c1 = ADD_x2(c1, MUL_x2(d4, ai));
    p1 = ADD_x2(p1, MUL_x2(d4, ak));
    c2 = ADD_x2(c2, MUL_x2(b0, ai));
    p2 = ADD_x2(p2, MUL_x2(b0, ak));
    c3 = ADD_x2(c3, MUL_x2(b1, ai));
    p3 = ADD_x2(p3, MUL_x2(b1, ak));
    c4 = ADD_x2(c4, MUL_x2(b2, ai));
    p4 = ADD_x2(p4, MUL_x2(b2, ak));

    ai = SHUF32_x2(a[4], 0x44);
    ak = PERM64_x2(a[4], 0x0A);
    c0 = ADD_x2(c0, MUL_x2(d1, ai));
    p0 = ADD_x2(p0, MUL_x2(d1, ak));
    c1 = ADD_x2(c1, MUL_x2(d2, ai));
    p1 = ADD_x2(p1, MUL_x2(d2, ak));
    c2 = ADD_x2(c2, MUL_x2(d3, ai));
    p2 = ADD_x2(p2, MUL_x2(d3, ak));
    c3 = ADD_x2(c3, MUL_x2(d4, ai));
    p3 = ADD_x2(p3, MUL_x2(d4, ak));
    c4 = ADD_x2(c4, MUL_x2(b0, ai));
    p4 = ADD_x2(p4, MUL_x2(b0, ak));

    ai = SHUF32_x2(a[1], 0xEE);
    ak = PERM64_x2(a[1], 0x5F);
    c0 = ADD_x2(c0, MUL_x2(e4, ai));
    p0 = ADD_x2(p0, MUL_x2(e4, ak));
    c1 = ADD_x2(c1, MUL_x2(d0, ai));
    p1 = ADD_x2(p1, MUL_x2(d0, ak));
    c2 = ADD_x2(c2, MUL_x2(d1, ai));
    p2 = ADD_x2(p2, MUL_x2(d1, ak));
    c3 = ADD_x2(c3, MUL_x2(d2, ai));
    p3 = ADD_x2(p3, MUL_x2(d2, ak));
    c4 = ADD_x2(c4, MUL_x2(d3, ai));
    p4 = ADD_x2(p4, MUL_x2(d3, ak));

    ai = SHUF32_x2(a[3], 0xEE);
    ak = PERM64_x2(a[3], 0x5F);
    c0 = ADD_x2(c0, MUL_x2(e2, ai));
    p0 = ADD_x2(p0, MUL_x2(e2, ak));
    c1 = ADD_x2(c1, MUL_x2(e3, ai));
    p1 = ADD_x2(p1, MUL_x2(e3, ak));
    c2 = ADD_x2(c2, MUL_x2(e4, ai));
    p2 = ADD_x2(p2, MUL_x2(e4, ak));
    c3 = ADD_x2(c3, MUL_x2(d0, ai));
    p3 = ADD_x2(p3, MUL_x2(d0, ak));
    c4 = ADD_x2(c4, MUL_x2(d1, ai));
    p4 = ADD_x2(p4, MUL_x2(d1, ak));

    d[0] = c0;
    c[0] = p0;
    d[1] = c1;
    c[1] = p1;
    d[2] = c2;
    c[2] = p2;
    d[3] = c3;
    c[3] = p3;
    d[4] = c4;
    c[4] = p4;

    aj = SHUF32_x2(a[1], 0x44);
    al = PERM64_x2(a[1], 0x0A);
    k0 = MUL_x2(d4, aj);
    m0 = MUL_x2(d4, al);
    k1 = MUL_x2(b0, aj);
    m1 = MUL_x2(b0, al);
    k2 = MUL_x2(b1, aj);
    m2 = MUL_x2(b1, al);
    k3 = MUL_x2(b2, aj);
    m3 = MUL_x2(b2, al);
    k4 = MUL_x2(b3, aj);
    m4 = MUL_x2(b3, al);

    aj = SHUF32_x2(a[3], 0x44);
    al = PERM64_x2(a[3], 0x0A);
    k0 = ADD_x2(k0, MUL_x2(d2, aj));
    m0 = ADD_x2(m0, MUL_x2(d2, al));
    k1 = ADD_x2(k1, MUL_x2(d3, aj));
    m1 = ADD_x2(m1, MUL_x2(d3, al));
    k2 = ADD_x2(k2, MUL_x2(d4, aj));
    m2 = ADD_x2(m2, MUL_x2(d4, al));
    k3 = ADD_x2(k3, MUL_x2(b0, aj));
    m3 = ADD_x2(m3, MUL_x2(b0, al));
    k4 = ADD_x2(k4, MUL_x2(b1, aj));
    m4 = ADD_x2(m4, MUL_x2(b1, al));

    aj = SHUF32_x2(a[0], 0xEE);
    al = PERM64_x2(a[0], 0x5F);
    k0 = ADD_x2(k0, MUL_x2(d0, aj));
    m0 = ADD_x2(m0, MUL_x2(d0, al));
    k1 = ADD_x2(k1, MUL_x2(d1, aj));
    m1 = ADD_x2(m1, MUL_x2(d1, al));
    k2 = ADD_x2(k2, MUL_x2(d2, aj));
    m2 = ADD_x2(m2, MUL_x2(d2, al));
    k3 = ADD_x2(k3, MUL_x2(d3, aj));
    m3 = ADD_x2(m3, MUL_x2(d3, al));
    k4 = ADD_x2(k4, MUL_x2(d4, aj));
    m4 = ADD_x2(m4, MUL_x2(d4, al));

    aj = SHUF32_x2(a[2], 0xEE);
    al = PERM64_x2(a[2], 0x5F);
    k0 = ADD_x2(k0, MUL_x2(e3, aj));
    m0 = ADD_x2(m0, MUL_x2(e3, al));
    k1 = ADD_x2(k1, MUL_x2(e4, aj));
    m1 = ADD_x2(m1, MUL_x2(e4, al));
    k2 = ADD_x2(k2, MUL_x2(d0, aj));
    m2 = ADD_x2(m2, MUL_x2(d0, al));
    k3 = ADD_x2(k3, MUL_x2(d1, aj));
    m3 = ADD_x2(m3, MUL_x2(d1, al));
    k4 = ADD_x2(k4, MUL_x2(d2, aj));
    m4 = ADD_x2(m4, MUL_x2(d2, al));

    aj = SHUF32_x2(a[4], 0xEE);
    al = PERM64_x2(a[4], 0x5F);
    k0 = ADD_x2(k0, MUL_x2(e1, aj));
    m0 = ADD_x2(m0, MUL_x2(e1, al));
    k1 = ADD_x2(k1, MUL_x2(e2, aj));
    m1 = ADD_x2(m1, MUL_x2(e2, al));
    k2 = ADD_x2(k2, MUL_x2(e3, aj));
    m2 = ADD_x2(m2, MUL_x2(e3, al));
    k3 = ADD_x2(k3, MUL_x2(e4, aj));
    m3 = ADD_x2(m3, MUL_x2(e4, al));
    k4 = ADD_x2(k4, MUL_x2(d0, aj));
    m4 = ADD_x2(m4, MUL_x2(d0, al));

    d[0] = ADD_x2(d[0], SHLV_x2(k0, *sh_0));
    c[0] = ADD_x2(c[0], SHLV_x2(m0, *sh_0));
    d[1] = ADD_x2(d[1], SHLV_x2(k1, *sh_1));
    c[1] = ADD_x2(c[1], SHLV_x2(m1, *sh_1));
    d[2] = ADD_x2(d[2], SHLV_x2(k2, *sh_0));
    c[2] = ADD_x2(c[2], SHLV_x2(m2, *sh_0));
    d[3] = ADD_x2(d[3], SHLV_x2(k3, *sh_1));
    c[3] = ADD_x2(c[3], SHLV_x2(m3, *sh_1));
    d[4] = ADD_x2(d[4], SHLV_x2(k4, *sh_0));
    c[4] = ADD_x2(c[4], SHLV_x2(m4, *sh_0));
}

DECL(void, intmul)(argElement_2w_x2 c, argElement_2w_x2 a, argElement_2w_x2 b)
{
    argElement_2w_x2 const times_19 = (argElement_2w_x2)GLOB_times_19_x2;
    argElement_2w_x2 const sh_0 = (argElement_2w_x2)GLOB_sh_0_x2;
    argElement_2w_x2 const sh_1 = (argElement_2w_x2)GLOB_sh_1_x2;

    __m512i ai, aj;
    __m512i b0, b1, b2, b3, b4;
    __m512i d0, d1, d2, d3, d4;
    __m512i e0, e1, e2, e3, e4;
    __m512i c0, c1, c2, c3, c4;
    __m512i k0, k1, k2, k3, k4;
    __m512i t0, t1, t2, t3, t4;

    b0 = b[0];
    b1 = b[1];
    b2 = b[2];
    b3 = b[3];
    b4 = b[4];

    e4 = MUL_x2(b4, *times_19);
    d4 = ALIGNR_x2(b4, e4);
    e3 = MUL_x2(b3, *times_19);
    d3 = ALIGNR_x2(b3, e3);
    e2 = MUL_x2(b2, *times_19);
    d2 = ALIGNR_x2(b2, e2);
    e1 = MUL_x2(b1, *times_19);
    d1 = ALIGNR_x2(b1, e1);
    e0 = MUL_x2(b0, *times_19);
    d0 = ALIGNR_x2(b0, e0);

    ai = SHUF32_x2(a[0], 0x44);
    aj = SHUF32_x2(a[1], 0x44);
    c0 = MUL_x2(b0, ai);
    k0 = MUL_x2(d4, aj);
    c1 = MUL_x2(b1, ai);
    k1 = MUL_x2(b0, aj);
    c2 = MUL_x2(b2, ai);
    k2 = MUL_x2(b1, aj);
    c3 = MUL_x2(b3, ai);
    k3 = MUL_x2(b2, aj);
    c4 = MUL_x2(b4, ai);
    k4 = MUL_x2(b3, aj);

    ai = SHUF32_x2(a[2], 0x44);
    aj = SHUF32_x2(a[3], 0x44);
    c0 = ADD_x2(c0, MUL_x2(d3, ai));
    k0 = ADD_x2(k0, MUL_x2(d2, aj));
    c1 = ADD_x2(c1, MUL_x2(d4, ai));
    k1 = ADD_x2(k1, MUL_x2(d3, aj));
    c2 = ADD_x2(c2, MUL_x2(b0, ai));
    k2 = ADD_x2(k2, MUL_x2(d4, aj));
    c3 = ADD_x2(c3, MUL_x2(b1, ai));
    k3 = ADD_x2(k3, MUL_x2(b0, aj));
    c4 = ADD_x2(c4, MUL_x2(b2, ai));
    k4 = ADD_x2(k4, MUL_x2(b1, aj));

    ai = SHUF32_x2(a[4], 0x44);
    aj = SHUF32_x2(a[0], 0xEE);
    c0 = ADD_x2(c0, MUL_x2(d1, ai));
    k0 = ADD_x2(k0, MUL_x2(d0, aj));
    c1 = ADD_x2(c1, MUL_x2(d2, ai));
    k1 = ADD_x2(k1, MUL_x2(d1, aj));
    c2 = ADD_x2(c2, MUL_x2(d3, ai));
    k2 = ADD_x2(k2, MUL_x2(d2, aj));
    c3 = ADD_x2(c3, MUL_x2(d4, ai));
    k3 = ADD_x2(k3, MUL_x2(d3, aj));
    c4 = ADD_x2(c4, MUL_x2(b0, ai));
    k4 = ADD_x2(k4, MUL_x2(d4, aj));

    ai = SHUF32_x2(a[1], 0xEE);
    aj = SHUF32_x2(a[2], 0xEE);
    c0 = ADD_x2(c0, MUL_x2(e4, ai));
    k0 = ADD_x2(k0, MUL_x2(e3, aj));
    c1 = ADD_x2(c1, MUL_x2(d0, ai));
    k1 = ADD_x2(k1, MUL_x2(e4, aj));
    c2 = ADD_x2(c2, MUL_x2(d1, ai));
    k2 = ADD_x2(k2, MUL_x2(d0, aj));
    c3 = ADD_x2(c3, MUL_x2(d2, ai));
    k3 = ADD_x2(k3, MUL_x2(d1, aj));
    c4 = ADD_x2(c4, MUL_x2(d3, ai));
    k4 = ADD_x2(k4, MUL_x2(d2, aj));

    ai = SHUF32_x2(a[3], 0xEE);
    aj = SHUF32_x2(a[4], 0xEE);
    c0 = ADD_x2(c0, MUL_x2(e2, ai));
    k0 = ADD_x2(k0, MUL_x2(e1, aj));
    c1 = ADD_x2(c1, MUL_x2(e3, ai));
    k1 = ADD_x2(k1, MUL_x2(e2, aj));
    c2 = ADD_x2(c2, MUL_x2(e4, ai));
    k2 = ADD_x2(k2, MUL_x2(e3, aj));
    c3 = ADD_x2(c3, MUL_x2(d0, ai));
    k3 = ADD_x2(k3, MUL_x2(e4, aj));
    c4 = ADD_x2(c4, MUL_x2(d1, ai));
    k4 = ADD_x2(k4, MUL_x2(d0, aj));

    t0 = SHLV_x2(k0, *sh_0);
    t1 = SHLV_x2(k1, *sh_1);
    t2 = SHLV_x2(k2, *sh_0);
    t3 = SHLV_x2(k3, *sh_1);
    t4 = SHLV_x2(k4, *sh_0);

    c[0] = ADD_x2(c0, t0);
    c[1] = ADD_x2(c1, t1);
    c[2] = ADD_x2(c2, t2);
    c[3] = ADD_x2(c3, t3);
    c[4] = ADD_x2(c4, t4);
}

DECL(void, intsqr)(argElement_2w_x2 c)
{
    argElement_2w_x2 const times_19 = (argElement_2w_x2)GLOB_times_19_x2;
    argElement_2w_x2 const sh_0 = (argElement_2w_x2)GLOB_sh_0_x2;
    argElement_2w_x2 const sh_1 = (argElement_2w_x2)GLOB_sh_1_x2;

    __m512i ai, aj, a2i, a2j;
    __m512i b0, b1, b2, b3, b4;
    __m512i d0, d1, d2, d3, d4;
    __m512i e0, e1, e2, e3, e4;
    __m512i c0, c1, c2, c3, c4;
    __m512i k0, k1, k2, k3, k4;
    __m512i t0, t1, t2, t3, t4;

    b0 = c[0];
    b1 = c[1];
    b2 = c[2];
    b3 = c[3];
    b4 = c[4];

    e4 = MUL_x2(b4, *times_19);
    d4 = ALIGNR_x2(b4, e4);
    e3 = MUL_x2(b3, *times_19);
    d3 = ALIGNR_x2(b3, e3);
    e2 = MUL_x2(b2, *times_19);
    d2 = ALIGNR_x2(b2, e2);
    e1 = MUL_x2(b1, *times_19);
    d1 = ALIGNR_x2(b1, e1);
    e0 = MUL_x2(b0, *times_19);
    d0 = ALIGNR_x2(b0, e0);

    ai = SHUF32_x2(c[0], 0x44);
    aj = SHUF32_x2(c[1], 0x44);
    a2i = ADD_x2(ai, ai);
    a2j = ADD_x2(aj, aj);
    c0 = MUL_x2(b0, ai);
    k0 = MUL_x2(d4, a2j);
    c1 = MUL_x2(b1, a2i); /*k1 = MUL(b0,aj);*/
    c2 = MUL_x2(b2, a2i);
    k2 = MUL_x2(b1, aj);
    c3 = MUL_x2(b3, a2i);
    k3 = MUL_x2(b2, a2j);
    c4 = MUL_x2(b4, a2i);
    k4 = MUL_x2(b3, a2j);

    ai = SHUF32_x2(c[2], 0x44);
    aj = SHUF32_x2(c[3], 0x44);
    a2i = ADD_x2(ai, ai);
    a2j = ADD_x2(aj, aj);
    c0 = ADD_x2(c0, MUL_x2(d3, a2i)); /*k0 = ADD(k0, MUL(d2,aj));*/
    c1 = ADD_x2(c1, MUL_x2(d4, a2i));
    k1 = MUL_x2(d3, aj);
    /*c2 = ADD(c2,MUL(b0,ai));*/ k2 = ADD_x2(k2, MUL_x2(d4, a2j));
    /*c3 = ADD(c3,MUL(b1,ai));*/	 /*k3 = ADD(k3,MUL(b0,aj));*/
    c4 = ADD_x2(c4, MUL_x2(b2, ai)); /*k4 = ADD(k4,MUL(b1,aj));*/

    ai = SHUF32_x2(c[4], 0x44);
    aj = SHUF32_x2(c[0], 0xEE);
    a2i = ADD_x2(ai, ai);
    a2j = ADD_x2(aj, aj);
    /*c0 = ADD(c0,MUL(d1,ai));*/ k0 = ADD_x2(k0, MUL_x2(d0, aj));
    /*c1 = ADD(c1,MUL(d2,ai));*/ k1 = ADD_x2(k1, MUL_x2(d1, a2j));
    /*c2 = ADD(c2,MUL(d3,ai));*/ k2 = ADD_x2(k2, MUL_x2(d2, a2j));
    c3 = ADD_x2(c3, MUL_x2(d4, ai));
    k3 = ADD_x2(k3, MUL_x2(d3, a2j));
    /*c4 = ADD(c4,MUL(b0,ai));*/ k4 = ADD_x2(k4, MUL_x2(d4, a2j));

    ai = SHUF32_x2(c[1], 0xEE);
    aj = SHUF32_x2(c[2], 0xEE);
    a2i = ADD_x2(ai, ai);
    a2j = ADD_x2(aj, aj);
    c0 = ADD_x2(c0, MUL_x2(e4, a2i));
    k0 = ADD_x2(k0, MUL_x2(e3, a2j));
    /*c1 = ADD(c1,MUL(d0,ai));*/ k1 = ADD_x2(k1, MUL_x2(e4, a2j));
    c2 = ADD_x2(c2, MUL_x2(d1, ai));  /*k2 = ADD(k2,MUL(d0,aj));*/
    c3 = ADD_x2(c3, MUL_x2(d2, a2i)); /*k3 = ADD(k3,MUL(d1,aj));*/
    c4 = ADD_x2(c4, MUL_x2(d3, a2i));
    k4 = ADD_x2(k4, MUL_x2(d2, aj));

    ai = SHUF32_x2(c[3], 0xEE);
    aj = SHUF32_x2(c[4], 0xEE);
    a2i = ADD_x2(ai, ai);
    /*c0 = ADD(c0,MUL(e2,ai));*/	  /*k0 = ADD(k0,MUL(e1,aj));*/
    c1 = ADD_x2(c1, MUL_x2(e3, ai));  /*k1 = ADD(k1,MUL(e2,aj));*/
    c2 = ADD_x2(c2, MUL_x2(e4, a2i)); /*k2 = ADD(k2,MUL(e3,aj));*/
    /*c3 = ADD(c3,MUL(d0,ai));*/ k3 = ADD_x2(k3, MUL_x2(e4, aj));
    /*c4 = ADD(c4,MUL(d1,ai));*/ /*k4 = ADD(k4,MUL(d0,aj));*/

    t0 = SHLV_x2(k0, *sh_0);
    t1 = SHLV_x2(k1, *sh_1);
    t2 = SHLV_x2(k2, *sh_0);
    t3 = SHLV_x2(k3, *sh_1);
    t4 = SHLV_x2(k4, *sh_0);

    c[0] = ADD_x2(c0, t0);
    c[1] = ADD_x2(c1, t1);
    c[2] = ADD_x2(c2, t2);
    c[3] = ADD_x2(c3, t3);
    c[4] = ADD_x2(c4, t4);
}

#define ones26 (((uint64_t)1 << BASE0_FP25519) - 1)
#define ones25 (((uint64_t)1 << BASE1_FP25519) - 1)
static const ALIGN uint64_t GLOB_mask0_x2[8] = {ones26, ones25, ones26, ones25, ones26, ones25, ones26, ones25};
static const ALIGN uint64_t GLOB_mask1_x2[8] = {ones25, ones26, ones25, ones26, ones25, ones26, ones25, ones26};
#undef ones26
#undef ones25
static const ALIGN uint64_t GLOB_shift0_x2[8] = {BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519};
static const ALIGN uint64_t GLOB_shift1_x2[8] = {BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519};
static const ALIGN uint64_t GLOB_shift_4_x2[8] = {64, 4, 64, 4, 64, 4, 64, 4};
static const ALIGN uint64_t GLOB_shift_1_x2[8] = {64, 1, 64, 1, 64, 1, 64, 1};

#define mul19(A)                                                                \
	A = SHUF32_x2(ADD_x2(ADD_x2(                                                \
							 SHLV_x2(A, *((argElement_2w_x2)GLOB_shift_4_x2)),  \
							 SHLV_x2(A, *((argElement_2w_x2)GLOB_shift_1_x2))), \
						 A),                                                    \
				  0x4E)

DECL(void, compress)(argElement_2w_x2 c)
{
    argElement_2w_x2 const mask0 = (argElement_2w_x2)GLOB_mask0_x2;
    argElement_2w_x2 const mask1 = (argElement_2w_x2)GLOB_mask1_x2;
    argElement_2w_x2 const shift0 = (argElement_2w_x2)GLOB_shift0_x2;
    argElement_2w_x2 const shift1 = (argElement_2w_x2)GLOB_shift1_x2;

    __m512i c0 = c[0];
    __m512i c1 = c[1];
    __m512i c2 = c[2];
    __m512i c3 = c[3];
    __m512i c4 = c[4];

    __m512i h0_5, h1_6, h2_7, h3_8, h4_9;

    h0_5 = SHRV_x2(c0, *shift0);
    c0 = AND_x2(c0, *mask0);
    c1 = ADD_x2(c1, h0_5);

    h1_6 = SHRV_x2(c1, *shift1);
    c1 = AND_x2(c1, *mask1);
    c2 = ADD_x2(c2, h1_6);

    h2_7 = SHRV_x2(c2, *shift0);
    c2 = AND_x2(c2, *mask0);
    c3 = ADD_x2(c3, h2_7);

    h3_8 = SHRV_x2(c3, *shift1);
    c3 = AND_x2(c3, *mask1);
    c4 = ADD_x2(c4, h3_8);

    h4_9 = SHRV_x2(c4, *shift0);
    c4 = AND_x2(c4, *mask0);

    mul19(h4_9);

    c0 = ADD_x2(c0, h4_9);

    h0_5 = SHRV_x2(c0, *shift0);
    c0 = AND_x2(c0, *mask0);
    c1 = ADD_x2(c1, h0_5);

    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
    c[3] = c3;
    c[4] = c4;
}

DECL(void, compress2)(argElement_2w_x2 c, argElement_2w_x2 d)
{
    argElement_2w_x2 const mask0 = (argElement_2w_x2)GLOB_mask0_x2;
    argElement_2w_x2 const mask1 = (argElement_2w_x2)GLOB_mask1_x2;
    argElement_2w_x2 const shift0 = (argElement_2w_x2)GLOB_shift0_x2;
    argElement_2w_x2 const shift1 = (argElement_2w_x2)GLOB_shift1_x2;

    __m512i c0 = c[0];
    __m512i d0 = d[0];
    __m512i c1 = c[1];
    __m512i d1 = d[1];
    __m512i c2 = c[2];
    __m512i d2 = d[2];
    __m512i c3 = c[3];
    __m512i d3 = d[3];
    __m512i c4 = c[4];
    __m512i d4 = d[4];

    __m512i h0_5, h1_6, h2_7, h3_8, h4_9;
    __m512i H0_5, H1_6, H2_7, H3_8, H4_9;

    h0_5 = SHRV_x2(c0, *shift0);
    H0_5 = SHRV_x2(d0, *shift0);
    c0 = AND_x2(c0, *mask0);
    d0 = AND_x2(d0, *mask0);
    c1 = ADD_x2(c1, h0_5);
    d1 = ADD_x2(d1, H0_5);

    h1_6 = SHRV_x2(c1, *shift1);
    H1_6 = SHRV_x2(d1, *shift1);
    c1 = AND_x2(c1, *mask1);
    d1 = AND_x2(d1, *mask1);
    c2 = ADD_x2(c2, h1_6);
    d2 = ADD_x2(d2, H1_6);

    h2_7 = SHRV_x2(c2, *shift0);
    H2_7 = SHRV_x2(d2, *shift0);
    c2 = AND_x2(c2, *mask0);
    d2 = AND_x2(d2, *mask0);
    c3 = ADD_x2(c3, h2_7);
    d3 = ADD_x2(d3, H2_7);

    h3_8 = SHRV_x2(c3, *shift1);
    H3_8 = SHRV_x2(d3, *shift1);
    c3 = AND_x2(c3, *mask1);
    d3 = AND_x2(d3, *mask1);
    c4 = ADD_x2(c4, h3_8);
    d4 = ADD_x2(d4, H3_8);

    h4_9 = SHRV_x2(c4, *shift0);
    H4_9 = SHRV_x2(d4, *shift0);
    c4 = AND_x2(c4, *mask0);
    d4 = AND_x2(d4, *mask0);

    mul19(h4_9);
    mul19(H4_9);

    c0 = ADD_x2(c0, h4_9);
    d0 = ADD_x2(d0, H4_9);

    h0_5 = SHRV_x2(c0, *shift0);
    H0_5 = SHRV_x2(d0, *shift0);
    c0 = AND_x2(c0, *mask0);
    d0 = AND_x2(d0, *mask0);
    c1 = ADD_x2(c1, h0_5);
    d1 = ADD_x2(d1, H0_5);

    c[0] = c0;
    d[0] = d0;
    c[1] = c1;
    d[1] = d1;
    c[2] = c2;
    d[2] = d2;
    c[3] = c3;
    d[3] = d3;
    c[4] = c4;
    d[4] = d4;
}

DECL(void, compressfast)(argElement_2w_x2 c)
{
    argElement_2w_x2 const mask0 = (argElement_2w_x2)GLOB_mask0_x2;
    argElement_2w_x2 const mask1 = (argElement_2w_x2)GLOB_mask1_x2;
    argElement_2w_x2 const shift0 = (argElement_2w_x2)GLOB_shift0_x2;
    argElement_2w_x2 const shift1 = (argElement_2w_x2)GLOB_shift1_x2;

    __m512i L[5], M[5];

    L[0] = AND_x2(c[0], *mask0);
    L[1] = AND_x2(c[1], *mask1);
    L[2] = AND_x2(c[2], *mask0);
    L[3] = AND_x2(c[3], *mask1);
    L[4] = AND_x2(c[4], *mask0);

    M[0] = SHRV_x2(c[0], *shift0);
    M[1] = SHRV_x2(c[1], *shift1);
    M[2] = SHRV_x2(c[2], *shift0);
    M[3] = SHRV_x2(c[3], *shift1);
    M[4] = SHRV_x2(c[4], *shift0);

    mul19(M[4]);

    c[0] = ADD_x2(L[0], M[4]);
    c[1] = ADD_x2(L[1], M[0]);
    c[2] = ADD_x2(L[2], M[1]);
    c[3] = ADD_x2(L[3], M[2]);
    c[4] = ADD_x2(L[4], M[3]);
}

#undef mul19

DECL(void, neg)(argElement_2w_x2 c)
{
    argElement_2w_x2 _2P = (argElement_2w_x2)CONST_2P_2P_H0H5_x2;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        c[i] = SUB_x2(_2P[i], c[i]);
    }
    FN(compress)(c);
}

DECL(void, mul)(argElement_2w_x2 c, argElement_2w_x2 a, argElement_2w_x2 b)
{
    FN(intmul)(c, a, b);
    FN(compress)(c);
}

DECL(void, sqr)(argElement_2w_x2 c)
{
    FN(intsqr)(c);
    FN(compress)(c);
}

DECL(void, sqrn)(argElement_2w_x2 c, const unsigned int times)
{
    unsigned int it;
    for (it = 0; it < times; it++) {
        FN(intsqr)(c);
        FN(compress)(c);
    }
}

DECL(void, rand)(argElement_2w_x2 a)
{
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    rand_Fp255_1w_redradix(a0_0);
    rand_Fp255_1w_redradix(a0_1);
    rand_Fp255_1w_redradix(a1_0);
    rand_Fp255_1w_redradix(a1_1);
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(inter)(a, a0, a1);
}

DECL(void, print)(FILE *file, argElement_2w_x2 a)
{
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(deinter)(a0, a1, a);
    print_Fp255_1w_redradix(file, a0_0);
    print_Fp255_1w_redradix(file, a0_1);
    print_Fp255_1w_redradix(file, a1_0);
    print_Fp255_1w_redradix(file, a1_1);
}

DECL(__m512i, sgn)(argElement_2w_x2 a)
{
    int s0_0, s0_1, s1_0, s1_1;
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(deinter)(a0, a1, a);
    s0_0 = sgn_Fp255_1w_redradix(a0_0);
    s0_1 = sgn_Fp255_1w_redradix(a0_1);
    s1_0 = sgn_Fp255_1w_redradix(a1_0);
    s1_1 = sgn_Fp255_1w_redradix(a1_1);
    return SET64_x2(0, s1_1, 0, s1_0, 0, s0_1, 0, s0_0);
}

DECL(__m512i, cmp)(argElement_2w_x2 a, argElement_2w_x2 b)
{
    int c0_0, c0_1, c1_0, c1_1;
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(deinter)(a0, a1, a);
    EltFp25519_1w_redradix b0_0, b0_1, b1_0, b1_1;
    argElement_1w b0[2] = {b0_0, b0_1};
    argElement_1w b1[2] = {b1_0, b1_1};
    FN(deinter)(b0, b1, b);
    c0_0 = cmp_Fp255_1w_redradix(a0_0, b0_0)!=0;
    c0_1 = cmp_Fp255_1w_redradix(a0_1, b0_1)!=0;
    c1_0 = cmp_Fp255_1w_redradix(a1_0, b1_0)!=0;
    c1_1 = cmp_Fp255_1w_redradix(a1_1, b1_1)!=0;
    return SET64_x2(0, c1_1, 0, c1_0, 0, c0_1, 0, c0_0);
}

DECL(argElement_2w_x2, alloc)(void)
{
    return (argElement_2w_x2)allocate_bytes((NUM_DIGITS_FP25519 / 2) * sizeof(__m512i));
}

DECL(void, unser)(argElement_2w_x2 a, uint8_t *buf)
{
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    unser_Fp255_1w_redradix(a0_0, buf + 0 * SIZE_FP25519);
    unser_Fp255_1w_redradix(a0_1, buf + 1 * SIZE_FP25519);
    unser_Fp255_1w_redradix(a1_0, buf + 2 * SIZE_FP25519);
    unser_Fp255_1w_redradix(a1_1, buf + 3 * SIZE_FP25519);
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(inter)(a, a0, a1);
}

DECL(void, ser)(uint8_t *buf, argElement_2w_x2 a)
{
    EltFp25519_1w_redradix a0_0, a0_1, a1_0, a1_1;
    argElement_1w a0[2] = {a0_0, a0_1};
    argElement_1w a1[2] = {a1_0, a1_1};
    FN(deinter)(a0, a1, a);
    ser_Fp255_1w_redradix(buf + 0 * SIZE_FP25519, a0_0);
    ser_Fp255_1w_redradix(buf + 1 * SIZE_FP25519, a0_1);
    ser_Fp255_1w_redradix(buf + 2 * SIZE_FP25519, a1_0);
    ser_Fp255_1w_redradix(buf + 3 * SIZE_FP25519, a1_1);
}

DECL(void, naddsub)(argElement_2w_x2 C, argElement_2w_x2 D, argElement_2w_x2 A, argElement_2w_x2 B)
{
    argElement_2w_x2 _2P = (argElement_2w_x2)CONST_2P_2P_H0H5_x2;

    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
        __m512i T = SUB_x2(_2P[i], A[i]);
        C[i] = SUB_x2(T, B[i]);
        D[i] = ADD_x2(T, B[i]);
    }
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP255 */
