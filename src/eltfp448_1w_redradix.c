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

#if defined(FP448)

#define FN(NAME) SUFFIX(NAME,1,redradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

DECL(void, compress)(argElement_1w A) {
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m128i mask = SET32(0, ones, 0, ones);

  __m128i c0 = LOAD128(A+0);
  __m128i c1 = LOAD128(A+1);
  __m128i c2 = LOAD128(A+2);
  __m128i c3 = LOAD128(A+3);
  __m128i c4 = LOAD128(A+4);
  __m128i c5 = LOAD128(A+5);
  __m128i c6 = LOAD128(A+6);
  __m128i c7 = LOAD128(A+7);

  __m128i h0_h8,  h1_h9,  h2_h10, h3_h11,
      h4_h12, h5_h13, h6_h14, h7_h15;

  h0_h8 = SHR128(c0, BASE_FP448);
  c0 = AND128(c0, mask);
  c1 = ADD128(c1, h0_h8);

  h1_h9 = SHR128(c1, BASE_FP448);
  c1 = AND128(c1, mask);
  c2 = ADD128(c2, h1_h9);

  h2_h10 = SHR128(c2, BASE_FP448);
  c2 = AND128(c2, mask);
  c3 = ADD128(c3, h2_h10);

  h3_h11 = SHR128(c3, BASE_FP448);
  c3 = AND128(c3, mask);
  c4 = ADD128(c4, h3_h11);

  h4_h12 = SHR128(c4, BASE_FP448);
  c4 = AND128(c4, mask);
  c5 = ADD128(c5, h4_h12);

  h5_h13 = SHR128(c5, BASE_FP448);
  c5 = AND128(c5, mask);
  c6 = ADD128(c6, h5_h13);

  h6_h14 = SHR128(c6, BASE_FP448);
  c6 = AND128(c6, mask);
  c7 = ADD128(c7, h6_h14);

  h7_h15 = SHR128(c7, BASE_FP448);
  c7 = AND128(c7, mask);

  /**
   * LOW  h7  ->  h0
   * HIGH h15 ->  h8
   **/
  c0 = ADD128(c0,BLEND32128(h7_h15,ZERO128,0x3));
  /**
   * LOW  h7   \/  h0
   * HIGH h15  /\  h8
   **/
  c0 = ADD128(c0,SHUF32128(h7_h15,0x4E));

  h0_h8 = SHR128(c0, BASE_FP448);
  c0 = AND128(c0, mask);
  c1 = ADD128(c1, h0_h8);

  STORE128(A+0,c0);
  STORE128(A+1,c1);
  STORE128(A+2,c2);
  STORE128(A+3,c3);
  STORE128(A+4,c4);
  STORE128(A+5,c5);
  STORE128(A+6,c6);
  STORE128(A+7,c7);
}

DECL(void, compressfast)(argElement_1w c) {
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask  = SET164(ones);

  __m256i A0,A1,A2,A3;
  __m256i l0,l1,l2,l3;
  __m256i m0,m1,m2,m3;
  __m256i _m0,_m1,_m2,_m3;
  __m256i h0,h1,h2,h3;
  __m256i c0,c1,c2,c3;

  A0 = LOAD(c+0);
  A1 = LOAD(c+1);
  A2 = LOAD(c+2);
  A3 = LOAD(c+3);

  l0 = AND(A0,mask);
  l1 = AND(A1,mask);
  l2 = AND(A2,mask);
  l3 = AND(A3,mask);

  m0 = SHR(A0,BASE_FP448);
  m1 = SHR(A1,BASE_FP448);
  m2 = SHR(A2,BASE_FP448);
  m3 = SHR(A3,BASE_FP448);

  m0 = AND(m0,mask);
  m1 = AND(m1,mask);
  m2 = AND(m2,mask);
  m3 = AND(m3,mask);

  h0 = SHR(A0,2*BASE_FP448);
  h1 = SHR(A1,2*BASE_FP448);
  h2 = SHR(A2,2*BASE_FP448);
  h3 = SHR(A3,2*BASE_FP448);

  _m3 = SHUF32(ADD(m3,SHR8(m3)),0x4E);
  h3 = SHUF32(ADD(h3,SHR8(h3)),0x4E);

  _m0 = PERM128(_m3,m0,0x21);
  _m1 = PERM128(m0,m1,0x21);
  _m2 = PERM128(m1,m2,0x21);
  _m3 = PERM128(m2,m3,0x21);

  c0 = ADD(l0,_m0);
  c1 = ADD(l1,_m1);
  c2 = ADD(l2,_m2);
  c3 = ADD(l3,_m3);

  c0 = ADD(c0,h3);
  c1 = ADD(c1,h0);
  c2 = ADD(c2,h1);
  c3 = ADD(c3,h2);

  STORE(c+0,c0);
  STORE(c+1,c1);
  STORE(c+2,c2);
  STORE(c+3,c3);
}

DECL(void, new_compressfast)(argElement_1w pA) {
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask = SET164(ones);

  __m256i A0, A1, A2, A3;
  __m256i l0, l1, l2, l3;
  __m256i m0, m1, m2, m3;
  __m256i _m0, _m1, _m2, _m3;
  __m256i c0, c1, c2, c3;

  A0 = LOAD(pA + 0);
  A1 = LOAD(pA + 1);
  A2 = LOAD(pA + 2);
  A3 = LOAD(pA + 3);

  l0 = AND(A0, mask);
  l1 = AND(A1, mask);
  l2 = AND(A2, mask);
  l3 = AND(A3, mask);

  m0 = SHR(A0, BASE_FP448);
  m1 = SHR(A1, BASE_FP448);
  m2 = SHR(A2, BASE_FP448);
  m3 = SHR(A3, BASE_FP448);

  _m3 = SHUF32(ADD(m3, SHR8(m3)), 0x4E);

  _m0 = PERM128(_m3, m0, 0x21);
  _m1 = PERM128(m0, m1, 0x21);
  _m2 = PERM128(m1, m2, 0x21);
  _m3 = PERM128(m2, m3, 0x21);

  c0 = ADD(l0, _m0);
  c1 = ADD(l1, _m1);
  c2 = ADD(l2, _m2);
  c3 = ADD(l3, _m3);

  STORE(pA + 0, c0);
  STORE(pA + 1, c1);
  STORE(pA + 2, c2);
  STORE(pA + 3, c3);
}

DECL(void, sqrn)(argElement_1w A, unsigned int times) {
  unsigned int i = 0;
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask  = SET164(ones);

  __m256i ai,aj,a2i,a2j;
  __m256i b0,b1,b2,b3,b4,b5,b6,b7;
  __m256i c0,c1,c2,c3,c4,c5,c6,c7;
  __m256i d4,d5,d6,d7;
  __m256i y0,y1,y2,y3;

  c0 = LOAD(A+0);
  c1 = LOAD(A+1);
  c2 = LOAD(A+2);
  c3 = LOAD(A+3);

  for(i=0;i<times;i++) {
    y0 = c0;
    y1 = c1;
    y2 = c2;
    y3 = c3;

    __m256i x0 = ADD(BLEND32(y0, ZERO, 0x33), SHUF32(y0, 0x4E));
    __m256i x1 = ADD(BLEND32(y1, ZERO, 0x33), SHUF32(y1, 0x4E));
    __m256i x2 = ADD(BLEND32(y2, ZERO, 0x33), SHUF32(y2, 0x4E));
    __m256i x3 = ADD(BLEND32(y3, ZERO, 0x33), SHUF32(y3, 0x4E));

    b0 = PERM128(y0,x0,0x20);
    b1 = PERM128(y0,x0,0x31);
    b2 = PERM128(y1,x1,0x20);
    b3 = PERM128(y1,x1,0x31);
    b4 = PERM128(y2,x2,0x20);
    b5 = PERM128(y2,x2,0x31);
    b6 = PERM128(y3,x3,0x20);
    b7 = PERM128(y3,x3,0x31);

    d7 = SHUF32(ADD(b7,SHR8(b7)),0x4E);

    ai = PERM64(y0, 0x50);
    aj = PERM64(y0, 0xFA);
    a2i = ADD(ai, ai);
    a2j = ADD(aj, aj);
    c0 = MUL(b0, ai);		c0 = ADD(c0, MUL(d7, a2j));
    c1 = MUL(b1, a2i);		/*c1 = ADD(c1,MUL(b0, a2j));*/
    c2 = MUL(b2, a2i);		c2 = ADD(c2, MUL(b1, aj));
    c3 = MUL(b3, a2i);		c3 = ADD(c3, MUL(b2, a2j));
    c4 = MUL(b4, a2i);		c4 = ADD(c4, MUL(b3, a2j));
    c5 = MUL(b5, a2i);		c5 = ADD(c5, MUL(b4, a2j));
    c6 = MUL(b6, a2i);		c6 = ADD(c6, MUL(b5, a2j));
    c7 = MUL(b7, a2i);		c7 = ADD(c7, MUL(b6, a2j));

    d6 = SHUF32(ADD(b6,SHR8(b6)),0x4E);
    d5 = SHUF32(ADD(b5,SHR8(b5)),0x4E);

    ai = PERM64(y1, 0x50);
    aj = PERM64(y1, 0xFA);
    a2i = ADD(ai, ai);
    a2j = ADD(aj, aj);
    c0 = ADD(c0, MUL(d6, a2i));		c0 = ADD(c0, MUL(d5, a2j));
    c1 = ADD(c1, MUL(d7, a2i));		c1 = ADD(c1, MUL(d6, a2j));
    /*c2 = ADD(c2,MUL(b0, ai));*/   c2 = ADD(c2, MUL(d7, a2j));
    /*c3 = ADD(c3,MUL(b1, ai));*/   /*c3 = ADD(c3,MUL(b0, aj));*/
    c4 = ADD(c4, MUL(b2, ai));      /*c4 = ADD(c4,MUL(b1, aj));*/
    c5 = ADD(c5, MUL(b3, a2i));     /*c5 = ADD(c5,MUL(b2, aj));*/
    c6 = ADD(c6, MUL(b4, a2i));		c6 = ADD(c6, MUL(b3, aj));
    c7 = ADD(c7, MUL(b5, a2i));		c7 = ADD(c7, MUL(b4, a2j));

    ai = PERM64(y3, 0x50);
    aj = PERM64(y3, 0xFA);
    a2i = ADD(ai, ai);
    /*c0 = ADD(c0,MUL(d2, ai));*/    /*c0 = ADD(c0,MUL(d1, aj));*/
    /*c1 = ADD(c1,MUL(d3, ai));*/    /*c1 = ADD(c1,MUL(d2, aj));*/
    /*c2 = ADD(c2,MUL(d4, ai));*/    /*c2 = ADD(c2,MUL(d3, aj));*/
    /*c3 = ADD(c3,MUL(d5, ai));*/    /*c3 = ADD(c3,MUL(d4, aj));*/
    c4 = ADD(c4, MUL(d6, ai));       /*c4 = ADD(c4,MUL(d5, aj));*/
    c5 = ADD(c5, MUL(d7, a2i));      /*c5 = ADD(c5,MUL(d6, aj));*/
    /*c6 = ADD(c6,MUL(b0, ai));*/    c6 = ADD(c6, MUL(d7, aj));
    /*c7 = ADD(c7,MUL(b1, ai));*/    /*c7 = ADD(c7,MUL(b0, aj));*/

    d4 = SHUF32(ADD(b4,SHR8(b4)),0x4E);

    ai = PERM64(y2, 0x50);
    aj = PERM64(y2, 0xFA);
    a2i = ADD(ai, ai);
    a2j = ADD(aj, aj);

    c0 = ADD(c0, MUL(d4, ai));      /*c0 = ADD(c0,MUL(d3, a2j));*/
    c1 = ADD(c1, MUL(d5, a2i));     /*c1 = ADD(c1,MUL(d4, a2j));*/
    c2 = ADD(c2, MUL(d6, a2i));		c2 = ADD(c2, MUL(d5, aj));
    c3 = ADD(c3, MUL(d7, a2i));		c3 = ADD(c3, MUL(d6, a2j));
    /*c4 = ADD(c4,MUL(b0, ai));*/   c4 = ADD(c4, MUL(d7, a2j));
    /*c5 = ADD(c5,MUL(b1, ai));*/   /*c5 = ADD(c5,MUL(b0, aj));*/
    /*c6 = ADD(c6,MUL(b2, ai));*/   /*c6 = ADD(c6,MUL(b1, aj));*/
    /*c7 = ADD(c7,MUL(b3, ai));*/   /*c7 = ADD(c7,MUL(b2, aj));*/

    __m128i p0 = ADD128(EXTR(c0,1),CAST256TO128(c0));
    __m128i p1 = ADD128(EXTR(c1,1),CAST256TO128(c1));
    __m128i p2 = ADD128(EXTR(c2,1),CAST256TO128(c2));
    __m128i p3 = ADD128(EXTR(c3,1),CAST256TO128(c3));
    __m128i p4 = ADD128(EXTR(c4,1),CAST256TO128(c4));
    __m128i p5 = ADD128(EXTR(c5,1),CAST256TO128(c5));
    __m128i p6 = ADD128(EXTR(c6,1),CAST256TO128(c6));
    __m128i p7 = ADD128(EXTR(c7,1),CAST256TO128(c7));

    c0 = INSR(CAST128TO256(p0),p1,1);
    c1 = INSR(CAST128TO256(p2),p3,1);
    c2 = INSR(CAST128TO256(p4),p5,1);
    c3 = INSR(CAST128TO256(p6),p7,1);

    ///////////////compress

    __m256i A0,A1,A2,A3;
    __m256i l0,l1,l2,l3;
    __m256i m0,m1,m2,m3;
    __m256i _m0,_m1,_m2,_m3;
    __m256i h0,h1,h2,h3;

    A0 = c0;
    A1 = c1;
    A2 = c2;
    A3 = c3;

    l0 = AND(A0,mask);
    l1 = AND(A1,mask);
    l2 = AND(A2,mask);
    l3 = AND(A3,mask);

    m0 = SHR(A0,BASE_FP448);
    m1 = SHR(A1,BASE_FP448);
    m2 = SHR(A2,BASE_FP448);
    m3 = SHR(A3,BASE_FP448);

    m0 = AND(m0,mask);
    m1 = AND(m1,mask);
    m2 = AND(m2,mask);
    m3 = AND(m3,mask);

    h0 = SHR(A0,2*BASE_FP448);
    h1 = SHR(A1,2*BASE_FP448);
    h2 = SHR(A2,2*BASE_FP448);
    h3 = SHR(A3,2*BASE_FP448);

    _m3 = SHUF32(ADD(m3,SHR8(m3)),0x4E);
    h3 = SHUF32(ADD(h3,SHR8(h3)),0x4E);

    _m0 = PERM128(_m3,m0,0x21);
    _m1 = PERM128(m0,m1,0x21);
    _m2 = PERM128(m1,m2,0x21);
    _m3 = PERM128(m2,m3,0x21);

    c0 = ADD(l0,_m0);
    c1 = ADD(l1,_m1);
    c2 = ADD(l2,_m2);
    c3 = ADD(l3,_m3);

    c0 = ADD(c0,h3);
    c1 = ADD(c1,h0);
    c2 = ADD(c2,h1);
    c3 = ADD(c3,h2);
  }
  STORE(A+0,c0);
  STORE(A+1,c1);
  STORE(A+2,c2);
  STORE(A+3,c3);
}

DECL(void, unser)(argElement_1w C, uint8_t *p8A) {
  int i;
  const uint64_t mask = (((uint64_t)1)<<BASE_FP448)-1;
  const argElement_1w A = (argElement_1w)p8A;

  C[0 ] =  A[0]>>0;
  C[2 ] = (A[0]>>BASE_FP448);
  C[4 ] = (A[0]>>(2*BASE_FP448)) | (A[1]<<8);

  C[6 ] = (A[1]>>20);
  C[8 ] = (A[1]>>(20+BASE_FP448)) | (A[2]<<16);

  C[10] = (A[2]>>12);
  C[12] = (A[2]>>(12+BASE_FP448)) | (A[3]<<24);

  C[14] = (A[3]>>4);
  C[1 ] = (A[3]>>(4+BASE_FP448));
  C[3 ] = (A[3]>>(4+2*BASE_FP448)) | (A[4]<<4);

  C[5 ] = (A[4]>>24);
  C[7 ] = (A[4]>>(24+BASE_FP448)) | (A[5]<<12);

  C[9 ] = (A[5]>>16);
  C[11] = (A[5]>>(16+BASE_FP448)) | (A[6]<<20);

  C[13] = (A[6]>>8);
  C[15] = (A[6]>>(8+BASE_FP448));

  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
    C[i] &= mask;
  }
}

DECL(void, ser)(uint8_t *buf, argElement_1w puA) {
  EltFp448_1w_fullradix pC;
  /**
	* It only accepts positive coefficients.
	* In order to remove negative coefficients, compute compress()
	*/
  int i;
  int64_t *pA = (int64_t*)puA;
  __int128_t a[NUM_DIGITS_FP448];
  __int128_t c[NUM_DIGITS_FP448_X64];
  int64_t * p64C = (int64_t*)pC;
  for(i=0;i< NUM_DIGITS_FP448;i++)
  {
    a[i] = (__int128_t)pA[i];
  }

  __int128_t tmp = a[15]>>BASE_FP448;
  a[15] &= (((__int128_t)1<<BASE_FP448)-1);

  a[0] += tmp;
  a[1] += tmp;

  c[0] =              (a[4 ]<<(2*BASE_FP448))   + (a[2 ]<<BASE_FP448)     + (a[0]) ;
  c[1] = (c[0]>>64) + (a[8 ]<<(20+BASE_FP448))  + (a[6 ]<<20);
  c[2] = (c[1]>>64) + (a[12]<<(12+BASE_FP448))  + (a[10]<<12);
  c[3] = (c[2]>>64) + (a[3 ]<<(4+2*BASE_FP448)) + (a[1 ]<<(4+BASE_FP448)) + (a[14]<<4);
  c[4] = (c[3]>>64) + (a[7 ]<<(24+BASE_FP448))  + (a[5 ]<<24);
  c[5] = (c[4]>>64) + (a[11]<<(16+BASE_FP448))  + (a[9 ]<<16);
  c[6] = (c[5]>>64) + (a[15]<<(8+BASE_FP448))   + (a[13]<<8);

  tmp = (c[6]>>64);
  c[0] += tmp;
  c[3] += tmp<<(4+BASE_FP448);

  for(i=0;i<NUM_DIGITS_FP448_X64;i++)
  {
    p64C[i]=c[i];
  }
  ser_Fp448_1w_fullradix(buf,pC);
}

DECL(int, cmp)(argElement_1w a, argElement_1w b) {
  ALIGN uint8_t a_bytes[SIZE_FP448];
  ALIGN uint8_t b_bytes[SIZE_FP448];
  FN(ser)(a_bytes, a);
  FN(ser)(b_bytes, b);
  return cmp_bytes(a_bytes, b_bytes, SIZE_FP448);
}

DECL(void, add)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __m256i A0 = LOAD(a+0);
  __m256i A1 = LOAD(a+1);
  __m256i A2 = LOAD(a+2);
  __m256i A3 = LOAD(a+3);

  __m256i B0 = LOAD(b+0);
  __m256i B1 = LOAD(b+1);
  __m256i B2 = LOAD(b+2);
  __m256i B3 = LOAD(b+3);

  __m256i C0 = ADD(A0,B0);
  __m256i C1 = ADD(A1,B1);
  __m256i C2 = ADD(A2,B2);
  __m256i C3 = ADD(A3,B3);

  STORE(c+0,C0);
  STORE(c+1,C1);
  STORE(c+2,C2);
  STORE(c+3,C3);
}

static const EltFp448_1w_redradix CONST_2P_ELEMENT = {
    0x1ffffffe,0x3ffffffc,
    0x1ffffffe,0x1ffffffc,
    0x1ffffffe,0x1ffffffe,
    0x1ffffffe,0x1ffffffe,
    0x1ffffffe,0x1ffffffe,
    0x1ffffffe,0x1ffffffe,
    0x1ffffffe,0x1ffffffe,
    0x1ffffffe,0x1ffffffe
};

DECL(void, sub)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __m256i A0 = LOAD(a+0);
  __m256i A1 = LOAD(a+1);
  __m256i A2 = LOAD(a+2);
  __m256i A3 = LOAD(a+3);

  __m256i B0 = LOAD(b+0);
  __m256i B1 = LOAD(b+1);
  __m256i B2 = LOAD(b+2);
  __m256i B3 = LOAD(b+3);

  __m256i _2P0 = LOAD(CONST_2P_ELEMENT+0);
  __m256i _2P1 = LOAD(CONST_2P_ELEMENT+1);
  __m256i _2P2 = LOAD(CONST_2P_ELEMENT+2);
  __m256i _2P3 = LOAD(CONST_2P_ELEMENT+3);

  STORE(c+0,ADD(A0,SUB(_2P0,B0)));
  STORE(c+1,ADD(A1,SUB(_2P1,B1)));
  STORE(c+2,ADD(A2,SUB(_2P2,B2)));
  STORE(c+3,ADD(A3,SUB(_2P3,B3)));
}

DECL(void, addsub)(argElement_1w a, argElement_1w b) {
  __m256i* _2P = (__m256i*)CONST_2P_ELEMENT;
  __m256i A0 = LOAD(a+0);
  __m256i A1 = LOAD(a+1);
  __m256i A2 = LOAD(a+2);
  __m256i A3 = LOAD(a+3);
  __m256i B0 = LOAD(b+0);
  __m256i B1 = LOAD(b+1);
  __m256i B2 = LOAD(b+2);
  __m256i B3 = LOAD(b+3);

  __m256i C0 = ADD(A0,B0);
  __m256i C1 = ADD(A1,B1);
  __m256i C2 = ADD(A2,B2);
  __m256i C3 = ADD(A3,B3);

  __m256i D0 = ADD(A0,SUB(_2P[0],B0));
  __m256i D1 = ADD(A1,SUB(_2P[1],B1));
  __m256i D2 = ADD(A2,SUB(_2P[2],B2));
  __m256i D3 = ADD(A3,SUB(_2P[3],B3));

  STORE(a+0,C0);
  STORE(a+1,C1);
  STORE(a+2,C2);
  STORE(a+3,C3);

  STORE(b+0,D0);
  STORE(b+1,D1);
  STORE(b+2,D2);
  STORE(b+3,D3);
}

DECL(void, neg)(argElement_1w c) {
  __m256i A0 = LOAD(c+0);
  __m256i A1 = LOAD(c+1);
  __m256i A2 = LOAD(c+2);
  __m256i A3 = LOAD(c+3);

  __m256i _2P0 = LOAD(CONST_2P_ELEMENT+0);
  __m256i _2P1 = LOAD(CONST_2P_ELEMENT+1);
  __m256i _2P2 = LOAD(CONST_2P_ELEMENT+2);
  __m256i _2P3 = LOAD(CONST_2P_ELEMENT+3);

  STORE(c+0,SUB(_2P0,A0));
  STORE(c+1,SUB(_2P1,A1));
  STORE(c+2,SUB(_2P2,A2));
  STORE(c+3,SUB(_2P3,A3));
}

DECL(void, intmul_sb)(argElement_1w C, argElement_1w A, argElement_1w B) {
  int i;
  __m256i ai,tmp;
  __m256i b0,b1,b2,b3,b4,b5,b6,b7;
  __m256i c0,c1,c2,c3,c4,c5,c6,c7;

  __m256i y0 = LOAD(B+0);
  __m256i y1 = LOAD(B+1);
  __m256i y2 = LOAD(B+2);
  __m256i y3 = LOAD(B+3);

  __m256i x0 = ADD(BLEND32(y0, ZERO, 0x33), SHUF32(y0, 0x4E));
  __m256i x1 = ADD(BLEND32(y1, ZERO, 0x33), SHUF32(y1, 0x4E));
  __m256i x2 = ADD(BLEND32(y2, ZERO, 0x33), SHUF32(y2, 0x4E));
  __m256i x3 = ADD(BLEND32(y3, ZERO, 0x33), SHUF32(y3, 0x4E));

  b0 = PERM128(y0,x0,0x20);
  b1 = PERM128(y0,x0,0x31);
  b2 = PERM128(y1,x1,0x20);
  b3 = PERM128(y1,x1,0x31);
  b4 = PERM128(y2,x2,0x20);
  b5 = PERM128(y2,x2,0x31);
  b6 = PERM128(y3,x3,0x20);
  b7 = PERM128(y3,x3,0x31);

  c0 = ZERO;
  c1 = ZERO;
  c2 = ZERO;
  c3 = ZERO;
  c4 = ZERO;
  c5 = ZERO;
  c6 = ZERO;
  c7 = ZERO;

  for(i=0;i<4;i++)
  {
    ai = PERM64(LOAD(A+i),0x50);
    c0 = ADD(c0,MUL(b0, ai));
    c1 = ADD(c1,MUL(b1, ai));
    c2 = ADD(c2,MUL(b2, ai));
    c3 = ADD(c3,MUL(b3, ai));
    c4 = ADD(c4,MUL(b4, ai));
    c5 = ADD(c5,MUL(b5, ai));
    c6 = ADD(c6,MUL(b6, ai));
    c7 = ADD(c7,MUL(b7, ai));

    tmp= b7;
    b7 = b6;
    b6 = b5;
    b5 = b4;
    b4 = b3;
    b3 = b2;
    b2 = b1;
    b1 = b0;
    b0 = ADD(BLEND32(tmp, ZERO, 0x33), SHUF32(tmp, 0x4E));

    ai = PERM64(LOAD(A+i),0xFA);
    c0 = ADD(c0,MUL(b0, ai));
    c1 = ADD(c1,MUL(b1, ai));
    c2 = ADD(c2,MUL(b2, ai));
    c3 = ADD(c3,MUL(b3, ai));
    c4 = ADD(c4,MUL(b4, ai));
    c5 = ADD(c5,MUL(b5, ai));
    c6 = ADD(c6,MUL(b6, ai));
    c7 = ADD(c7,MUL(b7, ai));

    tmp= b7;
    b7 = b6;
    b6 = b5;
    b5 = b4;
    b4 = b3;
    b3 = b2;
    b2 = b1;
    b1 = b0;
    b0 = ADD(BLEND32(tmp, ZERO, 0x33), SHUF32(tmp, 0x4E));
  }

  __m128i p0 = ADD128(EXTR(c0,1),CAST256TO128(c0));
  __m128i p1 = ADD128(EXTR(c1,1),CAST256TO128(c1));
  __m128i p2 = ADD128(EXTR(c2,1),CAST256TO128(c2));
  __m128i p3 = ADD128(EXTR(c3,1),CAST256TO128(c3));
  __m128i p4 = ADD128(EXTR(c4,1),CAST256TO128(c4));
  __m128i p5 = ADD128(EXTR(c5,1),CAST256TO128(c5));
  __m128i p6 = ADD128(EXTR(c6,1),CAST256TO128(c6));
  __m128i p7 = ADD128(EXTR(c7,1),CAST256TO128(c7));

//	c0 = _mm256_insertf128_si256(_mm256_castsi128_si256(p0),p1,1);
//	c1 = _mm256_insertf128_si256(_mm256_castsi128_si256(p2),p3,1);
//	c2 = _mm256_insertf128_si256(_mm256_castsi128_si256(p4),p5,1);
//	c3 = _mm256_insertf128_si256(_mm256_castsi128_si256(p6),p7,1);

  STORE128(C+0,p0);
  STORE128(C+1,p1);
  STORE128(C+2,p2);
  STORE128(C+3,p3);
  STORE128(C+4,p4);
  STORE128(C+5,p5);
  STORE128(C+6,p6);
  STORE128(C+7,p7);
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
  STORE(c + 3, LOAD(a + 3));
}

DECL(void, invsqrt)(argElement_1w c, argElement_1w a, const int only_inverse) {
  EltFp448_1w_redradix x0, x1;
  argElement_1w T[4];

  T[0] = x0;
  T[1] = c;
  T[2] = x1;
  T[3] = a;

  FN(copy)(T[1], T[3]);
  FN(sqrn)(T[1], 1);
  FN(mul)(T[1], T[1], T[3]);

  FN(copy)(T[0], T[1]);
  FN(sqrn)(T[0], 1);
  FN(mul)(T[0], T[0], T[3]);

  FN(copy)(T[1], T[0]);
  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 6);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 12);
  FN(mul)(T[1], T[1], T[2]);

  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 27);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 54);
  FN(mul)(T[1], T[1], T[2]);

  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 111);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 1);
  FN(mul)(T[1], T[1], T[3]);

  FN(sqrn)(T[1], 223);
  FN(mul)(T[1], T[1], T[2]);

  if (only_inverse != 0) {
    FN(sqrn)(T[1], 2);
    FN(mul)(T[1], T[1], T[3]);
  }
}

DECL(void, inv)(argElement_1w c, argElement_1w a) {
  FN(invsqrt)(c,a,1);
}

DECL(void, srt)(argElement_1w c, argElement_1w a) {
  FN(invsqrt)(c,a,0);
}

DECL(void, rand)(argElement_1w a) {
  ALIGN uint8_t a_bytes[SIZE_FP448];
  random_bytes(a_bytes, SIZE_FP448);
  FN(unser)(a, a_bytes);
}

DECL(void, print)(FILE *file, argElement_1w a) {
  ALIGN uint8_t a_bytes[SIZE_FP448];
  FN(ser)(a_bytes, a);
  print_hex_bytes(file, a_bytes, SIZE_FP448);
}

DECL(argElement_1w, alloc)() {
  return (argElement_1w) allocate_bytes( (NUM_DIGITS_FP448) * sizeof(uint64_t));
}

DECL(void, prime)(argElement_1w a) {
  a[0] = 0xfffffff;
  a[1] = 0xfffffff;
  a[2] = 0xfffffff;
  a[3] = 0xfffffff;
  a[4] = 0xfffffff;
  a[5] = 0xfffffff;
  a[6] = 0xfffffff;
  a[7] = 0xfffffff;
  a[8] = 0xffffffe;
  a[9] = 0xfffffff;
  a[10] = 0xfffffff;
  a[11] = 0xfffffff;
  a[12] = 0xfffffff;
  a[13] = 0xfffffff;
  a[14] = 0xfffffff;
  a[15] = 0xfffffff;
}

DECL(void, zero)(argElement_1w a) {
  STORE(a + 0, ZERO);
  STORE(a + 1, ZERO);
  STORE(a + 2, ZERO);
  STORE(a + 3, ZERO);
}

DECL(void, naddsub)(argElement_1w pC, argElement_1w pD, argElement_1w pA, argElement_1w pB) {
  __m256i* _2P = (__m256i*)CONST_2P_ELEMENT;
  __m256i A0 = LOAD(pA+0);
  __m256i A1 = LOAD(pA+1);
  __m256i A2 = LOAD(pA+2);
  __m256i A3 = LOAD(pA+3);

  __m256i B0 = LOAD(pB+0);
  __m256i B1 = LOAD(pB+1);
  __m256i B2 = LOAD(pB+2);
  __m256i B3 = LOAD(pB+3);

  __m256i T0 = SUB(_2P[0],A0);
  __m256i T1 = SUB(_2P[1],A1);
  __m256i T2 = SUB(_2P[2],A2);
  __m256i T3 = SUB(_2P[3],A3);

  __m256i C0 = SUB(T0,B0);
  __m256i C1 = SUB(T1,B1);
  __m256i C2 = SUB(T2,B2);
  __m256i C3 = SUB(T3,B3);

  __m256i D0 = ADD(T0,B0);
  __m256i D1 = ADD(T1,B1);
  __m256i D2 = ADD(T2,B2);
  __m256i D3 = ADD(T3,B3);

  STORE(pC+0,C0);
  STORE(pC+1,C1);
  STORE(pC+2,C2);
  STORE(pC+3,C3);

  STORE(pD+0,D0);
  STORE(pD+1,D1);
  STORE(pD+2,D2);
  STORE(pD+3,D3);
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP448 */
