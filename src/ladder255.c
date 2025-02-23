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

#if defined(DH255) || 1

#include "simd_avx2.h"
#include "faz_fp_avx2.h"
#include "edwards255.h"
#include <string.h>

static inline uint8_t *allocX25519_Key(void)
{
    return (uint8_t *)allocate_bytes(ECDH25519_KEY_SIZE_BYTES * sizeof(uint8_t));
}

static inline void randX25519_Key(uint8_t *key)
{
    random_bytes(key, ECDH25519_KEY_SIZE_BYTES);
}

static inline void printX25519_Key(FILE *file, uint8_t *key)
{
    print_hex_bytes(file, key, ECDH25519_KEY_SIZE_BYTES);
}

#if SWAP_METHOD == CMOV
/**
 *
 * @param bit
 * @param pX
 * @param pY
 */
static inline void cmov_x25519(int bit, argElement_1w const pX, argElement_1w const pY)
{
    __asm__ __volatile__(
        "test       %0,    %0;"
        "mov      (%1), %%eax;"
        "cmovnz   (%2), %%eax;"
        "mov     %%eax,  (%1);"
        "mov     8(%1), %%ecx;"
        "cmovnz  8(%2), %%ecx;"
        "mov     %%ecx, 8(%1);"

        "mov     32(%1), %%eax ;"
        "cmovnz  32(%2), %%eax ;"
        "mov     %%eax,  32(%1);"
        "mov     40(%1), %%ecx ;"
        "cmovnz  40(%2), %%ecx ;"
        "mov     %%ecx,  40(%1);"

        "mov     64(%1), %%eax ;"
        "cmovnz  64(%2), %%eax ;"
        "mov     %%eax,  64(%1);"
        "mov     72(%1), %%ecx ;"
        "cmovnz  72(%2), %%ecx ;"
        "mov     %%ecx,  72(%1);"

        "mov     96(%1), %%eax ;"
        "cmovnz  96(%2), %%eax ;"
        "mov     %%eax,  96(%1);"
        "mov     104(%1), %%ecx;"
        "cmovnz  104(%2), %%ecx;"
        "mov     %%ecx, 104(%1);"

        "mov     128(%1), %%eax;"
        "cmovnz  128(%2), %%eax;"
        "mov     %%eax, 128(%1);"
        "mov     136(%1), %%ecx;"
        "cmovnz  136(%2), %%ecx;"
        "mov     %%ecx, 136(%1);"
        :
        : "r"(bit), "r"(pX), "r"(pY)
        : "memory", "cc", "%rax", "%rcx");
}
#endif

/**
 *
 * @param bit
 * @param px
 * @param py
 */
static inline void select_cmov_x64(uint64_t bit, argElement_1w const px, argElement_1w const py)
{
    __asm__ __volatile__(
        "test        %2,     %2 \n\t"
        "movq      (%0),  %%rax \n\t"
        "cmovnz    (%1),  %%rax \n\t"
        "movq     %%rax,   (%0) \n\t"

        "movq     8(%0),  %%rax \n\t"
        "cmovnz   8(%1),  %%rax \n\t"
        "movq     %%rax,  8(%0) \n\t"

        "movq    16(%0), %%rax  \n\t"
        "cmovnz  16(%1), %%rax  \n\t"
        "movq    %%rax , 16(%0) \n\t"

        "movq    24(%0), %%rax  \n\t"
        "cmovnz  24(%1), %%rax  \n\t"
        "movq     %%rax, 24(%0) \n\t"
        :
        : "r"(px), "r"(py), "r"(bit)
        : "memory", "cc", "%rax");
}

static const ALIGN uint64_t CONST_2P_2P_Element_2w_Fp25519[2 * NUM_DIGITS_FP25519] = {
    0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe
};

static const ALIGN uint64_t CONST_2P_2P_Element_2w_Fp25519_x2[4 * NUM_DIGITS_FP25519] = {
    0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe,
    0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
    0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe
};

static inline void step_ladder_x25519(
    uint64_t *key,
    argElement_2w X2X3,
    argElement_2w Z2Z3)
{
    int i = 0, j = 0, s = 0;
    uint64_t prev = 0;
    const uint64_t param_a24 = 121665;
    const __m256i a24 = SET164(param_a24);

    ALIGN __m256i buffer[4 * (NUM_DIGITS_FP25519 / 2)];
    argElement_2w X1 = buffer + 0 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w t0 = buffer + 1 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w t1 = buffer + 2 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w t2 = buffer + 3 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w _2P = (argElement_2w)CONST_2P_2P_Element_2w_Fp25519;

    Fp25519._2w_red.arith.misc.copy(X1, X2X3);

    j = 62;
    for (i = 3; i >= 0; i--) {
        while (j >= 0) {
            uint64_t bit = (key[i] >> j) & 0x1;
            uint64_t swap = bit ^ prev;
            prev = bit;

            /* X2X3: [A|C] = [X2|X3] + [Z2|Z3]  */
            /* Z2Z3: [B|D] = [X2|X3] - [Z2|Z3]  */
            Fp25519._2w_red.arithex.addsub(X2X3, Z2Z3);
            /* Z2Z3: [D|B] = Permute([B|D])     */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                Z2Z3[s] = PERM64(Z2Z3[s], 0x4E);
            }

            /* t0:   [DA|CB] = [A|C] * [D|B]    */
            Fp25519._2w_red.arithex.intmul(t0, X2X3, Z2Z3);
            Fp25519._2w_red.arithex.compress(t0);
            /* t1:   [CB|DA] = Perm([DA|CB])    */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t1[s] = PERM64(t0[s], 0x4E);
            }
            /* Constant time swap               */
#if SWAP_METHOD == PERMUTATION
            /* Using vector permutation instructions  */
            __m256i perm = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
            __m256i mask = _mm256_set1_epi32((uint32_t)swap << 2);
            __m256i maskbit0 = XOR(perm, mask);
            __m256i maskbit1 = PERM64(maskbit0, 0x4E);
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                X2X3[s] = PERMV128(X2X3[s], maskbit0);
                Z2Z3[s] = PERMV128(Z2Z3[s], maskbit1);
            }
#elif SWAP_METHOD == LOGIC
            /* Using logic arithmetic instructions    */
            const __m256i mask = SET164(-swap);
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                __m256i U = PERM128(X2X3[s], Z2Z3[s], 0x21);
                __m256i V = BLEND32(X2X3[s], Z2Z3[s], 0xF0);
                X2X3[s] = XOR(ANDNOT(mask, V), AND(mask, U));
                Z2Z3[s] = PERM64(X2X3[s], 0x4E);
            }
#elif SWAP_METHOD == CMOV
            /* Using 64-bit CMOV instruction          */
            argElement_1w pX = (argElement_1w)X2X3;
            cmov_x25519(swap, pX, pX + 2);
            pX = (argElement_1w)Z2Z3;
            cmov_x25519(1 - swap, pX, pX + 2);
#else
#error "Define SWAP_METHOD with PERMUTATION or CMOV or LOGIC."
#endif
            /* t1:  [_|t1] = [CB|DA] + [DA|CB]  */
            /* t0:  [_|t0] = [CB|DA] - [DA|CB]  */
            Fp25519._2w_red.arithex.addsub(t1, t0);

            /* t0:  [  A | t0 ] = BLEND( [ A | C ] , [ __ | t0 ] ) */
            /* t1:  [  B | t1 ] = BLEND( [ B | D ] , [ __ | t1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t0[s] = BLEND32(X2X3[s], t0[s], 0xF0);
                t1[s] = BLEND32(Z2Z3[s], t1[s], 0xF0);
            }

            /* t0:  [ AA | t0 ] = [  A | t0 ]^2 */
            Fp25519._2w_red.arithex.intsqr(t0);
            /* t1:  [ BB | t1 ] = [  B | t1 ]^2 */
            Fp25519._2w_red.arithex.intsqr(t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red.arithex.compress2(t0, t1);

            /* t2:  [ AA |  1 ] = BLEND( [ AA | t0 ] , [ 0 | 0 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t2[s] = BLEND32(t0[s], ZERO, 0xF0);
            }
            t2[0] = OR(t2[0], SET64(0, 1, 0, 0));

            /* X2X3: [ X2 | X3 ] = [ AA |  1 ] * [ BB | t1 ]  */
            Fp25519._2w_red.arithex.intmul(X2X3, t2, t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red.arithex.compress(X2X3);

            /* t2: [  E | __ ] = [ AA | t0 ] - [ BB | t1 ]  */
            /* t1: [a24E| __ ] =         a24 * [  E | __ ]  */
            /* t1: [  F | __ ] = [a24E| __ ] + [ AA | t0 ]  */
            /* t2: [  E | t0 ] = BLEND( [ E | _ ] , [ AA | t0 ] ) */
            /* t1: [  F | X1 ] = BLEND( [ F | _ ] , [ __ | X1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t2[s] = ADD(t0[s], SUB(_2P[s], t1[s]));
                t1[s] = ADD(MUL(t2[s], a24), t0[s]);
                t2[s] = BLEND32(t2[s], t0[s], 0xF0);
                t1[s] = BLEND32(t1[s], X1[s], 0xF0);
            }

            /*  Reducing coefficient size       */
            Fp25519._2w_red.arithex.compressfast(t1);
            /* Z2Z3: [ Z2 | Z3 ] = [  E | t0 ] * [  F | X1 ]  */
            Fp25519._2w_red.arithex.intmul(Z2Z3, t2, t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red.arithex.compress(Z2Z3);
            j--;
        }
        j = 63;
    }
}

static inline void step_ladder_x25519_x2(
    uint64_t *key_0,
    uint64_t *key_1,
    argElement_2w_x2 X2X3,
    argElement_2w_x2 Z2Z3)
{
    int i = 0, j = 0, s = 0;
    uint64_t prev_0 = 0;
    uint64_t prev_1 = 0;
    const uint64_t param_a24 = 121665;
    const __m512i a24 = SET164_x2(param_a24);

    ALIGN __m512i buffer[4 * (NUM_DIGITS_FP25519 / 2)];
    argElement_2w_x2 X1 = buffer + 0 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w_x2 t0 = buffer + 1 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w_x2 t1 = buffer + 2 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w_x2 t2 = buffer + 3 * (NUM_DIGITS_FP25519 / 2);
    argElement_2w_x2 _2P = (argElement_2w_x2)CONST_2P_2P_Element_2w_Fp25519_x2;

    Fp25519._2w_red_x2.arith.misc.copy(X1, X2X3);

    uint16_t masku16_F0 = 0xF0F0;
    const __mmask16 maskFO = _load_mask16(&masku16_F0);

    j = 62;
    for (i = 3; i >= 0; i--) {
        while (j >= 0) {
            uint64_t bit_0 = (key_0[i] >> j) & 0x1;
            uint64_t bit_1 = (key_1[i] >> j) & 0x1;
            uint64_t swap_0 = bit_0 ^ prev_0;
            uint64_t swap_1 = bit_1 ^ prev_1;
            prev_0 = bit_0;
            prev_1 = bit_1;

            /* X2X3: [A|C] = [X2|X3] + [Z2|Z3]  */
            /* Z2Z3: [B|D] = [X2|X3] - [Z2|Z3]  */
            Fp25519._2w_red_x2.arithex.addsub(X2X3, Z2Z3);
            /* Z2Z3: [D|B] = Permute([B|D])     */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                Z2Z3[s] = PERM64_x2(Z2Z3[s], 0x4E);
            }

            /* t0:   [DA|CB] = [A|C] * [D|B]    */
            Fp25519._2w_red_x2.arithex.intmul(t0, X2X3, Z2Z3);
            Fp25519._2w_red_x2.arithex.compress(t0);
            /* t1:   [CB|DA] = Perm([DA|CB])    */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t1[s] = PERM64_x2(t0[s], 0x4E);
            }
            /* Constant time swap               */
#if SWAP_METHOD == PERMUTATION
            /* Using vector permutation instructions  */
            __m512i perm = _mm512_set_epi32(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
            __m512i mask = _mm512_set_epi32(
                               (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2,
                               (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2, (uint32_t)swap_1 << 2,
                               (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2,
                               (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2, (uint32_t)swap_0 << 2);
            __m512i maskbit0 = XOR_x2(perm, mask);
            __m512i maskbit1 = PERM64_x2(maskbit0, 0x4E);
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                X2X3[s] = PERMV128_x2(X2X3[s], maskbit0);
                Z2Z3[s] = PERMV128_x2(Z2Z3[s], maskbit1);
            }
#elif SWAP_METHOD == LOGIC
            /* Using logic arithmetic instructions    */
            const __m256i mask = SET164(-swap);
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                __m256i U = PERM128(X2X3[s], Z2Z3[s], 0x21);
                __m256i V = BLEND32(X2X3[s], Z2Z3[s], 0xF0);
                X2X3[s] = XOR(ANDNOT(mask, V), AND(mask, U));
                Z2Z3[s] = PERM64(X2X3[s], 0x4E);
            }
#elif SWAP_METHOD == CMOV
            /* Using 64-bit CMOV instruction          */
            argElement_1w pX = (argElement_1w)X2X3;
            cmov_x25519(swap, pX, pX + 2);
            pX = (argElement_1w)Z2Z3;
            cmov_x25519(1 - swap, pX, pX + 2);
#else
#error "Define SWAP_METHOD with PERMUTATION or CMOV or LOGIC."
#endif
            /* t1:  [_|t1] = [CB|DA] + [DA|CB]  */
            /* t0:  [_|t0] = [CB|DA] - [DA|CB]  */
            Fp25519._2w_red_x2.arithex.addsub(t1, t0);

            /* t0:  [  A | t0 ] = BLEND( [ A | C ] , [ __ | t0 ] ) */
            /* t1:  [  B | t1 ] = BLEND( [ B | D ] , [ __ | t1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t0[s] = BLEND32_x2(X2X3[s], t0[s], maskFO);
                t1[s] = BLEND32_x2(Z2Z3[s], t1[s], maskFO);
            }

            /* t0:  [ AA | t0 ] = [  A | t0 ]^2 */
            Fp25519._2w_red_x2.arithex.intsqr(t0);
            /* t1:  [ BB | t1 ] = [  B | t1 ]^2 */
            Fp25519._2w_red_x2.arithex.intsqr(t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red_x2.arithex.compress2(t0, t1);

            /* t2:  [ AA |  1 ] = BLEND( [ AA | t0 ] , [ 0 | 0 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t2[s] = BLEND32_x2(t0[s], ZERO_x2, maskFO);
            }
            t2[0] = OR_x2(t2[0], SET64_x2(0, 1, 0, 0, 0, 1, 0, 0));

            /* X2X3: [ X2 | X3 ] = [ AA |  1 ] * [ BB | t1 ]  */
            Fp25519._2w_red_x2.arithex.intmul(X2X3, t2, t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red_x2.arithex.compress(X2X3);

            /* t2: [  E | __ ] = [ AA | t0 ] - [ BB | t1 ]  */
            /* t1: [a24E| __ ] =         a24 * [  E | __ ]  */
            /* t1: [  F | __ ] = [a24E| __ ] + [ AA | t0 ]  */
            /* t2: [  E | t0 ] = BLEND( [ E | _ ] , [ AA | t0 ] ) */
            /* t1: [  F | X1 ] = BLEND( [ F | _ ] , [ __ | X1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519 / 2); s++) {
                t2[s] = ADD_x2(t0[s], SUB_x2(_2P[s], t1[s]));
                t1[s] = ADD_x2(MUL_x2(t2[s], a24), t0[s]);
                t2[s] = BLEND32_x2(t2[s], t0[s], maskFO);
                t1[s] = BLEND32_x2(t1[s], X1[s], maskFO);
            }

            /*  Reducing coefficient size       */
            Fp25519._2w_red_x2.arithex.compressfast(t1);
            /* Z2Z3: [ Z2 | Z3 ] = [  E | t0 ] * [  F | X1 ]  */
            Fp25519._2w_red_x2.arithex.intmul(Z2Z3, t2, t1);
            /*  Reducing coefficient size       */
            Fp25519._2w_red_x2.arithex.compress(Z2Z3);
            j--;
        }
        j = 63;
    }
}

static int x25519_shared_avx2(
    argECDHX_Key shared_secret,
    argECDHX_Key session_key,
    argECDHX_Key secret_key)
{
    ALIGN uint8_t session[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint8_t secret[ECDH25519_KEY_SIZE_BYTES];
    EltFp25519_2w_redradix QxPx = {ZERO};
    EltFp25519_2w_redradix QzPz = {ZERO};
    EltFp25519_1w_fullradix Z, X, invZ;
    EltFp25519_1w_redradix ZZ, XX, X1;

    ZEROUPPER;

    /* clampC function */
    memcpy(secret, secret_key, ECDH25519_KEY_SIZE_BYTES);
    secret[0] = secret[0] & (~(uint8_t)0x7);
    secret[ECDH25519_KEY_SIZE_BYTES - 1] =
        (uint8_t)64 | (secret[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);
    /**
     * As in the draft:
     * When receiving such an array, implementations of curve25519
     * MUST mask the most-significant bit in the final byte. This
     * is done to preserve compatibility with point formats which
     * reserve the sign bit for use in other protocols and to
     * increase resistance to implementation fingerprinting
     **/
    memcpy(session, session_key, ECDH25519_KEY_SIZE_BYTES);
    session[ECDH25519_KEY_SIZE_BYTES - 1] &= (1 << (255 % 8)) - 1;
    Fp25519._1w_red.arith.misc.unser(X1, session);

    /* start with (kP,0P) */
    QzPz[0] = SET64(0, 1, 0, 0);
    Fp25519._2w_red.arithex.inter(QxPx, X1, X1);

    /* main-loop */
    step_ladder_x25519((uint64_t *)secret, QxPx, QzPz);
    spc_memset(secret, 0, ECDH25519_KEY_SIZE_BYTES);

    /** Converting to full-radix */
    Fp25519._2w_red.arithex.deinter(XX, X1, QxPx);
    Fp25519._2w_red.arithex.deinter(ZZ, X1, QzPz);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)X, XX);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)Z, ZZ);
    ZEROUPPER;

    /* Converting to affine coordinates */
    Fp25519._1w_full.arith.inv(invZ, Z);
    Fp25519._1w_full.arith.mul(X, X, invZ);
    Fp25519._1w_full.arith.misc.ser(shared_secret, X);
    return 0;
}

static int x25519_shared_avx512(
    argECDHX_Key_x2 shared_secret,
    argECDHX_Key_x2 session_key,
    argECDHX_Key_x2 secret_key)
{
    ALIGN uint8_t session0[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint8_t session1[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint8_t secret0[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint8_t secret1[ECDH25519_KEY_SIZE_BYTES];
    EltFp25519_2w_redradix_x2 QxPx = {ZERO_x2};
    EltFp25519_2w_redradix_x2 QzPz = {ZERO_x2};
    EltFp25519_1w_fullradix Z_0, X_0, invZ_0;
    EltFp25519_1w_fullradix Z_1, X_1, invZ_1;
    EltFp25519_1w_redradix ZZ_0, XX_0, X1_0;
    EltFp25519_1w_redradix ZZ_1, XX_1, X1_1;
    argElement_1w ZZ[2] = {ZZ_0, ZZ_1};
    argElement_1w XX[2] = {XX_0, XX_1};
    argElement_1w X1[2] = {X1_0, X1_1};

    ZEROUPPER;

    /* clampC function */
    memcpy(secret0, secret_key->k0, ECDH25519_KEY_SIZE_BYTES);
    secret0[0] = secret0[0] & (~(uint8_t)0x7);
    secret0[ECDH25519_KEY_SIZE_BYTES - 1] =
        (uint8_t)64 | (secret0[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);

    memcpy(secret1, secret_key->k1, ECDH25519_KEY_SIZE_BYTES);
    secret1[0] = secret1[0] & (~(uint8_t)0x7);
    secret1[ECDH25519_KEY_SIZE_BYTES - 1] =
        (uint8_t)64 | (secret1[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);
    /**
     * As in the draft:
     * When receiving such an array, implementations of curve25519
     * MUST mask the most-significant bit in the final byte. This
     * is done to preserve compatibility with point formats which
     * reserve the sign bit for use in other protocols and to
     * increase resistance to implementation fingerprinting
     **/
    memcpy(session0, session_key->k0, ECDH25519_KEY_SIZE_BYTES);
    session0[ECDH25519_KEY_SIZE_BYTES - 1] &= (1 << (255 % 8)) - 1;
    Fp25519._1w_red.arith.misc.unser(X1[0], session0);

    memcpy(session1, session_key->k1, ECDH25519_KEY_SIZE_BYTES);
    session1[ECDH25519_KEY_SIZE_BYTES - 1] &= (1 << (255 % 8)) - 1;
    Fp25519._1w_red.arith.misc.unser(X1[1], session1);

    /* start with (kP,0P) */
    QzPz[0] = SET64_x2(0, 1, 0, 0, 0, 1, 0, 0);
    Fp25519._2w_red_x2.arithex.inter(QxPx, X1, X1);

    /* main-loop */
    step_ladder_x25519_x2((uint64_t *)secret0, (uint64_t *)secret1, QxPx, QzPz);
    spc_memset(secret0, 0, ECDH25519_KEY_SIZE_BYTES);
    spc_memset(secret1, 0, ECDH25519_KEY_SIZE_BYTES);

    /** Converting to full-radix */
    Fp25519._2w_red_x2.arithex.deinter(XX, X1, QxPx);
    Fp25519._2w_red_x2.arithex.deinter(ZZ, X1, QzPz);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)X_0, XX[0]);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)X_1, XX[1]);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)Z_0, ZZ[0]);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)Z_1, ZZ[1]);
    ZEROUPPER;

    /* Converting to affine coordinates */
    Fp25519._1w_full.arith.inv(invZ_0, Z_0);
    Fp25519._1w_full.arith.mul(X_0, X_0, invZ_0);
    Fp25519._1w_full.arith.misc.ser(shared_secret->k0, X_0);

    Fp25519._1w_full.arith.inv(invZ_1, Z_1);
    Fp25519._1w_full.arith.mul(X_1, X_1, invZ_1);
    Fp25519._1w_full.arith.misc.ser(shared_secret->k1, X_1);
    return 0;
}

static inline void point_Edwards2Montgomery_ed25519(uint8_t *enc, PointXYZT_2w_H0H5 *P)
{
    /**
     * Using the birational map between
     * edwards25519-> curve25519
     */
    EltFp25519_1w_fullradix add, sub, inv_sub;
    EltFp25519_1w_redradix t0, t1, t2;
    EltFp25519_2w_redradix addZY, subZY;

    Fp25519._2w_red.arith.add(addZY, P->TZ, P->XY);
    Fp25519._2w_red.arith.sub(subZY, P->TZ, P->XY);
    Fp25519._2w_red.arithex.compress(addZY);
    Fp25519._2w_red.arithex.compress(subZY);

    Fp25519._2w_red.arithex.deinter(t2, t0, addZY);
    Fp25519._2w_red.arithex.deinter(t2, t1, subZY);

    Fp25519._1w_red.arith.misc.ser((uint8_t *)add, t0);
    Fp25519._1w_red.arith.misc.ser((uint8_t *)sub, t1);
    Fp25519._1w_full.arith.inv(inv_sub, sub);
    Fp25519._1w_full.arith.mul(add, add, inv_sub);
    Fp25519._1w_full.arith.misc.ser(enc, add);
}

#define div_8_256(r)            \
  {                             \
    uint64_t bit3 = r[3] << 61; \
    uint64_t bit2 = r[2] << 61; \
    uint64_t bit1 = r[1] << 61; \
    r[3] = (r[3] >> 3);         \
    r[2] = (r[2] >> 3) | bit3;  \
    r[1] = (r[1] >> 3) | bit2;  \
    r[0] = (r[0] >> 3) | bit1;  \
  }

static inline int x25519_keygen_avx2(
    argECDHX_Key session_key,
    argECDHX_Key secret_key)
{
    PointXYZT_2w_H0H5 kB;
    X25519_KEY scalar;
    uint64_t *ptrScalar = (uint64_t *)scalar;

    /* clampC function */
    memcpy(scalar, secret_key, ECDH25519_KEY_SIZE_BYTES);
    scalar[0] = scalar[0] & (~(uint8_t)0x7);
    scalar[ECDH25519_KEY_SIZE_BYTES - 1] = (uint8_t)64 | (scalar[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);

    div_8_256(ptrScalar);
    point_multiplication_ed25519(&kB, scalar);
    spc_memset(scalar, 0, ECDH25519_KEY_SIZE_BYTES);

    _1way_doubling_2w_H0H5(&kB);
    _1way_doubling_2w_H0H5(&kB);
    _1way_doubling_2w_H0H5(&kB);

    point_Edwards2Montgomery_ed25519(session_key, &kB);
    return 0;
}

static inline int x25519_keygen_avx512(
    argECDHX_Key_x2 session_key,
    argECDHX_Key_x2 private_key)
{
    PointXYZT_2w_H0H5 kB_0;
    PointXYZT_2w_H0H5 kB_1;
    X25519_KEY scalar_0;
    X25519_KEY scalar_1;
    uint64_t *ptrScalar_0 = (uint64_t *)scalar_0;
    uint64_t *ptrScalar_1 = (uint64_t *)scalar_1;

    /* clampC function */
    memcpy(scalar_0, private_key->k0, ECDH25519_KEY_SIZE_BYTES);
    scalar_0[0] = scalar_0[0] & (~(uint8_t)0x7);
    scalar_0[ECDH25519_KEY_SIZE_BYTES - 1] = (uint8_t)64 | (scalar_0[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);

    div_8_256(ptrScalar_0);
    point_multiplication_ed25519(&kB_0, scalar_0);
    spc_memset(scalar_0, 0, ECDH25519_KEY_SIZE_BYTES);

    _1way_doubling_2w_H0H5(&kB_0);
    _1way_doubling_2w_H0H5(&kB_0);
    _1way_doubling_2w_H0H5(&kB_0);

    point_Edwards2Montgomery_ed25519(session_key->k0, &kB_0);

    /* clampC function */
    memcpy(scalar_1, private_key->k1, ECDH25519_KEY_SIZE_BYTES);
    scalar_1[0] = scalar_1[0] & (~(uint8_t)0x7);
    scalar_1[ECDH25519_KEY_SIZE_BYTES - 1] = (uint8_t)64 | (scalar_1[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);

    div_8_256(ptrScalar_1);
    point_multiplication_ed25519(&kB_1, scalar_1);
    spc_memset(scalar_1, 0, ECDH25519_KEY_SIZE_BYTES);

    _1way_doubling_2w_H0H5(&kB_1);
    _1way_doubling_2w_H0H5(&kB_1);
    _1way_doubling_2w_H0H5(&kB_1);

    point_Edwards2Montgomery_ed25519(session_key->k1, &kB_1);
    return 0;
}

static int x25519_shared_x64(
    argECDHX_Key shared_secret,
    argECDHX_Key session_key,
    argECDHX_Key secret_key)
{
    ALIGN uint8_t session[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint8_t secret[ECDH25519_KEY_SIZE_BYTES];
    ALIGN uint64_t buffer[4 * NUM_DIGITS_FP25519];
    ALIGN uint64_t coordinates[4 * NUM_DIGITS_FP25519];
    ALIGN uint64_t workspace[6 * NUM_DIGITS_FP25519];
    EltFp25519_1w_fullradix X1;

    int i = 0, j = 0;
    uint64_t prev = 0;
    uint64_t *const key = (uint64_t *)secret;
    uint64_t *const Px = coordinates + 0;
    uint64_t *const Pz = coordinates + 4;
    uint64_t *const Qx = coordinates + 8;
    uint64_t *const Qz = coordinates + 12;
    uint64_t *const X2 = Qx;
    uint64_t *const Z2 = Qz;
    uint64_t *const X3 = Px;
    uint64_t *const Z3 = Pz;
    uint64_t *const X2Z2 = Qx;
    uint64_t *const X3Z3 = Px;

    uint64_t *const A = workspace + 0;
    uint64_t *const B = workspace + 4;
    uint64_t *const D = workspace + 8;
    uint64_t *const C = workspace + 12;
    uint64_t *const DA = workspace + 16;
    uint64_t *const CB = workspace + 20;
    uint64_t *const AB = A;
    uint64_t *const DC = D;
    uint64_t *const DACB = DA;
    uint64_t *const buffer_2w = buffer;

    /* clampC function */
    memcpy(secret, secret_key, ECDH25519_KEY_SIZE_BYTES);
    secret[0] = secret[0] & (~(uint8_t)0x7);
    secret[ECDH25519_KEY_SIZE_BYTES - 1] =
        (uint8_t)64 | (secret[ECDH25519_KEY_SIZE_BYTES - 1] & (uint8_t)0x7F);

    /**
     * As in the draft:
     * When receiving such an array, implementations of curve25519
     * MUST mask the most-significant bit in the final byte. This
     * is done to preserve compatibility with point formats which
     * reserve the sign bit for use in other protocols and to
     * increase resistance to implementation fingerprinting
     **/
    memcpy(session, session_key, ECDH25519_KEY_SIZE_BYTES);
    session[ECDH25519_KEY_SIZE_BYTES - 1] &= (1 << (255 % 8)) - 1;

    Fp25519._1w_full.arith.misc.copy(X1, (argElement_1w)session);
    Fp25519._1w_full.arith.misc.copy(Px, (argElement_1w)session);
    Fp25519._1w_full.arith.misc.zero(Pz);
    Fp25519._1w_full.arith.misc.zero(Qx);
    Fp25519._1w_full.arith.misc.zero(Qz);

    Pz[0] = 1;
    Qx[0] = 1;

    /* main-loop */
    j = 62;
    for (i = 3; i >= 0; i--) {
        while (j >= 0) {
            uint64_t bit = (key[i] >> j) & 0x1;
            uint64_t swap = bit ^ prev;
            prev = bit;

            Fp25519._1w_full.arith.add(A, X2, Z2); /* A = (X2+Z2)                   */
            Fp25519._1w_full.arith.sub(B, X2, Z2); /* B = (X2-Z2)                   */
            Fp25519._1w_full.arith.add(C, X3, Z3); /* C = (X3+Z3)                   */
            Fp25519._1w_full.arith.sub(D, X3, Z3); /* D = (X3-Z3)                   */

            Fp25519._1w_full.arithex.intmul2(buffer_2w, AB, DC); /* [DA|CB] = [A|B]*[D|C]         */
            Fp25519._1w_full.arithex.reduce2(DACB, buffer_2w);

            select_cmov_x64(swap, A, C);
            select_cmov_x64(swap, B, D);
            Fp25519._1w_full.arithex.intsqr2(buffer_2w, AB); /* [AA|BB] = [A^2|B^2]           */
            Fp25519._1w_full.arithex.reduce2(AB, buffer_2w);

            Fp25519._1w_full.arith.add(X3, DA, CB);            /* X3 = (DA+CB)                  */
            Fp25519._1w_full.arith.sub(Z3, DA, CB);            /* Z3 = (DA-CB)                  */
            Fp25519._1w_full.arithex.intsqr2(buffer_2w, X3Z3); /* [X3|Z3] = [(DA+CB)|(DA+CB)]^2 */
            Fp25519._1w_full.arithex.reduce2(X3Z3, buffer_2w);

            Fp25519._1w_full.arith.misc.copy(X2, B);               /* X2 = B^2                      */
            Fp25519._1w_full.arith.sub(Z2, A, B);                  /* Z2 = E = AA-BB                */
            Fp25519._1w_full.arithex.mula24(B, Z2);                /*  B = a24*E                    */
            Fp25519._1w_full.arith.add(B, B, X2);                  /* B = a24*E+B                   */
            Fp25519._1w_full.arithex.intmul2(buffer_2w, X2Z2, AB); /* [X2|Z2] = [B|E]*[A|a24*E+B]   */
            Fp25519._1w_full.arithex.reduce2(X2Z2, buffer_2w);

            Fp25519._1w_full.arith.mul(Z3, Z3, X1); /* Z3 = Z3*X1                    */
            j--;
        }
        j = 63;
    }
    spc_memset(secret, 0, ECDH25519_KEY_SIZE_BYTES);

    /* Converting to affine coordinates */
    Fp25519._1w_full.arith.inv(A, Qz);
    Fp25519._1w_full.arith.mul(Qx, Qx, A);
    Fp25519._1w_full.arith.misc.ser(shared_secret, Qx);
    return 0;
}

static int x25519_keygen_x64(
    argECDHX_Key session_key,
    argECDHX_Key secret_key)
{
    ALIGN uint8_t gen[ECDH25519_KEY_SIZE_BYTES] = {0};
    gen[0] = 9;
    return x25519_shared_x64(session_key, gen, secret_key);
}

#else
extern int version;
#endif /* DH255 */
