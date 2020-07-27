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

#define FN(NAME) SUFFIX(NAME,1,redradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

#define mul19_128(A)           \
  A = SHUF32128(ADD128(ADD128( \
        SHLV128(A, shift_4),   \
        SHLV128(A, shift_1)),  \
        A),0x4E)

DECL(void, compress)(argElement_1w c) {
  const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
  const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
  const __m128i mask0 = SET32(0, ones25, 0, ones26);
  const __m128i mask1 = SET32(0, ones26, 0, ones25);
  const __m128i shift0 = SET32(0, 25, 0, 26);
  const __m128i shift1 = SET32(0, 26, 0, 25);
  const __m128i shift_4 = SET32(0, 4, 0, 64);
  const __m128i shift_1 = SET32(0, 1, 0, 64);

  __m128i c0 = LOAD128(c + 0);
  __m128i c1 = LOAD128(c + 1);
  __m128i c2 = LOAD128(c + 2);
  __m128i c3 = LOAD128(c + 3);
  __m128i c4 = LOAD128(c + 4);

  __m128i h0_5, h1_6, h2_7, h3_8, h4_9;

  h0_5 = SHRV128(c0, shift0);
  c0 = AND128(c0, mask0);
  c1 = ADD128(c1, h0_5);

  h1_6 = SHRV128(c1, shift1);
  c1 = AND128(c1, mask1);
  c2 = ADD128(c2, h1_6);

  h2_7 = SHRV128(c2, shift0);
  c2 = AND128(c2, mask0);
  c3 = ADD128(c3, h2_7);

  h3_8 = SHRV128(c3, shift1);
  c3 = AND128(c3, mask1);
  c4 = ADD128(c4, h3_8);

  h4_9 = SHRV128(c4, shift0);
  c4 = AND128(c4, mask0);

  mul19_128(h4_9);

  c0 = ADD128(c0, h4_9);

  h0_5 = SHRV128(c0, shift0);
  c0 = AND128(c0, mask0);
  c1 = ADD128(c1, h0_5);

  STORE128(c + 0, c0);
  STORE128(c + 1, c1);
  STORE128(c + 2, c2);
  STORE128(c + 3, c3);
  STORE128(c + 4, c4);
}

DECL(void, compressfast)(argElement_1w c) {
  const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
  const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
  const __m128i mask0 = SET32(0, ones25, 0, ones26);
  const __m128i mask1 = SET32(0, ones26, 0, ones25);
  const __m128i shift0 = SET32(0, 25, 0, 26);
  const __m128i shift1 = SET32(0, 26, 0, 25);
  const __m128i shift_4 = SET32(0, 4, 0, 64);
  const __m128i shift_1 = SET32(0, 1, 0, 64);

  __m128i C[5], L[5], M[5];

  C[0] = LOAD128(c + 0);
  C[1] = LOAD128(c + 1);
  C[2] = LOAD128(c + 2);
  C[3] = LOAD128(c + 3);
  C[4] = LOAD128(c + 4);

  L[0] = AND128(C[0], mask0);
  L[1] = AND128(C[1], mask1);
  L[2] = AND128(C[2], mask0);
  L[3] = AND128(C[3], mask1);
  L[4] = AND128(C[4], mask0);

  M[0] = SHRV128(C[0], shift0);
  M[1] = SHRV128(C[1], shift1);
  M[2] = SHRV128(C[2], shift0);
  M[3] = SHRV128(C[3], shift1);
  M[4] = SHRV128(C[4], shift0);

  mul19_128(M[4]);

  C[0] = ADD128(L[0], M[4]);
  C[1] = ADD128(L[1], M[0]);
  C[2] = ADD128(L[2], M[1]);
  C[3] = ADD128(L[3], M[2]);
  C[4] = ADD128(L[4], M[3]);

  STORE128(c + 0, C[0]);
  STORE128(c + 1, C[1]);
  STORE128(c + 2, C[2]);
  STORE128(c + 3, C[3]);
  STORE128(c + 4, C[4]);
}

DECL(void, sqrn)(argElement_1w a, unsigned int times) {
  const __m128i k_19_1 = SET32(0,1,0,19);
  const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
  const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
  const __m128i mask0 = SET32(0, ones25, 0, ones26);
  const __m128i mask1 = SET32(0, ones26, 0, ones25);
  const __m128i shift0 = SET32(0, 25, 0, 26);
  const __m128i shift1 = SET32(0, 26, 0, 25);
  const __m128i shift_4 = SET32(0,4,0,64);
  const __m128i shift_1 = SET32(0,1,0,64);

  __m128i ai,aj,ae,ao,ak,al;
  __m128i b0,b1,b2,b3,b4;
  __m128i d0,d1,d2,d3,d4;
  __m128i c0,c1,c2,c3,c4;
  unsigned int it = 0;

  b0 = LOAD128(a+0);
  b1 = LOAD128(a+1);
  b2 = LOAD128(a+2);
  b3 = LOAD128(a+3);
  b4 = LOAD128(a+4);

  for(it=0;it<times;it++)
  {
    __m128i A0,A1,A2,A3,A4;
    __m128i h0_5, h1_6, h2_7, h3_8, h4_9;

    A0 = b0;
    A1 = b1;
    A2 = b2;
    A3 = b3;
    A4 = b4;
    ai = SHUF32128(A0, 0x44);		aj = ADD128(ai, ai);/* 2a0 */
    ak = SHUF32128(A1, 0x44);		al = ADD128(ak, ak);/* 2a1 */
    ak = BLEND32128(ak, al, 0x3);	ao = ADD128(ak, ak);
    ae = SHUF32128(ao, 0x4E);

    d4 = SHUF32128(b4, 0x4E);
    d4 = MUL128(d4, k_19_1);

    c0 = MUL128(b0, ai);		c0 = ADD128(c0, MUL128(d4, ao));
    c1 = MUL128(b1, aj);      /*c1 = ADD128(c1, MUL128(b0, --));*/
    c2 = MUL128(b2, aj);		c2 = ADD128(c2, MUL128(b1, ak));
    c3 = MUL128(b3, aj);		c3 = ADD128(c3, MUL128(b2, ae));
    c4 = MUL128(b4, aj);		c4 = ADD128(c4, MUL128(b3, ao));

    ai = SHUF32128(A2, 0x44);		aj = ADD128(ai, ai);/* 2a2 */
    ak = SHUF32128(A3, 0x44);		al = ADD128(ak, ak);/* 2a3 */
    ak = BLEND32128(ak, al, 0xC);	ao = ADD128(ak, ak);
    ao = SHUF32128(ao, 0x4E);

    d3 = SHUF32128(b3, 0x4E);
    d2 = SHUF32128(b2, 0x4E);

    d3 = MUL128(d3, k_19_1);
    d2 = MUL128(d2, k_19_1);
    c0 = ADD128(c0, MUL128(d3, aj));     /*c0 = ADD128(c0,MUL128(d2,--));*/
    c1 = ADD128(c1, MUL128(d4, aj));	   c1 = ADD128(c1, MUL128(d3, ak));
    /*c2 = ADD128(c2,MUL128(b0,--));*/     c2 = ADD128(c2, MUL128(d4, ao));
    /*c3 = ADD128(c3,MUL128(b1,--));*/   /*c3 = ADD128(c3,MUL128(b0,--));*/
    c4 = ADD128(c4, MUL128(b2, ai));     /*c4 = ADD128(c4,MUL128(b1,--));*/

    ai = SHUF32128(A4, 0x44);
    aj = SHUF32128(A0, 0xEE);
    ak = ADD128(aj, aj);/* 2a5 */
    al = ADD128(ak, ak);/* 4a5 */
    aj = BLEND32128(aj, ak, 0x3);
    ao = BLEND32128(ak, al, 0xC);
    ae = BLEND32128(ak, al, 0x3);

    d1 = SHUF32128(b1, 0x4E);
    d0 = SHUF32128(b0, 0x4E);
    d1 = MUL128(d1, k_19_1);
    d0 = MUL128(d0, k_19_1);
    /*c0 = ADD128(c0,MUL128(d1,--));*/	c0 = ADD128(c0, MUL128(d0, aj));
    /*c1 = ADD128(c1,MUL128(d2,--));*/	c1 = ADD128(c1, MUL128(d1, ao));
    /*c2 = ADD128(c2,MUL128(d3,--));*/	c2 = ADD128(c2, MUL128(d2, ae));
    c3 = ADD128(c3, MUL128(d4, ai));	c3 = ADD128(c3, MUL128(d3, ao));
    /*c4 = ADD128(c4,MUL128(b0,--));*/	c4 = ADD128(c4, MUL128(d4, ae));

    ai = SHUF32128(A1, 0xEE);		aj = ADD128(ai, ai);
    ak = SHUF32128(A2, 0xEE);		al = ADD128(ak, ak);/* 2a7 */
    ak = BLEND32128(ak, al, 0x3);	ao = ADD128(ak, ak);
    ae = SHUF32128(ao, 0x4E);

    b4 = SHUF32128(d4, 0x4E);
    b3 = SHUF32128(d3, 0x4E);
    b4 = MUL128(b4, k_19_1);
    b3 = MUL128(b3, k_19_1);
    c0 = ADD128(c0, MUL128(b4, aj)); 	c0 = ADD128(c0, MUL128(b3, ao));
    /*c1 = ADD128(c1,MUL128(d0,--));*/	c1 = ADD128(c1, MUL128(b4, ae));
    c2 = ADD128(c2, MUL128(d1, ai));  	/*c2 = ADD128(c2,MUL128(d0,--));*/
    c3 = ADD128(c3, MUL128(d2, aj));  	/*c3 = ADD128(c3,MUL128(d1,--));*/
    c4 = ADD128(c4, MUL128(d3, aj));	c4 = ADD128(c4, MUL128(d2, ak));

    ai = SHUF32128(A3, 0xEE);		aj = ADD128(ai, ai);
    ak = SHUF32128(A4, 0xEE);		al = ADD128(ak, ak);
    ae = BLEND32128(ak, al, 0xC);

    /*c0 = ADD128(c0,MUL128(b2,--));*/    /*c0 = ADD128(c0,MUL128(b1,ae));*/
    c1 = ADD128(c1, MUL128(b3, ai));      /*c1 = ADD128(c1,MUL128(b2,ao));*/
    c2 = ADD128(c2, MUL128(b4, aj));      /*c2 = ADD128(c2,MUL128(b3,ae));*/
    /*c3 = ADD128(c3,MUL128(d0,--));*/    c3 = ADD128(c3, MUL128(b4, ae));
    /*c4 = ADD128(c4,MUL128(d1,--));*/    /*c4 = ADD128(c4,MUL128(d0,ae));*/

    /* Digit Size Reduction */
    h0_5 = SHRV128(c0, shift0);
    c0 = AND128(c0, mask0);
    c1 = ADD128(c1, h0_5);

    h1_6 = SHRV128(c1, shift1);
    c1 = AND128(c1, mask1);
    c2 = ADD128(c2, h1_6);

    h2_7 = SHRV128(c2, shift0);
    c2 = AND128(c2, mask0);
    c3 = ADD128(c3, h2_7);

    h3_8 = SHRV128(c3, shift1);
    c3 = AND128(c3, mask1);
    c4 = ADD128(c4, h3_8);

    h4_9 = SHRV128(c4, shift0);
    c4 = AND128(c4, mask0);

    mul19_128(h4_9);

    c0 = ADD128(c0, h4_9);

    h0_5 = SHRV128(c0, shift0);
    c0 = AND128(c0, mask0);
    c1 = ADD128(c1, h0_5);

    h1_6 = SHRV128(c1, shift1);
    c1 = AND128(c1, mask1);
    c2 = ADD128(c2, h1_6);

    b0 = c0;
    b1 = c1;
    b2 = c2;
    b3 = c3;
    b4 = c4;
  }

  STORE128(a + 0, b0);
  STORE128(a + 1, b1);
  STORE128(a + 2, b2);
  STORE128(a + 3, b3);
  STORE128(a + 4, b4);
}

#undef mul19_128

DECL(void, unser)(argElement_1w pC, uint8_t *p8A) {
  const uint64_t mask0 = ((uint64_t) 1 << (BASE0_FP25519)) - 1;
  const uint64_t mask1 = ((uint64_t) 1 << (BASE1_FP25519)) - 1;
  const __m128i vmask0 = _mm_set_epi64x(mask1, mask0);
  const __m128i vmask1 = _mm_set_epi64x(mask0, mask1);
  EltFp25519_1w_fullradix pA;
  __m128i xA, xF;
  __m128i z0, z1, z2, z3, z4;
  __m128i xB, xC, xD, xE, xG, xH, xI, xJ;

  copy_Fp255_1w_fullradix(pA,(argElement_1w)p8A);
  modp_Fp255_1w_fullradix(pA);

  xA = LOAD128(pA + 0);
  xF = LOAD128(pA + 1);

  xC = SHR128(SHR8N128(xA, (BASE0_FP25519 + BASE1_FP25519) / 8), (BASE0_FP25519 + BASE1_FP25519) % 8);
  xH = SHR128(SHR8N128(xF, (BASE0_FP25519 + BASE1_FP25519) / 8), (BASE0_FP25519 + BASE1_FP25519) % 8);
  xE = SHR128(SHR8N128(xA, (2 * (BASE0_FP25519 + BASE1_FP25519)) / 8), (2 * (BASE0_FP25519 + BASE1_FP25519)) % 8);
  xJ = SHR128(SHR8N128(xF, (2 * (BASE0_FP25519 + BASE1_FP25519)) / 8), (2 * (BASE0_FP25519 + BASE1_FP25519)) % 8);

  xB = SHR128(xA, BASE0_FP25519);
  xG = SHR128(xF, BASE1_FP25519);
  xD = SHR128(xC, BASE0_FP25519);
  xI = SHR128(xH, BASE1_FP25519);

  z0 = UPKL64128(xA, xF);
  z1 = UPKL64128(xB, xG);
  z2 = UPKL64128(xC, xH);
  z3 = UPKL64128(xD, xI);
  z4 = UPKL64128(xE, xJ);

  z0 = AND128(z0, vmask0);
  z1 = AND128(z1, vmask1);
  z2 = AND128(z2, vmask0);
  z3 = AND128(z3, vmask1);
  z4 = AND128(z4, vmask0);

  STORE128(pC + 0, z0);
  STORE128(pC + 1, z1);
  STORE128(pC + 2, z2);
  STORE128(pC + 3, z3);
  STORE128(pC + 4, z4);
  STORE128(pC + 5, ZERO128);
}

DECL(void, ser)(uint8_t *buf, argElement_1w a) {
  EltFp25519_1w_fullradix c;
  __asm__ __volatile__(
  "xorl  %%r8d,  %%r8d ;"
  "xorl  %%r9d,  %%r9d ;"
  "xorl %%r10d, %%r10d ;"
  "xorl %%r11d, %%r11d ;"
  "xorl %%r12d, %%r12d ;"

  "movq   (%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "addq  %%rax,  %%r8 ;"
  "adcq  %%rcx,  %%r9 ;"
  "adcq     $0, %%r10 ;"

  "movq 16(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $26, %%rax, %%rcx ;"
  "shlq    $26, %%rax ;"
  "addq  %%rax,  %%r8 ;"
  "adcq  %%rcx,  %%r9 ;"
  "adcq     $0, %%r10 ;"

  "movq 32(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $51, %%rax, %%rcx ;"
  "shlq    $51, %%rax ;"
  "addq  %%rax,  %%r8 ;"
  "adcq  %%rcx,  %%r9 ;"
  "adcq     $0, %%r10 ;"

  "movq 48(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $13, %%rax, %%rcx ;"
  "shlq    $13, %%rax ;"
  "addq  %%rax,  %%r9 ;"
  "adcq  %%rcx, %%r10 ;"
  "adcq     $0, %%r11 ;"

  "movq 64(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $38, %%rax, %%rcx ;"
  "shlq    $38, %%rax ;"
  "addq  %%rax,  %%r9 ;"
  "adcq  %%rcx, %%r10 ;"
  "adcq     $0, %%r11 ;"

  "movq  8(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "addq  %%rax, %%r10 ;"
  "adcq  %%rcx, %%r11 ;"
  "adcq     $0, %%r12 ;"

  "movq 24(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $25, %%rax, %%rcx ;"
  "shlq    $25, %%rax ;"
  "addq  %%rax, %%r10 ;"
  "adcq  %%rcx, %%r11 ;"
  "adcq     $0, %%r12 ;"

  "movq 40(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $51, %%rax, %%rcx ;"
  "shlq    $51, %%rax ;"
  "addq  %%rax, %%r10 ;"
  "adcq  %%rcx, %%r11 ;"
  "adcq     $0, %%r12 ;"

  "movq 56(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $12, %%rax, %%rcx ;"
  "shlq    $12, %%rax ;"
  "addq  %%rax, %%r11 ;"
  "adcq  %%rcx, %%r12 ;"

  "movq 72(%1), %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "shldq   $38, %%rax, %%rcx ;"
  "shlq    $38, %%rax ;"
  "addq  %%rax, %%r11 ;"
  "adcq  %%rcx, %%r12 ;"

  "imulq $38, %%r12, %%rax ;"
  "xorl  %%ecx, %%ecx ;"
  "addq  %%rax,  %%r8 ;"
  "adcq  %%rcx,  %%r9 ;"

  "movq  %%r8,   (%0) ;"
  "movq  %%r9,  8(%0) ;"
  "movq %%r10, 16(%0) ;"
  "movq %%r11, 24(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%r8", "%r9", "%r10", "%r11", "%r12"
  );
  ser_Fp255_1w_fullradix(buf,c);
}

DECL(int, sgn)(argElement_1w a) {
    ALIGN uint8_t a_bytes[SIZE_FP25519];
    FN(ser)(a_bytes, a);
    return a_bytes[0]&0x1;
}

DECL(int, cmp)(argElement_1w a, argElement_1w b) {
  ALIGN uint8_t a_bytes[SIZE_FP25519];
  ALIGN uint8_t b_bytes[SIZE_FP25519];
  FN(ser)(a_bytes, a);
  FN(ser)(b_bytes, b);
  return cmp_bytes(a_bytes, b_bytes, SIZE_FP25519) & 0x1;
}

DECL(void, cmv)(int bit, argElement_1w c, argElement_1w a, argElement_1w b) {
    const __m256i mask = SET164(-(int64_t)bit);
    __m256i a0 = LOAD(a + 0);
    __m256i a1 = LOAD(a + 1);
    __m256i a2 = LOAD(a + 2);

    __m256i b0 = LOAD(b + 0);
    __m256i b1 = LOAD(b + 1);
    __m256i b2 = LOAD(b + 2);

    __m256i c0 = _mm256_blendv_epi8(a0,b0,mask);
    __m256i c1 = _mm256_blendv_epi8(a1,b1,mask);
    __m256i c2 = _mm256_blendv_epi8(a2,b2,mask);

    STORE(c + 0, c0);
    STORE(c + 1, c1);
    STORE(c + 2, c2);
}

DECL(void, add)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __m256i a0 = LOAD(a + 0);
  __m256i a1 = LOAD(a + 1);
  __m256i a2 = LOAD(a + 2);

  __m256i b0 = LOAD(b + 0);
  __m256i b1 = LOAD(b + 1);
  __m256i b2 = LOAD(b + 2);

  __m256i c0 = ADD(a0, b0);
  __m256i c1 = ADD(a1, b1);
  __m256i c2 = ADD(a2, b2);

  STORE(c + 0, c0);
  STORE(c + 1, c1);
  STORE(c + 2, c2);
}

DECL(void, sub)(argElement_1w c, argElement_1w a, argElement_1w b) {
  const __m256i _2P0 = SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7ffffda);
  const __m256i _2P1 = SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7fffffe);
  const __m256i _2P2 = SET64(0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe);

  __m256i a0 = LOAD(a + 0);
  __m256i a1 = LOAD(a + 1);
  __m256i a2 = LOAD(a + 2);

  __m256i b0 = LOAD(b + 0);
  __m256i b1 = LOAD(b + 1);
  __m256i b2 = LOAD(b + 2);

  __m256i c0 = SUB(_2P0, b0);
  __m256i c1 = SUB(_2P1, b1);
  __m256i c2 = SUB(_2P2, b2);

  c0 = ADD(a0, c0);
  c1 = ADD(a1, c1);
  c2 = ADD(a2, c2);

  STORE(c + 0, c0);
  STORE(c + 1, c1);
  STORE(c + 2, c2);
}

DECL(void, addsub)(argElement_1w pA, argElement_1w pB) {
  const __m256i _2P[3] = {
      SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7ffffda),/* [ h6, h1, h5, h0] */
      SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7fffffe),/* [ h8, h3, h7, h2] */
      SET64(0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe) /* [ xx, xx, h9, h4] */
  };
  int i = 0;
  for (i = 0; i < 3; i++) {
    __m256i A = LOAD(pA + i);
    __m256i B = LOAD(pB + i);
    __m256i C = ADD(A, B);
    __m256i D = ADD(A, SUB(_2P[i], B));
    STORE(pA + i, C);
    STORE(pB + i, D);
  }
}

DECL(void, neg)(argElement_1w c) {
  const __m256i _2P0 = SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7ffffda);
  const __m256i _2P1 = SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7fffffe);
  const __m256i _2P2 = SET64(0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe);

  __m256i a0 = LOAD(c + 0);
  __m256i a1 = LOAD(c + 1);
  __m256i a2 = LOAD(c + 2);

  __m256i c0 = SUB(_2P0, a0);
  __m256i c1 = SUB(_2P1, a1);
  __m256i c2 = SUB(_2P2, a2);

  STORE(c + 0, c0);
  STORE(c + 1, c1);
  STORE(c + 2, c2);
}

DECL(void, intmul_sb)(argElement_1w c, argElement_1w a, argElement_1w b) {
	const __m256i k_19_1 = SET64(1,19,1,19);
	const __m256i k_1_19_1_1 = SET64(1,19,1,1);

	__m256i ai, aH, aL;
	__m256i B0,B1,B2;
	__m256i b0,b1,b2,b3,b4;
	__m256i    d1,d2,d3,d4;
	__m256i c0,c1,c2,c3,c4;
	__m128i p0,p1,p2,p3,p4;

	B0 = LOAD(b+0);
	B1 = LOAD(b+1);
	B2 = LOAD(b+2);

	b0 = PERM64(B0,0x14);
	b1 = PERM64(B0,0xBE);
	b2 = PERM64(B1,0x14);
	b3 = PERM64(B1,0xBE);
	b4 = PERM64(B2,0x14);

	b0 = MUL(b0,k_1_19_1_1);
	b1 = MUL(b1,k_1_19_1_1);
	b2 = MUL(b2,k_1_19_1_1);
	b3 = MUL(b3,k_1_19_1_1);
	b4 = MUL(b4,k_1_19_1_1);

	ai = PERM64(LOAD(a+0),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	c0 = MUL(b0, aL);
	c1 = MUL(b1, aH);
	c2 = MUL(b2, aL);
	c3 = MUL(b3, aH);
	c4 = MUL(b4, aL);

	ai = PERM64(LOAD(a+0), 0xFA);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x03);
	aH = BLEND32(ai, aH, 0x0C);

	d4 = SHUF32(b4,0x4E);
	d4 = MUL(d4,k_19_1);

	c0 = ADD(c0,MUL(d4, aL));
	c1 = ADD(c1,MUL(b0, aH));
	c2 = ADD(c2,MUL(b1, aL));
	c3 = ADD(c3,MUL(b2, aH));
	c4 = ADD(c4,MUL(b3, aL));

	ai = PERM64(LOAD(a+1),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	d3 = SHUF32(b3,0x4E);
	d3 = MUL(d3,k_19_1);

	c0 = ADD(c0,MUL(d3, aL));
	c1 = ADD(c1,MUL(d4, aH));
	c2 = ADD(c2,MUL(b0, aL));
	c3 = ADD(c3,MUL(b1, aH));
	c4 = ADD(c4,MUL(b2, aL));

	ai = PERM64(LOAD(a + 1), 0xFA);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x03);
	aH = BLEND32(ai, aH, 0x0C);

	d2 = SHUF32(b2,0x4E);
	d2 = MUL(d2,k_19_1);

	c0 = ADD(c0,MUL(d2, aL));
	c1 = ADD(c1,MUL(d3, aH));
	c2 = ADD(c2,MUL(d4, aL));
	c3 = ADD(c3,MUL(b0, aH));
	c4 = ADD(c4,MUL(b1, aL));

	ai = PERM64(LOAD(a+2),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	d1 = SHUF32(b1,0x4E);
	d1 = MUL(d1,k_19_1);

	c0 = ADD(c0,MUL(d1, aL));
	c1 = ADD(c1,MUL(d2, aH));
	c2 = ADD(c2,MUL(d3, aL));
	c3 = ADD(c3,MUL(d4, aH));
	c4 = ADD(c4,MUL(b0, aL));

	p0 = ADD128(EXTR(c0,1),CAST256TO128(c0));
	p1 = ADD128(EXTR(c1,1),CAST256TO128(c1));
	p2 = ADD128(EXTR(c2,1),CAST256TO128(c2));
	p3 = ADD128(EXTR(c3,1),CAST256TO128(c3));
	p4 = ADD128(EXTR(c4,1),CAST256TO128(c4));

	c0 = INSR(CAST128TO256(p0),p1,1);
	c1 = INSR(CAST128TO256(p2),p3,1);
	c2 = CAST128TO256(p4);

	STORE(c+0,c0);
	STORE(c+1,c1);
	STORE(c+2,c2);
}

DECL(void, intmul)(argElement_1w c, argElement_1w a, argElement_1w b) {
  FN(intmul_sb)(c,a,b);
}

DECL(void, mul)(argElement_1w c, argElement_1w a, argElement_1w b) {
  FN(intmul)(c,a,b);
  FN(compress)(c);
}

DECL(void, sqr)(argElement_1w c) {
  FN(sqrn)(c, 1);
}

DECL(void, copy)(argElement_1w c, argElement_1w a) {
  STORE(c + 0, LOAD(a + 0));
  STORE(c + 1, LOAD(a + 1));
  STORE(c + 2, LOAD(a + 2));
}

DECL(void, inv)(argElement_1w c, argElement_1w a) {
  EltFp25519_1w_redradix x0, x1, x2;
  argElement_1w T[5];

  T[0] = x0;
  T[1] = c; /* x^(-1) */
  T[2] = x1;
  T[3] = x2;
  T[4] = a; /* x */

  FN(copy)(T[1], T[4]);
  FN(sqrn)(T[1], 1);
  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 2);
  FN(intmul)(T[0], T[4], T[2]);
  FN(compress)(T[0]);
  FN(intmul)(T[1], T[1], T[0]);
  FN(compress)(T[1]);
  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 1);
  FN(intmul)(T[0], T[0], T[2]);
  FN(compress)(T[0]);
  FN(copy)(T[2], T[0]);
  FN(sqrn)(T[2], 5);
  FN(intmul)(T[0], T[0], T[2]);
  FN(compress)(T[0]);
  FN(copy)(T[2], T[0]);
  FN(sqrn)(T[2], 10);
  FN(intmul)(T[2], T[2], T[0]);
  FN(compress)(T[2]);
  FN(copy)(T[3], T[2]);
  FN(sqrn)(T[3], 20);
  FN(intmul)(T[3], T[3], T[2]);
  FN(compress)(T[3]);
  FN(sqrn)(T[3], 10);
  FN(intmul)(T[3], T[3], T[0]);
  FN(compress)(T[3]);
  FN(copy)(T[0], T[3]);
  FN(sqrn)(T[0], 50);
  FN(intmul)(T[0], T[0], T[3]);
  FN(compress)(T[0]);
  FN(copy)(T[2], T[0]);
  FN(sqrn)(T[2], 100);
  FN(intmul)(T[2], T[2], T[0]);
  FN(compress)(T[2]);
  FN(sqrn)(T[2], 50);
  FN(intmul)(T[2], T[2], T[3]);
  FN(compress)(T[2]);
  FN(sqrn)(T[2], 5);
  FN(intmul)(T[1], T[1], T[2]);
  FN(compress)(T[1]);
}

DECL(void, invsqrt)(argElement_1w uv_p38, argElement_1w u, argElement_1w v) {
  const EltFp25519_1w_redradix sqrt_minus_one = {
      0x20ea0b0, 0x1fbd7a7,
      0x186c9d2, 0x2804c9e,
      0x08f189d, 0x1e16569,
      0x035697f, 0x004fc1d,
      0x0bd0c60, 0x0ae0c92
  };
  EltFp25519_1w_redradix v2, uv, uv3, uv7, x2, x9, sqrt;
  argElement_1w Tab[4];

  FN(mul)(uv, u, v);     /* uv */
  FN(copy)(v2, v);
  FN(sqr)(v2);           /* v^2 */
  FN(mul)(uv3, uv, v2);  /* uv^3 */
  FN(sqr)(v2);           /* v^4 */
  FN(mul)(uv7, uv3, v2); /* uv^7 */

  Tab[0] = x2;
  Tab[1] = x9;
  Tab[2] = sqrt;
  Tab[3] = uv7;

  FN(copy)(Tab[0], uv7);
  FN(sqr)(Tab[0]);/* x^2 */
  /* 0 */
  FN(copy)(Tab[1], Tab[0]);
  FN(sqrn)(Tab[1], 2);
  FN(mul)(Tab[1], Tab[1], Tab[3]);
  /* 1 */
  FN(mul)(Tab[0], Tab[0], Tab[1]);
  /* 2 */
  FN(sqrn)(Tab[0], 1);
  FN(mul)(Tab[0], Tab[0], Tab[1]);
  /* 3 */
  FN(copy)(Tab[1], Tab[0]);
  FN(sqrn)(Tab[1], 5);
  FN(mul)(Tab[1], Tab[1], Tab[0]);
  /* 4 */
  FN(copy)(Tab[2], Tab[1]);
  FN(sqrn)(Tab[2], 5);
  FN(mul)(Tab[2], Tab[2], Tab[0]);
  /* 5 */
  FN(copy)(Tab[1], Tab[2]);
  FN(sqrn)(Tab[1], 15);
  FN(mul)(Tab[1], Tab[1], Tab[2]);
  /* 6 */
  FN(copy)(Tab[2], Tab[1]);
  FN(sqrn)(Tab[2], 10);
  FN(sqrn)(Tab[2], 20);
  FN(mul)(Tab[2], Tab[2], Tab[1]);
  /* 7 */
  FN(copy)(Tab[1], Tab[2]);
  FN(sqrn)(Tab[1], 10);
  FN(sqrn)(Tab[1], 50);
  FN(mul)(Tab[1], Tab[1], Tab[2]);
  /* 8 */
  FN(sqrn)(Tab[1], 5);
  FN(mul)(Tab[1], Tab[1], Tab[0]);
  /* 9 */
  FN(copy)(Tab[2], Tab[1]);
  FN(sqrn)(Tab[2], 100);
  FN(sqrn)(Tab[2], 20);
  FN(sqrn)(Tab[2], 5);
  FN(mul)(Tab[2], Tab[2], Tab[1]);
  /* 10 */
  FN(sqrn)(Tab[2], 2);
  FN(mul)(Tab[2], Tab[2], Tab[3]);

  FN(mul)(uv_p38, sqrt, uv3);
  /**
   * Checking whether
   *      v*B^2 == -u
   **/
  FN(copy)(uv3, uv_p38);
  FN(sqr)(uv3);        /*   B^2 */
  FN(mul)(uv, uv3, v); /* v*B^2 */
  FN(neg)(u);

  if (FN(cmp)(uv, u) == 0) {
    FN(mul)(uv_p38, uv_p38, (argElement_1w) sqrt_minus_one);/* (√-1)*B */
  }
}

DECL(void, srt)(argElement_1w c, argElement_1w a) {
  FN(invsqrt)(c, c, a);
}

DECL(void, rand)(argElement_1w a) {
  ALIGN uint8_t a_bytes[SIZE_FP25519];
  random_bytes(a_bytes, SIZE_FP25519);
  FN(unser)(a, a_bytes);
}

DECL(void, print)(FILE *file, argElement_1w a) {
  ALIGN uint8_t a_bytes[SIZE_FP25519];
  FN(ser)(a_bytes, a);
  print_hex_bytes(file, a_bytes, SIZE_FP25519);
}

DECL(argElement_1w, alloc)() {
  return (argElement_1w) allocate_bytes((4 * ((NUM_DIGITS_FP25519 + 3) / 4)) * sizeof(uint64_t));
}

DECL(void, prime)(argElement_1w a) {
  a[0] = 0x3ffffed;
  a[1] = 0x1ffffff;
  a[2] = 0x1ffffff;
  a[3] = 0x3ffffff;
  a[4] = 0x3ffffff;
  a[5] = 0x1ffffff;
  a[6] = 0x1ffffff;
  a[7] = 0x3ffffff;
  a[8] = 0x3ffffff;
  a[9] = 0x1ffffff;
}

DECL(void, zero)(argElement_1w a) {
  STORE(a + 0, ZERO);
  STORE(a + 1, ZERO);
  STORE(a + 2, ZERO);
}

DECL(void, naddsub)(argElement_1w pC, argElement_1w pD, argElement_1w pA, argElement_1w pB) {
  const __m256i _2P[3] = {
      SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7ffffda),/* [ h6, h1, h5, h0] */
      SET64(0x7fffffe, 0x3fffffe, 0x3fffffe, 0x7fffffe),/* [ h8, h3, h7, h2] */
      SET64(0x0000000, 0x0000000, 0x3fffffe, 0x7fffffe) /* [ xx, xx, h9, h4] */
  };
  int i = 0;
  for (i = 0; i < 3; i++) {
    __m256i A = LOAD(pA + i);
    __m256i B = LOAD(pB + i);
    __m256i T = SUB(_2P[i], A);
    __m256i C = SUB(T, B);
    __m256i D = ADD(T, B);
    STORE(pC + i, C);
    STORE(pD + i, D);
  }
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP255 */
