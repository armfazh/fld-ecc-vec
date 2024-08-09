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

#if defined(DH448)

#include "simd_avx2.h"
#include "faz_fp_avx2.h"
#include "edwards448.h"
#include <string.h>

/**
 *
 * @return
 */
static inline uint8_t * allocX448_Key(void) {
  return (uint8_t*) allocate_bytes(ECDH448_KEY_SIZE_BYTES * sizeof(uint8_t));
}

static inline void randX448_Key(uint8_t * key) {
  random_bytes(key,ECDH448_KEY_SIZE_BYTES);
}

static inline void printX448_Key(FILE*file,uint8_t * key){
  print_hex_bytes(file,key,ECDH448_KEY_SIZE_BYTES);
}

#if SWAP_METHOD == CMOV
/**
 *
 * @param bit
 * @param pX
 * @param pY
 */
static inline void cmov_x448(int bit, uint64_t * const pX,uint64_t * const pY)
{
	__asm__ __volatile__(
		"test       %0,    %0     \n\t"
		"mov      (%1), %%eax     \n\t"
		"cmovnz   (%2), %%eax     \n\t"
		"mov     %%eax,   (%1)    \n\t"
		"mov     8(%1), %%ecx     \n\t"
		"cmovnz  8(%2), %%ecx     \n\t"
		"mov     %%ecx,  8(%1)    \n\t"

		"mov     32(%1), %%eax    \n\t"
		"cmovnz  32(%2), %%eax    \n\t"
		"mov     %%eax,  32(%1)   \n\t"
		"mov     40(%1), %%ecx    \n\t"
		"cmovnz  40(%2), %%ecx    \n\t"
		"mov     %%ecx,  40(%1)   \n\t"

		"mov     64(%1), %%eax    \n\t"
		"cmovnz  64(%2), %%eax    \n\t"
		"mov     %%eax,  64(%1)   \n\t"
		"mov     72(%1), %%ecx    \n\t"
		"cmovnz  72(%2), %%ecx    \n\t"
		"mov     %%ecx,  72(%1)   \n\t"

		"mov     96(%1), %%eax    \n\t"
		"cmovnz  96(%2), %%eax    \n\t"
		"mov     %%eax,  96(%1)   \n\t"
		"mov     104(%1), %%ecx   \n\t"
		"cmovnz  104(%2), %%ecx   \n\t"
		"mov     %%ecx, 104(%1)   \n\t"

		"mov     128(%1), %%eax   \n\t"
		"cmovnz  128(%2), %%eax   \n\t"
		"mov     %%eax, 128(%1)   \n\t"
		"mov     136(%1), %%ecx   \n\t"
		"cmovnz  136(%2), %%ecx   \n\t"
		"mov     %%ecx, 136(%1)   \n\t"

		"mov     160(%1), %%eax   \n\t"
		"cmovnz  160(%2), %%eax   \n\t"
		"mov     %%eax, 160(%1)   \n\t"
		"mov     168(%1), %%ecx   \n\t"
		"cmovnz  168(%2), %%ecx   \n\t"
		"mov     %%ecx, 168(%1)   \n\t"

		"mov     192(%1), %%eax   \n\t"
		"cmovnz  192(%2), %%eax   \n\t"
		"mov     %%eax, 192(%1)   \n\t"
		"mov     200(%1), %%ecx   \n\t"
		"cmovnz  200(%2), %%ecx   \n\t"
		"mov     %%ecx, 200(%1)   \n\t"

		"mov     224(%1), %%eax   \n\t"
		"cmovnz  224(%2), %%eax   \n\t"
		"mov     %%eax, 224(%1)   \n\t"
		"mov     232(%1), %%ecx   \n\t"
		"cmovnz  232(%2), %%ecx   \n\t"
		"mov     %%ecx, 232(%1)   \n\t"
	:
	:  "r" (bit),"r" (pX),"r" (pY)
	: "memory", "cc", "%rax", "%rcx"
	);
}
#endif

/**
 *
 * @param key
 * @param X2X3
 * @param Z2Z3
 */
static inline void step_ladder_x448(
    uint64_t *key,
    argElement_2w X2X3,
    argElement_2w Z2Z3) {
  int i = 0, j = 0, s = 0;
  uint64_t prev = 0;
  const uint64_t param_a24 = 39081;
  const __m256i a24 = SET164(param_a24);

  ALIGN __m256i buffer[4 * (NUM_DIGITS_FP448 / 2)];
  argElement_2w X1 = buffer + 0 * (NUM_DIGITS_FP448 / 2);
  argElement_2w t0 = buffer + 1 * (NUM_DIGITS_FP448 / 2);
  argElement_2w t1 = buffer + 2 * (NUM_DIGITS_FP448 / 2);
  argElement_2w t2 = buffer + 3 * (NUM_DIGITS_FP448 / 2);
  argElement_2w _2P = (argElement_2w) CONST_2P_2P_H0H8;

  Fp448._2w_red.arith.misc.copy(X1, X2X3);

  j = 63;
  for (i = 6; i >= 0; i--) {
    while (j >= 0) {
      uint64_t bit = (key[i] >> j) & 0x1;
      uint64_t swap = bit ^prev;
      prev = bit;

      /* X2X3: [A|C] = [X2|X3] + [Z2|Z3]  */
      /* Z2Z3: [B|D] = [X2|X3] - [Z2|Z3]  */
      Fp448._2w_red.arithex.addsub(X2X3, Z2Z3);
      /* Z2Z3: [D|B] = Permute([B|D])     */
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        Z2Z3[s] = PERM64(Z2Z3[s], 0x4E);
      }

      /* t0:   [DA|CB] = [A|C] * [D|B]    */
      Fp448._2w_red.arithex.intmul(t0, X2X3, Z2Z3);
      Fp448._2w_red.arithex.compress(t0);
      /* t1:   [CB|DA] = Perm([DA|CB])    */
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        t1[s] = PERM64(t0[s], 0x4E);
      }

      /* Constant time swap               */
#if SWAP_METHOD == PERMUTATION
      /* Using vector permutation instructions  */
      __m256i perm = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
      __m256i mask = _mm256_set1_epi32((uint32_t) swap << 2);
      __m256i maskbit0 = XOR(perm, mask);
      __m256i maskbit1 = PERM64(maskbit0, 0x4E);
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        X2X3[s] = PERMV128(X2X3[s], maskbit0);
        Z2Z3[s] = PERMV128(Z2Z3[s], maskbit1);
      }
#elif SWAP_METHOD == LOGIC
      /* Using logic arithmetic instructions    */
            const __m256i mask = SET164(-swap);
            for (s = 0; s < (NUM_DIGITS_FP448/2); s++) {
                __m256i U = PERM128(X2X3[s],Z2Z3[s],0x21);
                __m256i V = BLEND32(X2X3[s],Z2Z3[s],0xF0);
                X2X3[s] = XOR(ANDNOT(mask,V),AND(mask,U));
                Z2Z3[s] = PERM64(X2X3[s],0x4E);
            }
#elif SWAP_METHOD == CMOV
      /* Using 64-bit CMOV instruction          */
      argElement_1w pX = (argElement_1w) X2X3;
      cmov_x448(swap,pX,pX + 2);
      pX = (argElement_1w) Z2Z3;
      cmov_x448(1-swap,pX,pX + 2);
#else
#error "Define SWAP_METHOD with PERMUTATION or CMOV or LOGIC."
#endif
      /* t1:  [_|t1] = [CB|DA] + [DA|CB]  */
      /* t0:  [_|t0] = [CB|DA] - [DA|CB]  */
      Fp448._2w_red.arithex.addsub(t1, t0);

      /* t0:  [  A | t0 ] = BLEND( [ A | C ] , [ __ | t0 ] ) */
      /* t1:  [  B | t1 ] = BLEND( [ B | D ] , [ __ | t1 ] ) */
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        t0[s] = BLEND32(X2X3[s], t0[s], 0xF0);
        t1[s] = BLEND32(Z2Z3[s], t1[s], 0xF0);
      }

      Fp448._2w_red.arithex.compressfast(t0);
      Fp448._2w_red.arithex.compressfast(t1);

      /* t0:  [ AA | t0 ] = [  A | t0 ]^2 */
      Fp448._2w_red.arithex.intsqr(t0);
      /* t1:  [ BB | t1 ] = [  B | t1 ]^2 */
      Fp448._2w_red.arithex.intsqr(t1);
      /*  Reducing coefficient size       */
      Fp448._2w_red.arithex.compress2(t0, t1);

      /* t2:  [ AA |  1 ] = BLEND( [ AA | t0 ] , [ 0 | 0 ] ) */
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        t2[s] = BLEND32(t0[s], ZERO, 0xF0);
      }
      t2[0] = OR(t2[0], SET64(0, 1, 0, 0));

      /* X2X3: [ X2 | X3 ] = [ AA |  1 ] * [ BB | t1 ]  */
      Fp448._2w_red.arithex.intmul(X2X3, t2, t1);
      /*  Reducing coefficient size       */
      Fp448._2w_red.arithex.compress(X2X3);

      /* t2: [  E | __ ] = [ AA | t0 ] - [ BB | t1 ]  */
      /* t1: [a24E| __ ] =         a24 * [  E | __ ]  */
      /* t1: [  F | __ ] = [a24E| __ ] + [ AA | t0 ]  */
      /* t2: [  E | t0 ] = BLEND( [ E | _ ] , [ AA | t0 ] ) */
      /* t1: [  F | X1 ] = BLEND( [ F | _ ] , [ __ | X1 ] ) */
      for (s = 0; s < (NUM_DIGITS_FP448 / 2); s++) {
        t2[s] = ADD(t0[s], SUB(_2P[s], t1[s]));
        t1[s] = ADD(MUL(t2[s], a24), t0[s]);
        t2[s] = BLEND32(t2[s], t0[s], 0xF0);
        t1[s] = BLEND32(t1[s], X1[s], 0xF0);
      }

      /*  Reducing coefficient size       */
      Fp448._2w_red.arithex.compressfast(t1);
      /* Z2Z3: [ Z2 | Z3 ] = [  E | t0 ] * [  F | X1 ]  */
      Fp448._2w_red.arithex.intmul(Z2Z3, t2, t1);
      /*  Reducing coefficient size       */
      Fp448._2w_red.arithex.compress(Z2Z3);
      j--;
    }
    j = 63;
  }
}

/**
 *
 * @param shared_secret
 * @param session_key
 * @param secret_key
 * @return
 */
static int x448_shared_avx2(
    argECDHX_Key shared_secret,
    argECDHX_Key session_key,
    argECDHX_Key secret_key) {
  ALIGN uint8_t session[ECDH448_KEY_SIZE_BYTES];
  ALIGN uint8_t secret[ECDH448_KEY_SIZE_BYTES];
  EltFp448_2w_redradix QzPz = {ZERO};
  EltFp448_2w_redradix QxPx = {ZERO};
  EltFp448_1w_redradix Z, X, invZ, X1;

  ZEROUPPER;

  /** clamp function */
  memcpy(secret, secret_key, ECDH448_KEY_SIZE_BYTES);
  secret[0] = secret[0] & (~(uint8_t) 0x3);
  secret[ECDH448_KEY_SIZE_BYTES - 1] |= 0x80;

  memcpy(session, session_key, ECDH448_KEY_SIZE_BYTES);
  Fp448._1w_red.arith.misc.unser(X1, session);

  /* start with (kP,0P) */
  QzPz[0] = SET64(0, 1, 0, 0);
  Fp448._2w_red.arithex.inter(QxPx, X1, X1);

  /* main-loop */
  step_ladder_x448((uint64_t *) secret, QxPx, QzPz);
  spc_memset(secret, 0, ECDH448_KEY_SIZE_BYTES);

  /* Converting to affine coordinates */
  Fp448._2w_red.arithex.deinter(X, X1, QxPx);
  Fp448._2w_red.arithex.deinter(Z, X1, QzPz);

  Fp448._1w_red.arith.inv(invZ, Z);
  Fp448._1w_red.arith.mul(X, X, invZ);
  Fp448._1w_red.arith.misc.ser(shared_secret, X);

  ZEROUPPER;
  return 0;
}

static void point_Edwards2Montgomery_ed448(uint8_t * enc,PointXYZT_2w_H0H8 * P) {
  /**
   * Using the 4-isogenus map between
   * edwards448 -> curve448
   */
  EltFp448_1w_redradix X, Y, inv_X;

  Fp448._2w_red.arithex.deinter(X, Y, P->XY);
  Fp448._1w_red.arith.inv(inv_X, X);
  Fp448._1w_red.arith.mul(X, Y, inv_X);
  Fp448._1w_red.arith.sqr(X);
  Fp448._1w_red.arith.misc.ser(enc, X);
/*	deinterleave_2w_h0h8(X,Y,P->XY);
	invsqrt_Element_1w_h0h8(inv_X,X,1);
	mul_Element_1w_h0h8(X,Y,inv_X);
	compress_Element_1w_h0h8(X);
	sqr_Element_1w_h0h8(X);
	compress_Element_1w_h0h8(X);

	singleH0H8_To_str_bytes(enc,X);*/
}

/**
 *
 * @param public_key
 * @param private_key
 * @return
 */
static int x448_keygen_avx2(
	argECDHX_Key session_key,
	argECDHX_Key secret_key) {
  PointXYZT_2w_H0H8 kB;
  X448_KEY scalar;

  /* clampC function */
  memcpy(scalar, secret_key, ECDH448_KEY_SIZE_BYTES);
  scalar[0] = scalar[0] & (~(uint8_t) 0x3);
  scalar[ECDH448_KEY_SIZE_BYTES - 1] |= 0x80;

  fixed_point_multiplication_ed448(&kB, scalar);
  spc_memset(scalar, 0, ECDH448_KEY_SIZE_BYTES);

  point_Edwards2Montgomery_ed448(session_key, &kB);
  return 0;
}

#else
extern int version;
#endif /* DH448 */
