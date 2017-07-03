/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of eddsa_avx2.
 *
 * eddsa_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eddsa_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eddsa_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *
 * @param A
 * @param nA
 */
static void str_bytes_to_Element_4w_h0h8(argElement_4w A, __m256i nA[7])
{
	int i;
	const uint64_t ones = (((uint64_t)1)<<BASE_FP448)-1;
	const __m256i mask = _mm256_set1_epi64x(ones);

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

/**
 *
 * @param A
 * @param pA
 * @param pB
 * @param pC
 * @param pD
 */
static void zip_Element_4w_h0h8(argElement_4w A,uint64_t*pA,uint64_t*pB,uint64_t*pC,uint64_t*pD)
{
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

/**
 *
 * @param pA
 * @param pB
 * @param pC
 * @param pD
 * @param A
 */
static void unzip_Element_4w_h0h8(uint64_t*pA,uint64_t*pB,uint64_t*pC,uint64_t*pD,argElement_4w A)
{
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

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void add_Element_4w_h0h8(__m256i * __restrict C, __m256i * __restrict A, __m256i * __restrict B)
{
	int i=0;
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		C[i] = ADD(A[i], B[i]);
	}
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void sub_Element_4w_h0h8(__m256i * __restrict C, __m256i * __restrict A, __m256i * __restrict B)
{
	const __m256i _2P[NUM_DIGITS_FP448] = {
			SET1_64(0x1ffffffe),	SET1_64(0x3ffffffc),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffc),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe),
			SET1_64(0x1ffffffe),	SET1_64(0x1ffffffe)
	};
	int i=0;
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		C[i] = ADD(A[i],SUB(_2P[i],B[i]));
	}
}

/**
 *
 * @param A
 * @param B
 */
static void addsub_large_Element_4w_h0h8(__m256i * __restrict A, __m256i * __restrict B)
{
	const __m256i _2_to_34P[NUM_DIGITS_FP448] = {
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffff800000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000),
			SET1_64(0x3ffffffc00000000),	SET1_64(0x3ffffffc00000000)
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

#define mul_school_2x2(w0,w1,w2,w3,p0,p1,q0,q1) \
do{                            \
	w0 = MUL(p0,q0);           \
	w1 = MUL(p1,q0);           \
	w1 = ADD(w1,MUL(p0,q1));   \
	w2 =        MUL(p1,q1);    \
	w3 = ZERO;                 \
}while(0)

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

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_karatsuba_4w_h0h8(__m256i * __restrict C, __m256i * __restrict A, __m256i * __restrict B)
{
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
	C[14] = ADD(x7,y7);	                    C[15] = SUB(z7,x7);
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void sqr_karatsuba_4w_h0h8(__m256i * C)
{
	__m256i a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15;
	__m256i x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14;
	__m256i y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14;
	__m256i z0,z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12,z13,z14;

	a0 = C[0];	a8  = C[1];
	a1 = C[2];	a9  = C[3];
	a2 = C[4];	a10 = C[5];
	a3 = C[6];	a11 = C[7];
	a4 = C[8];	a12 = C[9];
	a5 = C[10];	a13 = C[11];
	a6 = C[12];	a14 = C[13];
	a7 = C[14];	a15 = C[15];

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

	C[0 ] = SUB(ADD(ADD(x0,y0),z8 ),x8 );	C[1 ] = SUB(ADD(ADD(y8 ,z0),z8 ),x0);
	C[2 ] = SUB(ADD(ADD(x1,y1),z9 ),x9 );	C[3 ] = SUB(ADD(ADD(y9 ,z1),z9 ),x1);
	C[4 ] = SUB(ADD(ADD(x2,y2),z10),x10);	C[5 ] = SUB(ADD(ADD(y10,z2),z10),x2);
	C[6 ] = SUB(ADD(ADD(x3,y3),z11),x11);	C[7 ] = SUB(ADD(ADD(y11,z3),z11),x3);
	C[8 ] = SUB(ADD(ADD(x4,y4),z12),x12);	C[9 ] = SUB(ADD(ADD(y12,z4),z12),x4);
	C[10] = SUB(ADD(ADD(x5,y5),z13),x13);	C[11] = SUB(ADD(ADD(y13,z5),z13),x5);
	C[12] = SUB(ADD(ADD(x6,y6),z14),x14);	C[13] = SUB(ADD(ADD(y14,z6),z14),x6);
	C[14] = ADD(x7,y7);	                    C[15] = SUB(z7,x7);
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_Element_4w_h0h8(__m256i *C, __m256i *A, __m256i *B)
{
	mul_karatsuba_4w_h0h8(C,A,B);
}

/**
 *
 * @param C
 */
static void sqr_Element_4w_h0h8(__m256i * C)
{
	sqr_karatsuba_4w_h0h8(C);
}
#undef mul_school_2x2
#undef mul_school_4x4
#undef mul_karat_8x8
#undef sqr_school_4x4
#undef sqr_karat_8x8

#define CARRY_WORD(X,H,I,J) \
	H = SHR(X[I], BASE_FP448);\
	X[I] = AND(X[I], mask);\
	X[J] = ADD(X[J], H);

/**
 *
 * @param C
 */
static void compress_Element_4w_h0h8(__m256i * C)
{
	__m256i h,residue;
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask = _mm256_set1_epi64x(ones);

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

/**
 *
 * @param C
 * @param D
 */
static void compress2_Element_4w_h0h8(__m256i * __restrict C,__m256i * __restrict D)
{
	__m256i h0,h1,residue0,residue1;
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask = _mm256_set1_epi64x(ones);

	CARRY_WORD(C,h0,0 ,2 )                            	CARRY_WORD(D,h1,0 ,2 )
	CARRY_WORD(C,h0,2 ,4 )                            	CARRY_WORD(D,h1,2 ,4 )
	CARRY_WORD(C,h0,4 ,6 )                            	CARRY_WORD(D,h1,4 ,6 )
	CARRY_WORD(C,h0,6 ,8 )                            	CARRY_WORD(D,h1,6 ,8 )
	CARRY_WORD(C,h0,8 ,10)                            	CARRY_WORD(D,h1,8 ,10)
	CARRY_WORD(C,h0,10,12)                            	CARRY_WORD(D,h1,10,12)
	CARRY_WORD(C,h0,12,14)                            	CARRY_WORD(D,h1,12,14)
	CARRY_WORD(C,h0,14,1 )                            	CARRY_WORD(D,h1,14,1 )
	CARRY_WORD(C,h0,1 ,3 )                            	CARRY_WORD(D,h1,1 ,3 )
	CARRY_WORD(C,h0,3 ,5 )                            	CARRY_WORD(D,h1,3 ,5 )
	CARRY_WORD(C,h0,5 ,7 )                            	CARRY_WORD(D,h1,5 ,7 )
	CARRY_WORD(C,h0,7 ,9 )                            	CARRY_WORD(D,h1,7 ,9 )
	CARRY_WORD(C,h0,9 ,11)                            	CARRY_WORD(D,h1,9 ,11)
	CARRY_WORD(C,h0,11,13)                            	CARRY_WORD(D,h1,11,13)
	CARRY_WORD(C,h0,13,15)                            	CARRY_WORD(D,h1,13,15)
	residue0 = SHR(C[15], BASE_FP448);               	residue1 = SHR(D[15], BASE_FP448);
	C[15] = AND(C[15], mask);                       	D[15] = AND(D[15], mask);

	C[0] = ADD(C[0],residue0);                      	D[0] = ADD(D[0],residue1);
	CARRY_WORD(C,h0,0 ,2 )                            	CARRY_WORD(D,h1,0 ,2 )
	CARRY_WORD(C,h0,2 ,4 )                            	CARRY_WORD(D,h1,2 ,4 )

	C[1] = ADD(C[1],residue0);                      	D[1] = ADD(D[1],residue1);
	CARRY_WORD(C,h0,1 ,3 )                            	CARRY_WORD(D,h1,1 ,3 )
	CARRY_WORD(C,h0,3 ,5 )                            	CARRY_WORD(D,h1,3 ,5 )
}
#undef CARRY_WORD

/** Util Functions */

/**
 *
 * @param X_Y_Z_T
 */
static void random_Element_4w_h0h8(argElement_4w X_Y_Z_T)
{
	Element_1w_Fp448 X,Y,Z,T;
	random_Element_1w_h0h8(X);
	random_Element_1w_h0h8(Y);
	random_Element_1w_h0h8(Z);
	random_Element_1w_h0h8(T);
	zip_Element_4w_h0h8(X_Y_Z_T,X,Y,Z,T);
}

/**
 *
 * @param X_Y_Z_T
 */
static void print_Element_4w_h0h8(argElement_4w X_Y_Z_T)
{
	Element_1w_Fp448 X,Y,Z,T;
	unzip_Element_4w_h0h8(X,Y,Z,T,X_Y_Z_T);
	print_Element_1w_h0h8(X);
	print_Element_1w_h0h8(Y);
	print_Element_1w_h0h8(Z);
	print_Element_1w_h0h8(T);
}

/**
 *
 * @param X0_X1_X2_X3
 * @param Y0_Y1_Y2_Y3
 * @return
 */
static int compare_Element_4w_h0h8(argElement_4w X0_X1_X2_X3, argElement_4w Y0_Y1_Y2_Y3)
{
	Element_1w_Fp448 X0,X1,X2,X3;
	Element_1w_Fp448 Y0,Y1,Y2,Y3;
	unzip_Element_4w_h0h8(X0,X1,X2,X3,X0_X1_X2_X3);
	unzip_Element_4w_h0h8(Y0,Y1,Y2,Y3,Y0_Y1_Y2_Y3);
	return compare_Element_1w_h0h8(X0, Y0) &&
		   compare_Element_1w_h0h8(X1, Y1) &&
		   compare_Element_1w_h0h8(X2, Y2) &&
		   compare_Element_1w_h0h8(X3, Y3);
}

/**
 *
 * @return
 */
static __m256i * init_Element_4w_h0h8()
{
	return (__m256i*) allocate_bytes(NUM_DIGITS_FP448 * sizeof(__m256i));
}

