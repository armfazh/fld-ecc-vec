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

#define FN(NAME) SUFFIX(NAME,4,redradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

/**
 *
 * @param A
 * @param nA
 */
static inline void str_bytes_to_Element_4w_h0h8(argElement_4w A, __m256i nA[7]) {
  int i;
  const uint64_t ones = (((uint64_t)1)<<BASE_FP448)-1;
  const __m256i mask = SET164(ones);

  A[0]  = nA[0];
  A[2]  = SHR(nA[0],BASE_FP448);
  A[4]  = XOR(SHR(nA[0],2*BASE_FP448),SHL(nA[1],8));

  A[6]  = SHR(nA[1],20);
  A[8]  = XOR(SHR(nA[1],20+BASE_FP448),SHL(nA[2],16));

  A[10] = SHR(nA[2],12);
  A[12] = XOR(SHR(nA[2],12+BASE_FP448),SHL(nA[3],24));

  A[14] = SHR(nA[3],4);
  A[1]  = SHR(nA[3],4+BASE_FP448);
  A[3]  = XOR(SHR(nA[3],4+2*BASE_FP448),SHL(nA[4],4));

  A[5]  = SHR(nA[4],24);
  A[7]  = XOR(SHR(nA[4],24+BASE_FP448),SHL(nA[5],12));

  A[9]  = SHR(nA[5],16);
  A[11] = XOR(SHR(nA[5],16+BASE_FP448),SHL(nA[6],20));

  A[13] = SHR(nA[6],8);
  A[15] = SHR(nA[6],8+BASE_FP448);

  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
    A[i] = AND(A[i],mask);
  }
}

DECL(void, copy)(argElement_4w c, argElement_4w a) {
  int i = 0;
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    c[i] = a[i];
  }
}

DECL(void, zero)(argElement_4w c) {
  int i = 0;
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    c[i] = ZERO;
  }
}

DECL(void,zip)(argElement_4w A,argElement_1w pA,argElement_1w pB,argElement_1w pC,argElement_1w pD) {
  __m256i A0 = LOAD(pA+0);
  __m256i B0 = LOAD(pB+0);
  __m256i C0 = LOAD(pC+0);
  __m256i D0 = LOAD(pD+0);

  __m256i AC0 = PERM128(A0,C0,0x20);
  __m256i BD0 = PERM128(B0,D0,0x20);
  __m256i AC2 = PERM128(A0,C0,0x31);
  __m256i BD2 = PERM128(B0,D0,0x31);
  A[0] = UPKL64(AC0,BD0);
  A[1] = UPKH64(AC0,BD0);
  A[2] = UPKL64(AC2,BD2);
  A[3] = UPKH64(AC2,BD2);

  __m256i A4 = LOAD(pA+1);
  __m256i B4 = LOAD(pB+1);
  __m256i C4 = LOAD(pC+1);
  __m256i D4 = LOAD(pD+1);

  __m256i AC4 = PERM128(A4,C4,0x20);
  __m256i BD4 = PERM128(B4,D4,0x20);
  __m256i AC6 = PERM128(A4,C4,0x31);
  __m256i BD6 = PERM128(B4,D4,0x31);
  A[4] = UPKL64(AC4,BD4);
  A[5] = UPKH64(AC4,BD4);
  A[6] = UPKL64(AC6,BD6);
  A[7] = UPKH64(AC6,BD6);

  __m256i A8 = LOAD(pA+2);
  __m256i B8 = LOAD(pB+2);
  __m256i C8 = LOAD(pC+2);
  __m256i D8 = LOAD(pD+2);

  __m256i AC8  = PERM128(A8,C8,0x20);
  __m256i BD8  = PERM128(B8,D8,0x20);
  __m256i AC10 = PERM128(A8,C8,0x31);
  __m256i BD10 = PERM128(B8,D8,0x31);
  A[8] = UPKL64(AC8,BD8);
  A[9] = UPKH64(AC8,BD8);
  A[10] = UPKL64(AC10,BD10);
  A[11] = UPKH64(AC10,BD10);

  __m256i A12 = LOAD(pA+3);
  __m256i B12 = LOAD(pB+3);
  __m256i C12 = LOAD(pC+3);
  __m256i D12 = LOAD(pD+3);

  __m256i AC12 = PERM128(A12,C12,0x20);
  __m256i BD12 = PERM128(B12,D12,0x20);
  __m256i AC14 = PERM128(A12,C12,0x31);
  __m256i BD14 = PERM128(B12,D12,0x31);
  A[12] = UPKL64(AC12,BD12);
  A[13] = UPKH64(AC12,BD12);
  A[14] = UPKL64(AC14,BD14);
  A[15] = UPKH64(AC14,BD14);
}

DECL(void,unzip)(argElement_1w pA,argElement_1w pB,argElement_1w pC,argElement_1w pD,argElement_4w A) {
  __m256i A02 = PERM128(A[0],A[2],0x20);
  __m256i A13 = PERM128(A[1],A[3],0x20);
  __m256i C02 = PERM128(A[0],A[2],0x31);
  __m256i C13 = PERM128(A[1],A[3],0x31);
  __m256i A0 = UPKL64(A02,A13);
  __m256i B0 = UPKH64(A02,A13);
  __m256i C0 = UPKL64(C02,C13);
  __m256i D0 = UPKH64(C02,C13);

  STORE(pA+0,A0);
  STORE(pB+0,B0);
  STORE(pC+0,C0);
  STORE(pD+0,D0);

  __m256i A46 = PERM128(A[4],A[6],0x20);
  __m256i A57 = PERM128(A[5],A[7],0x20);
  __m256i C46 = PERM128(A[4],A[6],0x31);
  __m256i C57 = PERM128(A[5],A[7],0x31);
  __m256i A4 = UPKL64(A46,A57);
  __m256i B4 = UPKH64(A46,A57);
  __m256i C4 = UPKL64(C46,C57);
  __m256i D4 = UPKH64(C46,C57);

  STORE(pA+1,A4);
  STORE(pB+1,B4);
  STORE(pC+1,C4);
  STORE(pD+1,D4);

  __m256i A810 = PERM128(A[8],A[10],0x20);
  __m256i A911 = PERM128(A[9],A[11],0x20);
  __m256i C810 = PERM128(A[8],A[10],0x31);
  __m256i C911 = PERM128(A[9],A[11],0x31);
  __m256i A8 = UPKL64(A810,A911);
  __m256i B8 = UPKH64(A810,A911);
  __m256i C8 = UPKL64(C810,C911);
  __m256i D8 = UPKH64(C810,C911);

  STORE(pA+2,A8);
  STORE(pB+2,B8);
  STORE(pC+2,C8);
  STORE(pD+2,D8);

  __m256i A1214 = PERM128(A[12],A[14],0x20);
  __m256i A1315 = PERM128(A[13],A[15],0x20);
  __m256i C1214 = PERM128(A[12],A[14],0x31);
  __m256i C1315 = PERM128(A[13],A[15],0x31);
  __m256i A12 = UPKL64(A1214,A1315);
  __m256i B12 = UPKH64(A1214,A1315);
  __m256i C12 = UPKL64(C1214,C1315);
  __m256i D12 = UPKH64(C1214,C1315);

  STORE(pA+3,A12);
  STORE(pB+3,B12);
  STORE(pC+3,C12);
  STORE(pD+3,D12);
}

DECL(void, add)(argElement_4w  C, argElement_4w  A, argElement_4w  B) {
	int i=0;
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		C[i] = ADD(A[i], B[i]);
	}
}

DECL(void, sub)(argElement_4w  C, argElement_4w  A, argElement_4w  B) {
  const __m256i _2P[NUM_DIGITS_FP448] = {
      SET164(0x1ffffffe),	SET164(0x3ffffffc),
      SET164(0x1ffffffe),	SET164(0x1ffffffc),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe)
  };
	int i=0;
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		C[i] = ADD(A[i],SUB(_2P[i],B[i]));
	}
}

DECL(void, addsub)(argElement_4w  C,argElement_4w  D, argElement_4w  A, argElement_4w  B) {
  const __m256i _2P[NUM_DIGITS_FP448] = {
      SET164(0x1ffffffe),	SET164(0x3ffffffc),
      SET164(0x1ffffffe),	SET164(0x1ffffffc),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe),
      SET164(0x1ffffffe),	SET164(0x1ffffffe)
  };
	int i=0;
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		C[i] = ADD(A[i],B[i]);
		D[i] = ADD(A[i],SUB(_2P[i],B[i]));
	}
}

DECL(void, addsublar)(argElement_4w  A, argElement_4w  B) {
  const __m256i _2_to_34P[NUM_DIGITS_FP448] = {
      SET164(0x3ffffffc00000000),	SET164(0x3ffffff800000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000),	SET164(0x3ffffffc00000000)
  };
  int i=0;
  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
      __m256i add = ADD(A[i],B[i]);
      __m256i sub = ADD(A[i],SUB(_2_to_34P[i], B[i]));
      A[i] = add;
      B[i] = sub;
  }
}

DECL(void, ngz)(argElement_4w c, argElement_4w a) {
  const uint64_t CONST_00_00_2P_00_H0H8[4*NUM_DIGITS_FP448] = {
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x3ffffffc,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffc,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000,
      0x0000000,0x0000000,0x1ffffffe,0x0000000
  };
  const __m256i mask_subadd = SET64(0,-1,0,0);
  argElement_4w _00_00_2P_00 = (argElement_4w)CONST_00_00_2P_00_H0H8;
  int i=0;
  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
    __m256i YXZT = SHUF32(a[i], 0x4E);
    __m256i XY_TZ = ADD(_00_00_2P_00[i],SUB(XOR(a[i],mask_subadd),mask_subadd));
    c[i] = BLEND32(YXZT,XY_TZ,0xF0);
  }
}

#define mul_school_4x4(r0,r1,r2,r3,r4,r5,r6,s0,s1,s2,s3,t0,t1,t2,t3) \
do{                           \
	r0 = MUL(s0,t0);          \
	r1 = MUL(s1,t0);          \
	r2 = MUL(s2,t0);          \
	r3 = MUL(s3,t0);          \
                              \
	r1 = ADD(r1,MUL(s0,t1));  \
	r2 = ADD(r2,MUL(s1,t1));  \
	r3 = ADD(r3,MUL(s2,t1));  \
	r4 =        MUL(s3,t1);   \
	                          \
	r2 = ADD(r2,MUL(s0,t2));  \
	r3 = ADD(r3,MUL(s1,t2));  \
	r4 = ADD(r4,MUL(s2,t2));  \
	r5 =        MUL(s3,t2);   \
	                          \
	r3 = ADD(r3,MUL(s0,t3));  \
	r4 = ADD(r4,MUL(s1,t3));  \
	r5 = ADD(r5,MUL(s2,t3));  \
	r6 =        MUL(s3,t3);   \
}while(0)

#define mul_karat_8x8(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,h13,h14, \
               f0,f1,f2,f3,f4,f5,f6,f7,g0,g1,g2,g3,g4,g5,g6,g7)          \
do{                                                                      \
	__m256i o0,o1,o2,o3,o4,o5,o6;                                        \
	__m256i p0,p1,p2,p3,p4,p5,p6;                                        \
	__m256i q0,q1,q2,q3,q4,q5,q6;                                        \
                                                                         \
	mul_school_4x4(o0,o1,o2,o3,o4,o5,o6,f0,f1,f2,f3,g0,g1,g2,g3);        \
	mul_school_4x4(p0,p1,p2,p3,p4,p5,p6,f4,f5,f6,f7,g4,g5,g6,g7);        \
	mul_school_4x4(q0,q1,q2,q3,q4,q5,q6,                                 \
		ADD(f0,f4),                                                      \
		ADD(f1,f5),                                                      \
		ADD(f2,f6),                                                      \
		ADD(f3,f7),                                                      \
		ADD(g0,g4),                                                      \
		ADD(g1,g5),                                                      \
		ADD(g2,g6),                                                      \
		ADD(g3,g7));                                                     \
    __m256i ref0 = SUB(o4,p0);                                           \
    __m256i ref1 = SUB(o5,p1);                                           \
    __m256i ref2 = SUB(o6,p2);                                           \
	h0 = o0;                         h8  = SUB(SUB(q4,p4),ref0);         \
	h1 = o1;                         h9  = SUB(SUB(q5,p5),ref1);         \
	h2 = o2;                         h10 = SUB(SUB(q6,p6),ref2);         \
	h3 = o3;                         h11 = p3;                           \
	h4 = ADD(ref0,SUB(q0,o0));       h12 = p4;                           \
	h5 = ADD(ref1,SUB(q1,o1));       h13 = p5;                           \
	h6 = ADD(ref2,SUB(q2,o2));       h14 = p6;                           \
	h7 = SUB(SUB(q3,o3),p3);                                             \
}while(0)

DECL(void, intmul_karat)(argElement_4w C, argElement_4w A, argElement_4w B) {
  __m256i a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15;
  __m256i b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15;
  __m256i x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14;
  __m256i y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14;
  __m256i z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14;

  a0 = A[0];	a8  = A[1];     b0 = B[0];	b8  = B[1];
  a1 = A[2];	a9  = A[3];     b1 = B[2];	b9  = B[3];
  a2 = A[4];	a10 = A[5];     b2 = B[4];	b10 = B[5];
  a3 = A[6];	a11 = A[7];     b3 = B[6];	b11 = B[7];
  a4 = A[8];	a12 = A[9];     b4 = B[8];	b12 = B[9];
  a5 = A[10];	a13 = A[11];    b5 = B[10];	b13 = B[11];
  a6 = A[12];	a14 = A[13];    b6 = B[12];	b14 = B[13];
  a7 = A[14];	a15 = A[15];    b7 = B[14];	b15 = B[15];

  mul_karat_8x8(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,
                a0,a1,a2,a3,a4,a5,a6,a7,b0,b1,b2,b3,b4,b5,b6,b7);
  mul_karat_8x8(y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14,
                a8,a9,a10,a11,a12,a13,a14,a15,
                b8,b9,b10,b11,b12,b13,b14,b15);
  mul_karat_8x8(z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14,
                ADD(a0,a8 ),
                ADD(a1,a9 ),
                ADD(a2,a10),
                ADD(a3,a11),
                ADD(a4,a12),
                ADD(a5,a13),
                ADD(a6,a14),
                ADD(a7,a15),
                ADD(b0,b8 ),
                ADD(b1,b9 ),
                ADD(b2,b10),
                ADD(b3,b11),
                ADD(b4,b12),
                ADD(b5,b13),
                ADD(b6,b14),
                ADD(b7,b15));

  C[0 ] = SUB(ADD(ADD(x0,y0),z8 ),x8 );	C[1 ] = SUB(ADD(ADD(y8 ,z0),z8 ),x0);
  C[2 ] = SUB(ADD(ADD(x1,y1),z9 ),x9 );	C[3 ] = SUB(ADD(ADD(y9 ,z1),z9 ),x1);
  C[4 ] = SUB(ADD(ADD(x2,y2),z10),x10);	C[5 ] = SUB(ADD(ADD(y10,z2),z10),x2);
  C[6 ] = SUB(ADD(ADD(x3,y3),z11),x11);	C[7 ] = SUB(ADD(ADD(y11,z3),z11),x3);
  C[8 ] = SUB(ADD(ADD(x4,y4),z12),x12);	C[9 ] = SUB(ADD(ADD(y12,z4),z12),x4);
  C[10] = SUB(ADD(ADD(x5,y5),z13),x13);	C[11] = SUB(ADD(ADD(y13,z5),z13),x5);
  C[12] = SUB(ADD(ADD(x6,y6),z14),x14);	C[13] = SUB(ADD(ADD(y14,z6),z14),x6);
  C[14] = ADD(x7,y7);	                C[15] = SUB(z7,x7);
}

DECL(void, intmul)(argElement_4w c, argElement_4w a, argElement_4w b) {
	FN(intmul_karat)(c,a,b);
}

#define sqr_school_4x4(r0,r1,r2,r3,r4,r5,r6,s0,s1,s2,s3) \
do{                                                      \
	__m256i t2 = MUL(s0,s2);                             \
	__m256i t4 = MUL(s1,s3);                             \
	r0 = MUL(s0,s0);                                     \
	r1 = MUL(s0,s1);                                     \
	r1 = ADD(r1,r1);                                     \
 	r2 = MUL(s1,s1);                                     \
 	r2 = ADD(r2,ADD(t2,t2));                             \
	r3 = MUL(s0,s3);                                     \
	r3 = ADD(r3,MUL(s1,s2));                             \
	r3 = ADD(r3,r3);                                     \
	r4 = MUL(s2,s2);                                     \
 	r4 = ADD(r4,ADD(t4,t4));                             \
	r5 = MUL(s2,s3);                                     \
	r5 = ADD(r5,r5);                                     \
	r6 = MUL(s3,s3);                                     \
}while(0)

#define sqr_karat_8x8(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,h13,h14, \
               f0,f1,f2,f3,f4,f5,f6,f7)                                  \
do{                                                                      \
	__m256i o0,o1,o2,o3,o4,o5,o6;                                        \
	__m256i p0,p1,p2,p3,p4,p5,p6;                                        \
	__m256i q0,q1,q2,q3,q4,q5,q6;                                        \
                                                                         \
	sqr_school_4x4(o0,o1,o2,o3,o4,o5,o6,f0,f1,f2,f3);                    \
	sqr_school_4x4(p0,p1,p2,p3,p4,p5,p6,f4,f5,f6,f7);                    \
	sqr_school_4x4(q0,q1,q2,q3,q4,q5,q6,                                 \
		ADD(f0,f4),                                                      \
		ADD(f1,f5),                                                      \
		ADD(f2,f6),                                                      \
		ADD(f3,f7));                                                     \
    __m256i ref0 = SUB(o4,p0);                                           \
    __m256i ref1 = SUB(o5,p1);                                           \
    __m256i ref2 = SUB(o6,p2);                                           \
	h0 = o0;                         h8  = SUB(SUB(q4,p4),ref0);         \
	h1 = o1;                         h9  = SUB(SUB(q5,p5),ref1);         \
	h2 = o2;                         h10 = SUB(SUB(q6,p6),ref2);         \
	h3 = o3;                         h11 = p3;                           \
	h4 = ADD(ref0,SUB(q0,o0));       h12 = p4;                           \
	h5 = ADD(ref1,SUB(q1,o1));       h13 = p5;                           \
	h6 = ADD(ref2,SUB(q2,o2));       h14 = p6;                           \
	h7 = SUB(SUB(q3,o3),p3);                                             \
}while(0)

DECL(void, intsqr_karat)(argElement_4w A) {
  __m256i a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15;
  __m256i x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14;
  __m256i y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14;
  __m256i z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14;

  a0 = A[0];	a8  = A[1];
  a1 = A[2];	a9  = A[3];
  a2 = A[4];	a10 = A[5];
  a3 = A[6];	a11 = A[7];
  a4 = A[8];	a12 = A[9];
  a5 = A[10];	a13 = A[11];
  a6 = A[12];	a14 = A[13];
  a7 = A[14];	a15 = A[15];

  sqr_karat_8x8(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,
                a0,a1,a2,a3,a4,a5,a6,a7);
  sqr_karat_8x8(y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14,
                a8,a9,a10,a11,a12,a13,a14,a15);
  sqr_karat_8x8(z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14,
                ADD(a0,a8 ),
                ADD(a1,a9 ),
                ADD(a2,a10),
                ADD(a3,a11),
                ADD(a4,a12),
                ADD(a5,a13),
                ADD(a6,a14),
                ADD(a7,a15));

  A[0 ] = SUB(ADD(ADD(x0,y0),z8 ),x8 );	A[1 ] = SUB(ADD(ADD(y8 ,z0),z8 ),x0);
  A[2 ] = SUB(ADD(ADD(x1,y1),z9 ),x9 );	A[3 ] = SUB(ADD(ADD(y9 ,z1),z9 ),x1);
  A[4 ] = SUB(ADD(ADD(x2,y2),z10),x10);	A[5 ] = SUB(ADD(ADD(y10,z2),z10),x2);
  A[6 ] = SUB(ADD(ADD(x3,y3),z11),x11);	A[7 ] = SUB(ADD(ADD(y11,z3),z11),x3);
  A[8 ] = SUB(ADD(ADD(x4,y4),z12),x12);	A[9 ] = SUB(ADD(ADD(y12,z4),z12),x4);
  A[10] = SUB(ADD(ADD(x5,y5),z13),x13);	A[11] = SUB(ADD(ADD(y13,z5),z13),x5);
  A[12] = SUB(ADD(ADD(x6,y6),z14),x14);	A[13] = SUB(ADD(ADD(y14,z6),z14),x6);
  A[14] = ADD(x7,y7);	                A[15] = SUB(z7,x7);
}

DECL(void, intsqr)(argElement_4w c) {
  FN(intsqr_karat)(c);
}

#undef mul_school_4x4
#undef mul_karat_8x8
#undef sqr_school_4x4
#undef sqr_karat_8x8

#define CARRY_WORD(X,H,I,J) \
	H = SHR(X[I], BASE_FP448);\
	X[I] = AND(X[I], mask);\
	X[J] = ADD(X[J], H);

DECL(void, compress)(argElement_4w C) {
  __m256i h,residue;
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask = SET164(ones);

  CARRY_WORD(C,h,0 ,2 )
  CARRY_WORD(C,h,2 ,4 )
  CARRY_WORD(C,h,4 ,6 )
  CARRY_WORD(C,h,6 ,8 )
  CARRY_WORD(C,h,8 ,10)
  CARRY_WORD(C,h,10,12)
  CARRY_WORD(C,h,12,14)
  CARRY_WORD(C,h,14,1 )
  CARRY_WORD(C,h,1 ,3 )
  CARRY_WORD(C,h,3 ,5 )
  CARRY_WORD(C,h,5 ,7 )
  CARRY_WORD(C,h,7 ,9 )
  CARRY_WORD(C,h,9 ,11)
  CARRY_WORD(C,h,11,13)
  CARRY_WORD(C,h,13,15)
  residue = SHR(C[15], BASE_FP448);
  C[15] = AND(C[15], mask);

  C[0] = ADD(C[0],residue);
  CARRY_WORD(C,h,0 ,2 )
  CARRY_WORD(C,h,2 ,4 )

  C[1] = ADD(C[1],residue);
  CARRY_WORD(C,h,1 ,3 )
  CARRY_WORD(C,h,3 ,5 )
}

DECL(void, compress2)(argElement_4w C,argElement_4w D) {
  __m256i h0,h1,residue0,residue1;
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask = SET164(ones);

  CARRY_WORD(C,h0,0 ,2 )                  CARRY_WORD(D,h1,0 ,2 )
  CARRY_WORD(C,h0,2 ,4 )                  CARRY_WORD(D,h1,2 ,4 )
  CARRY_WORD(C,h0,4 ,6 )                  CARRY_WORD(D,h1,4 ,6 )
  CARRY_WORD(C,h0,6 ,8 )                  CARRY_WORD(D,h1,6 ,8 )
  CARRY_WORD(C,h0,8 ,10)                  CARRY_WORD(D,h1,8 ,10)
  CARRY_WORD(C,h0,10,12)                  CARRY_WORD(D,h1,10,12)
  CARRY_WORD(C,h0,12,14)                  CARRY_WORD(D,h1,12,14)
  CARRY_WORD(C,h0,14,1 )                  CARRY_WORD(D,h1,14,1 )
  CARRY_WORD(C,h0,1 ,3 )                  CARRY_WORD(D,h1,1 ,3 )
  CARRY_WORD(C,h0,3 ,5 )                  CARRY_WORD(D,h1,3 ,5 )
  CARRY_WORD(C,h0,5 ,7 )                  CARRY_WORD(D,h1,5 ,7 )
  CARRY_WORD(C,h0,7 ,9 )                  CARRY_WORD(D,h1,7 ,9 )
  CARRY_WORD(C,h0,9 ,11)                  CARRY_WORD(D,h1,9 ,11)
  CARRY_WORD(C,h0,11,13)                  CARRY_WORD(D,h1,11,13)
  CARRY_WORD(C,h0,13,15)                  CARRY_WORD(D,h1,13,15)
  residue0 = SHR(C[15], BASE_FP448);       residue1 = SHR(D[15], BASE_FP448);
  C[15] = AND(C[15], mask);               D[15] = AND(D[15], mask);

  C[0] = ADD(C[0],residue0);              D[0] = ADD(D[0],residue1);
  CARRY_WORD(C,h0,0 ,2 )                  CARRY_WORD(D,h1,0 ,2 )
  CARRY_WORD(C,h0,2 ,4 )                  CARRY_WORD(D,h1,2 ,4 )

  C[1] = ADD(C[1],residue0);              D[1] = ADD(D[1],residue1);
  CARRY_WORD(C,h0,1 ,3 )                  CARRY_WORD(D,h1,1 ,3 )
  CARRY_WORD(C,h0,3 ,5 )                  CARRY_WORD(D,h1,3 ,5 )
}

DECL(void, compressfast)(argElement_4w C) {
  int i;
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask  = SET164(ones);

  EltFp448_4w_redradix M,H;

  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
    M[i] = SHR(C[i], BASE_FP448);
    M[i] = AND(M[i], mask);
    H[i] = SHR(C[i], 2*BASE_FP448);
    C[i] = AND(C[i], mask);
  }

  C[0 ] = ADD(C[0 ],ADD(M[15],H[13]));
  C[2 ] = ADD(C[2 ],ADD(M[0 ],H[15]));
  C[4 ] = ADD(C[4 ],ADD(M[2 ],H[0 ]));
  C[6 ] = ADD(C[6 ],ADD(M[4 ],H[2 ]));
  C[8 ] = ADD(C[8 ],ADD(M[6 ],H[4 ]));
  C[10] = ADD(C[10],ADD(M[8 ],H[6 ]));
  C[12] = ADD(C[12],ADD(M[10],H[8 ]));
  C[14] = ADD(C[14],ADD(M[12],H[10]));

  C[1 ] = ADD(C[1 ],ADD(M[15],H[13]));
  C[3 ] = ADD(C[3 ],H[15]);

  C[1 ] = ADD(C[1 ],ADD(M[14],H[12]));
  C[3 ] = ADD(C[3 ],ADD(M[1 ],H[14]));
  C[5 ] = ADD(C[5 ],ADD(M[3 ],H[1 ]));
  C[7 ] = ADD(C[7 ],ADD(M[5 ],H[3 ]));
  C[9 ] = ADD(C[9 ],ADD(M[7 ],H[5 ]));
  C[11] = ADD(C[11],ADD(M[9 ],H[7 ]));
  C[13] = ADD(C[13],ADD(M[11],H[9 ]));
  C[15] = ADD(C[15],ADD(M[13],H[11]));
}

DECL(void, new_compressfast2)(argElement_4w C,argElement_4w D) {
  int i;
  const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
  const __m256i mask  = SET164(ones);

  EltFp448_4w_redradix M0;                         EltFp448_4w_redradix M1;

  for(i=0;i<NUM_DIGITS_FP448;i++)
  {
    M0[i] = SHR(C[i], BASE_FP448);					M1[i] = SHR(D[i], BASE_FP448);
    M0[i] = AND(M0[i], mask);                       M1[i] = AND(M1[i], mask);
    C[i] = AND(C[i], mask);                         D[i] = AND(D[i], mask);
  }

  C[0 ] = ADD(C[0 ],M0[15]);                          D[0 ] = ADD(D[0 ],M1[15]);
  C[2 ] = ADD(C[2 ],M0[0 ]);                          D[2 ] = ADD(D[2 ],M1[0 ]);
  C[4 ] = ADD(C[4 ],M0[2 ]);                          D[4 ] = ADD(D[4 ],M1[2 ]);
  C[6 ] = ADD(C[6 ],M0[4 ]);                          D[6 ] = ADD(D[6 ],M1[4 ]);
  C[8 ] = ADD(C[8 ],M0[6 ]);                          D[8 ] = ADD(D[8 ],M1[6 ]);
  C[10] = ADD(C[10],M0[8 ]);                          D[10] = ADD(D[10],M1[8 ]);
  C[12] = ADD(C[12],M0[10]);                          D[12] = ADD(D[12],M1[10]);
  C[14] = ADD(C[14],M0[12]);                          D[14] = ADD(D[14],M1[12]);

  C[1 ] = ADD(C[1 ],M0[15]);                          D[1 ] = ADD(D[1 ],M1[15]);

  C[1 ] = ADD(C[1 ],M0[14]);                          D[1 ] = ADD(D[1 ],M1[14]);
  C[3 ] = ADD(C[3 ],M0[1 ]);                          D[3 ] = ADD(D[3 ],M1[1 ]);
  C[5 ] = ADD(C[5 ],M0[3 ]);                          D[5 ] = ADD(D[5 ],M1[3 ]);
  C[7 ] = ADD(C[7 ],M0[5 ]);                          D[7 ] = ADD(D[7 ],M1[5 ]);
  C[9 ] = ADD(C[9 ],M0[7 ]);                          D[9 ] = ADD(D[9 ],M1[7 ]);
  C[11] = ADD(C[11],M0[9 ]);                          D[11] = ADD(D[11],M1[9 ]);
  C[13] = ADD(C[13],M0[11]);                          D[13] = ADD(D[13],M1[11]);
  C[15] = ADD(C[15],M0[13]);                          D[15] = ADD(D[15],M1[13]);
}

DECL(void, mul)(argElement_4w c, argElement_4w a, argElement_4w b) {
  FN(intmul)(c,a,b);
  FN(compress)(c);
}

DECL(void, sqr)(argElement_4w c) {
  FN(intsqr)(c);
  FN(compress)(c);
}

DECL(void, rand)(argElement_4w a) {
	EltFp448_1w_redradix a0,a1,a2,a3;
	rand_Fp448_1w_redradix(a0);
	rand_Fp448_1w_redradix(a1);
	rand_Fp448_1w_redradix(a2);
	rand_Fp448_1w_redradix(a3);
	FN(zip)(a,a0,a1,a2,a3);
}

DECL(void, print)(FILE*file,argElement_4w a) {
	EltFp448_1w_redradix a0,a1,a2,a3;
	FN(unzip)(a0,a1,a2,a3,a);
	print_Fp448_1w_redradix(file,a0);
	print_Fp448_1w_redradix(file,a1);
	print_Fp448_1w_redradix(file,a2);
	print_Fp448_1w_redradix(file,a3);
}

DECL(__m256i, cmp)(argElement_4w a, argElement_4w b) {
    int c0, c1, c2, c3;
	EltFp448_1w_redradix a0,a1,a2,a3;
	EltFp448_1w_redradix b0,b1,b2,b3;
	FN(unzip)(a0,a1,a2,a3,a);
	FN(unzip)(b0,b1,b2,b3,b);
    c0 = cmp_Fp448_1w_redradix(a0,b0);
    c1 = cmp_Fp448_1w_redradix(a1,b1);
    c2 = cmp_Fp448_1w_redradix(a2,b2);
    c3 = cmp_Fp448_1w_redradix(a3,b3);
	return SET64(c3, c2, c1, c0);
}

DECL(void, unser)(argElement_4w a, uint8_t * buf) {
  EltFp448_1w_redradix a0, a1,a2,a3;
  unser_Fp448_1w_redradix(a0, buf + 0*SIZE_FP448);
  unser_Fp448_1w_redradix(a1, buf + 1*SIZE_FP448);
  unser_Fp448_1w_redradix(a2, buf + 2*SIZE_FP448);
  unser_Fp448_1w_redradix(a3, buf + 3*SIZE_FP448);
  FN(zip)(a, a0, a1,a2,a3);
}

DECL(void, ser)(uint8_t *buf, argElement_4w a) {
  EltFp448_1w_redradix a0, a1,a2,a3;
  FN(unzip)(a0, a1,a2,a3, a);
  ser_Fp448_1w_redradix(buf+0*SIZE_FP448,a0);
  ser_Fp448_1w_redradix(buf+1*SIZE_FP448,a1);
  ser_Fp448_1w_redradix(buf+2*SIZE_FP448,a2);
  ser_Fp448_1w_redradix(buf+3*SIZE_FP448,a3);
}

DECL(argElement_4w,alloc)() {
	return (argElement_4w) allocate_bytes(NUM_DIGITS_FP448 * sizeof(__m256i));
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP448 */
