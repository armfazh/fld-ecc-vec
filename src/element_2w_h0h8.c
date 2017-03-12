
#define copy_Element_2w_h0h8(C,A) \
do{                               \
	C[0] = A[0];                  \
	C[1] = A[1];                  \
	C[2] = A[2];                  \
	C[3] = A[3];                  \
	C[4] = A[4];                  \
	C[5] = A[5];                  \
	C[6] = A[6];                  \
	C[7] = A[7];                  \
}while(0)

/**
 *
 * @param C
 * @param LOW
 * @param HIGH
 */
static void interleave_2w_h0h8(argElement_2w C, argElement_1w LOW, argElement_1w HIGH)
{
	__m256i A0  = LOAD(LOW+0);
	__m256i A4  = LOAD(LOW+1);
	__m256i A8  = LOAD(LOW+2);
	__m256i A12 = LOAD(LOW+3);

	__m256i B0  = LOAD(HIGH+0);
	__m256i B4  = LOAD(HIGH+1);
	__m256i B8  = LOAD(HIGH+2);
	__m256i B12 = LOAD(HIGH+3);

	C[0] = PERM128(A0,B0,0x20);
	C[1] = PERM128(A0,B0,0x31);
	C[2] = PERM128(A4,B4,0x20);
	C[3] = PERM128(A4,B4,0x31);
	C[4] = PERM128(A8,B8,0x20);
	C[5] = PERM128(A8,B8,0x31);
	C[6] = PERM128(A12,B12,0x20);
	C[7] = PERM128(A12,B12,0x31);
}

/**
 *
 * @param LOW
 * @param HIGH
 * @param A
 */
static void deinterleave_2w_h0h8(argElement_1w LOW, argElement_1w HIGH, argElement_2w A)
{
	STORE(LOW  + 0, PERM128(A[0], A[1], 0x20));
	STORE(HIGH + 0, PERM128(A[0], A[1], 0x31));
	STORE(LOW  + 1, PERM128(A[2], A[3], 0x20));
	STORE(HIGH + 1, PERM128(A[2], A[3], 0x31));
	STORE(LOW  + 2, PERM128(A[4], A[5], 0x20));
	STORE(HIGH + 2, PERM128(A[4], A[5], 0x31));
	STORE(LOW  + 3, PERM128(A[6], A[7], 0x20));
	STORE(HIGH + 3, PERM128(A[6], A[7], 0x31));
}



/**
 *
 * @param C
 * @param A
 * @param B
 */
static void add_Element_2w_h0h8(argElement_2w C,argElement_2w A,argElement_2w B)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
		C[i] = ADD(A[i],B[i]);
}

static const uint64_t CONST_2P_2P_H0H8[2*NUM_DIGITS_FP448] = {
		0x1ffffffe,0x3ffffffc,0x1ffffffe,0x3ffffffc,
		0x1ffffffe,0x1ffffffc,0x1ffffffe,0x1ffffffc,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,0x1ffffffe,0x1ffffffe};

static const uint64_t CONST_2P_00_H0H8[2*NUM_DIGITS_FP448] = {
		0x1ffffffe,0x3ffffffc,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffc,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000,
		0x1ffffffe,0x1ffffffe,0x0000000,0x0000000};
/**
 *
 * @param C
 * @param A
 * @param B
 */
static void sub_Element_2w_h0h8(argElement_2w __restrict C, argElement_2w __restrict A, argElement_2w __restrict B)
{
	argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H8;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
		C[i] = ADD(A[i],SUB(_2P[i],B[i]));
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
 * @param C
 * @param A
 * @param permutation
 */
static inline void subadd_Element_2w_h0h8(
		argElement_2w __restrict C,
		argElement_2w __restrict A,
		const int permutation)
{
	const __m256i mask_subadd = _mm256_set_epi64x(0,0,-1, -1);
	argElement_2w _2P_00 = (argElement_2w)CONST_2P_00_H0H8;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		__m256i M=ZERO,N=ZERO,PA;
		PA = PERM64(A[i],0x4E);
		if(permutation==0)
		{
			M = A[i];
			N = PA;
		}
		else if(permutation==1)
		{
			N = A[i];
			M = PA;
		}
		C[i] = ADD(ADD(M,_2P_00[i]),SUB(XOR(N,mask_subadd),mask_subadd));
	}
}

/**
 *
 * @param A
 * @param B
 */
static inline void addsub_Element_2w_h0h8(argElement_2w __restrict A, argElement_2w __restrict B)
{
	argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H8;

	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		__m256i D,C;
		D = ADD(A[i], B[i]);
		C = ADD(A[i],SUB(_2P[i],B[i]));
		A[i] = D;
		B[i] = C;
	}
}

/**
 * Given
 * 		A = [X |Y ]
 * Returns
 * 		C = [2P-X | Y]
 * @param C
 * @param A
 */
static inline void negZZ_Element_2w_h0h8(argElement_2w C,argElement_2w A)
{
	const __m256i mask_subadd = _mm256_set_epi64x(0,0,-1, -1);
	argElement_2w _2P_00 = (argElement_2w)CONST_2P_00_H0H8;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		C[i] = ADD(_2P_00[i],SUB(XOR(A[i],mask_subadd),mask_subadd));
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

#define mul_karat_4x4(r0,r1,r2,r3,r4,r5,r6,s0,s1,s2,s3,t0,t1,t2,t3)   \
do{                                                                      \
	__m256i i0,i1,i2,i3;                                                 \
	__m256i j0,j1,j2,j3;                                                 \
	__m256i k0,k1,k2,k3;                                                 \
                                                                         \
	mul_school_2x2(i0,i1,i2,i3,s0,s1,t0,t1);                             \
	mul_school_2x2(j0,j1,j2,j3,s2,s3,t2,t3);                             \
	mul_school_2x2(k0,k1,k2,k3,                                          \
				  ADD(s0,s2),                                            \
				  ADD(s1,s3),                                            \
				  ADD(t0,t2),                                            \
				  ADD(t1,t3));                                           \
    r0 = i0;                           r4 = SUB(ADD(j0,k2),ADD(i2,j2));  \
    r1 = i1;                           r5 = SUB(ADD(j1,k3),ADD(i3,j3));  \
    r2 = SUB(ADD(i2,k0),ADD(i0,j0));   r6 = j2;                          \
    r3 = SUB(ADD(i3,k1),ADD(i1,j1));                                     \
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

#define mul_karat_8x8(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,h13,h14,    \
               f0,f1,f2,f3,f4,f5,f6,f7,g0,g1,g2,g3,g4,g5,g6,g7)             \
do{                                                                         \
 	__m256i o0,o1,o2,o3,o4,o5,o6;                                           \
	__m256i p0,p1,p2,p3,p4,p5,p6;                                           \
	__m256i q0,q1,q2,q3,q4,q5,q6;                                           \
	mul_school_4x4(o0,o1,o2,o3,o4,o5,o6,f0,f1,f2,f3,g0,g1,g2,g3);           \
	mul_school_4x4(p0,p1,p2,p3,p4,p5,p6,f4,f5,f6,f7,g4,g5,g6,g7);           \
	mul_school_4x4(q0,q1,q2,q3,q4,q5,q6,                                    \
		ADD(f0,f4),                                                         \
		ADD(f1,f5),                                                         \
		ADD(f2,f6),                                                         \
		ADD(f3,f7),                                                         \
		ADD(g0,g4),                                                         \
		ADD(g1,g5),                                                         \
		ADD(g2,g6),                                                         \
		ADD(g3,g7));                                                        \
    __m256i ref0 = SUB(o4,p0);                                              \
    __m256i ref1 = SUB(o5,p1);                                              \
    __m256i ref2 = SUB(o6,p2);                                              \
	h0 = o0;                         h8  = SUB(SUB(q4,p4),ref0);            \
	h1 = o1;                         h9  = SUB(SUB(q5,p5),ref1);            \
	h2 = o2;                         h10 = SUB(SUB(q6,p6),ref2);            \
	h3 = o3;                         h11 = p3;                              \
	h4 = ADD(ref0,SUB(q0,o0));       h12 = p4;                              \
	h5 = ADD(ref1,SUB(q1,o1));       h13 = p5;                              \
	h6 = ADD(ref2,SUB(q2,o2));       h14 = p6;                              \
	h7 = SUB(SUB(q3,o3),p3);                                                \
}while(0)


#define mul_karate_8x8(h0,h1,h2,h3,h4,h5,h6,h7,                                        \
               f0,f1,f2,f3,g0,g1,g2,g3)                                                \
do{                                                                                    \
 	__m256i o0p0,o1p1,o2p2,o3p3,o4p4,o5p5,o6p6;                                        \
	__m256i q0,q1,q2,q3,q4,q5,q6;                                                      \
	mul_school_4x4(o0p0,o1p1,o2p2,o3p3,o4p4,o5p5,o6p6,f0,f1,f2,f3,g0,g1,g2,g3);        \
	__m256i sF0 = ADD(f0,SHUF(f0,0x4E)); __m256i sG0 = ADD(g0,SHUF(g0,0x4E));          \
	__m256i sF1 = ADD(f1,SHUF(f1,0x4E)); __m256i sG1 = ADD(g1,SHUF(g1,0x4E));          \
	__m256i sF2 = ADD(f2,SHUF(f2,0x4E)); __m256i sG2 = ADD(g2,SHUF(g2,0x4E));          \
	__m256i sF3 = ADD(f3,SHUF(f3,0x4E)); __m256i sG3 = ADD(g3,SHUF(g3,0x4E));          \
                                                                                       \
	mul_school_4x4(q0,q1,q2,q3,q4,q5,q6,                                               \
				  sF0,sF1,sF2,sF3,                                                     \
				  sG0,sG1,sG2,sG3);                                                    \
																					   \
	h0 = ADD(o0p0,BLEND32(SUB(q4,ADD(o4p4,SHUF(o4p4,0x4E))),ZERO,0x33));               \
	h1 = ADD(o1p1,BLEND32(SUB(q5,ADD(o5p5,SHUF(o5p5,0x4E))),ZERO,0x33));               \
	h2 = ADD(o2p2,BLEND32(SUB(q6,ADD(o6p6,SHUF(o6p6,0x4E))),ZERO,0x33));               \
	h3 = o3p3;                                                                         \
	h4 = ADD(o4p4,BLEND32(SUB(q0,ADD(o0p0,SHUF(o0p0,0x4E))),ZERO,0xCC));               \
	h5 = ADD(o5p5,BLEND32(SUB(q1,ADD(o1p1,SHUF(o1p1,0x4E))),ZERO,0xCC));               \
	h6 = ADD(o6p6,BLEND32(SUB(q2,ADD(o2p2,SHUF(o2p2,0x4E))),ZERO,0xCC));               \
	h7 = BLEND32(SUB(q3,ADD(o3p3,SHUF(o3p3,0x4E))),ZERO,0xCC);                         \
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

#define sqr_karate_8x8(h0,h1,h2,h3,h4,h5,h6,h7,                           \
                       f0,f1,f2,f3)                                       \
do{                                                                       \
 	__m256i o0p0,o1p1,o2p2,o3p3,o4p4,o5p5,o6p6;                           \
	__m256i q0,q1,q2,q3,q4,q5,q6;                                         \
	sqr_school_4x4(o0p0,o1p1,o2p2,o3p3,o4p4,o5p5,o6p6,f0,f1,f2,f3);       \
	__m256i sF0 = ADD(f0,SHUF(f0,0x4E));                                  \
	__m256i sF1 = ADD(f1,SHUF(f1,0x4E));                                  \
	__m256i sF2 = ADD(f2,SHUF(f2,0x4E));                                  \
	__m256i sF3 = ADD(f3,SHUF(f3,0x4E));                                  \
                                                                          \
	sqr_school_4x4(q0,q1,q2,q3,q4,q5,q6,                                  \
				  sF0,sF1,sF2,sF3);                                       \
																	      \
	h0 = ADD(o0p0,BLEND32(SUB(q4,ADD(o4p4,SHUF(o4p4,0x4E))),ZERO,0x33));  \
	h1 = ADD(o1p1,BLEND32(SUB(q5,ADD(o5p5,SHUF(o5p5,0x4E))),ZERO,0x33));  \
	h2 = ADD(o2p2,BLEND32(SUB(q6,ADD(o6p6,SHUF(o6p6,0x4E))),ZERO,0x33));  \
	h3 = o3p3;                                                            \
	h4 = ADD(o4p4,BLEND32(SUB(q0,ADD(o0p0,SHUF(o0p0,0x4E))),ZERO,0xCC));  \
	h5 = ADD(o5p5,BLEND32(SUB(q1,ADD(o1p1,SHUF(o1p1,0x4E))),ZERO,0xCC));  \
	h6 = ADD(o6p6,BLEND32(SUB(q2,ADD(o2p2,SHUF(o2p2,0x4E))),ZERO,0xCC));  \
	h7 = BLEND32(SUB(q3,ADD(o3p3,SHUF(o3p3,0x4E))),ZERO,0xCC);            \
}while(0)

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_karatsuba_2w_h0h8(__m256i *  C, __m256i * A, __m256i *  B)
{
	__m256i a0,a1,a2,a3,a4,a5,a6,a7;
	__m256i b0,b1,b2,b3,b4,b5,b6,b7;
	__m256i x0y0,x1y1,x2y2,x3y3,x4y4,x5y5,x6y6,x7y7,x8y8,x9y9,x10y10,x11y11,x12y12,x13y13,x14y14;
	__m256i z0,z1,z2,z3,z4,z5,z6,z7;

	a0 = A[0];   b0 = B[0];
	a1 = A[1];   b1 = B[1];
	a2 = A[2];   b2 = B[2];
	a3 = A[3];   b3 = B[3];
	a4 = A[4];   b4 = B[4];
	a5 = A[5];   b5 = B[5];
	a6 = A[6];   b6 = B[6];
	a7 = A[7];   b7 = B[7];

	mul_karat_8x8(x0y0,x1y1,x2y2,x3y3,x4y4,x5y5,x6y6,x7y7,x8y8,x9y9,x10y10,x11y11,x12y12,x13y13,x14y14,
				  a0,a1,a2,a3,a4,a5,a6,a7,
				  b0,b1,b2,b3,b4,b5,b6,b7);

	__m256i sA0_4 = ADD(UPKL64(a0,a4),UPKH64(a0,a4));
	__m256i sA1_5 = ADD(UPKL64(a1,a5),UPKH64(a1,a5));
	__m256i sA2_6 = ADD(UPKL64(a2,a6),UPKH64(a2,a6));
	__m256i sA3_7 = ADD(UPKL64(a3,a7),UPKH64(a3,a7));
	__m256i sB0_4 = ADD(UPKL64(b0,b4),UPKH64(b0,b4));
	__m256i sB1_5 = ADD(UPKL64(b1,b5),UPKH64(b1,b5));
	__m256i sB2_6 = ADD(UPKL64(b2,b6),UPKH64(b2,b6));
	__m256i sB3_7 = ADD(UPKL64(b3,b7),UPKH64(b3,b7));

	mul_karate_8x8(z0,z1,z2,z3,z4,z5,z6,z7,
				   sA0_4,sA1_5,sA2_6,sA3_7,
				   sB0_4,sB1_5,sB2_6,sB3_7);

	__m256i x0x8  = UPKL64(x0y0,x8y8  );  __m256i y0y8  = UPKH64(x0y0,x8y8  ); __m256i x0z8  = BLEND32(x0y0,z0,0xCC);  C[0] = ADD(ADD(SHUF(SUB(z0,x0x8 ),0x4E),y0y8 ),x0z8 );
	__m256i x1x9  = UPKL64(x1y1,x9y9  );  __m256i y1y9  = UPKH64(x1y1,x9y9  ); __m256i x1z9  = BLEND32(x1y1,z1,0xCC);  C[1] = ADD(ADD(SHUF(SUB(z1,x1x9 ),0x4E),y1y9 ),x1z9 );
	__m256i x2x10 = UPKL64(x2y2,x10y10);  __m256i y2y10 = UPKH64(x2y2,x10y10); __m256i x2z10 = BLEND32(x2y2,z2,0xCC);  C[2] = ADD(ADD(SHUF(SUB(z2,x2x10),0x4E),y2y10),x2z10);
	__m256i x3x11 = UPKL64(x3y3,x11y11);  __m256i y3y11 = UPKH64(x3y3,x11y11); __m256i x3z11 = BLEND32(x3y3,z3,0xCC);  C[3] = ADD(ADD(SHUF(SUB(z3,x3x11),0x4E),y3y11),x3z11);
	__m256i x4x12 = UPKL64(x4y4,x12y12);  __m256i y4y12 = UPKH64(x4y4,x12y12); __m256i x4z12 = BLEND32(x4y4,z4,0xCC);  C[4] = ADD(ADD(SHUF(SUB(z4,x4x12),0x4E),y4y12),x4z12);
	__m256i x5x13 = UPKL64(x5y5,x13y13);  __m256i y5y13 = UPKH64(x5y5,x13y13); __m256i x5z13 = BLEND32(x5y5,z5,0xCC);  C[5] = ADD(ADD(SHUF(SUB(z5,x5x13),0x4E),y5y13),x5z13);
	__m256i x6x14 = UPKL64(x6y6,x14y14);  __m256i y6y14 = UPKH64(x6y6,x14y14); __m256i x6z14 = BLEND32(x6y6,z6,0xCC);  C[6] = ADD(ADD(SHUF(SUB(z6,x6x14),0x4E),y6y14),x6z14);
	__m256i x7x15 = UPKL64(x7y7,ZERO);    __m256i y7y15 = UPKH64(x7y7,ZERO);   __m256i x7z15 = BLEND32(x7y7,z7,0xCC);  C[7] = ADD(ADD(SHUF(SUB(z7,x7x15),0x4E),y7y15),x7z15);
}

/**
 *
 * @param C
 */
static void sqr_karatsuba_2w_h0h8(__m256i *  C)
{
	__m256i a0,a1,a2,a3,a4,a5,a6,a7;
	__m256i x0y0,x1y1,x2y2,x3y3,x4y4,x5y5,x6y6,x7y7,x8y8,x9y9,x10y10,x11y11,x12y12,x13y13,x14y14;
	__m256i z0,z1,z2,z3,z4,z5,z6,z7;

	a0 = C[0];
	a1 = C[1];
	a2 = C[2];
	a3 = C[3];
	a4 = C[4];
	a5 = C[5];
	a6 = C[6];
	a7 = C[7];

	sqr_karat_8x8(x0y0,x1y1,x2y2,x3y3,x4y4,x5y5,x6y6,x7y7,x8y8,x9y9,x10y10,x11y11,x12y12,x13y13,x14y14,
				  a0,a1,a2,a3,a4,a5,a6,a7);

	__m256i sA0_4 = ADD(UPKL64(a0,a4),UPKH64(a0,a4));
	__m256i sA1_5 = ADD(UPKL64(a1,a5),UPKH64(a1,a5));
	__m256i sA2_6 = ADD(UPKL64(a2,a6),UPKH64(a2,a6));
	__m256i sA3_7 = ADD(UPKL64(a3,a7),UPKH64(a3,a7));

	sqr_karate_8x8(z0,z1,z2,z3,z4,z5,z6,z7,
				   sA0_4,sA1_5,sA2_6,sA3_7);

	__m256i x0x8  = UPKL64(x0y0,x8y8  );  __m256i y0y8  = UPKH64(x0y0,x8y8  ); __m256i x0z8  = BLEND32(x0y0,z0,0xCC);  C[0] = ADD(ADD(SHUF(SUB(z0,x0x8 ),0x4E),y0y8 ),x0z8 );
	__m256i x1x9  = UPKL64(x1y1,x9y9  );  __m256i y1y9  = UPKH64(x1y1,x9y9  ); __m256i x1z9  = BLEND32(x1y1,z1,0xCC);  C[1] = ADD(ADD(SHUF(SUB(z1,x1x9 ),0x4E),y1y9 ),x1z9 );
	__m256i x2x10 = UPKL64(x2y2,x10y10);  __m256i y2y10 = UPKH64(x2y2,x10y10); __m256i x2z10 = BLEND32(x2y2,z2,0xCC);  C[2] = ADD(ADD(SHUF(SUB(z2,x2x10),0x4E),y2y10),x2z10);
	__m256i x3x11 = UPKL64(x3y3,x11y11);  __m256i y3y11 = UPKH64(x3y3,x11y11); __m256i x3z11 = BLEND32(x3y3,z3,0xCC);  C[3] = ADD(ADD(SHUF(SUB(z3,x3x11),0x4E),y3y11),x3z11);
	__m256i x4x12 = UPKL64(x4y4,x12y12);  __m256i y4y12 = UPKH64(x4y4,x12y12); __m256i x4z12 = BLEND32(x4y4,z4,0xCC);  C[4] = ADD(ADD(SHUF(SUB(z4,x4x12),0x4E),y4y12),x4z12);
	__m256i x5x13 = UPKL64(x5y5,x13y13);  __m256i y5y13 = UPKH64(x5y5,x13y13); __m256i x5z13 = BLEND32(x5y5,z5,0xCC);  C[5] = ADD(ADD(SHUF(SUB(z5,x5x13),0x4E),y5y13),x5z13);
	__m256i x6x14 = UPKL64(x6y6,x14y14);  __m256i y6y14 = UPKH64(x6y6,x14y14); __m256i x6z14 = BLEND32(x6y6,z6,0xCC);  C[6] = ADD(ADD(SHUF(SUB(z6,x6x14),0x4E),y6y14),x6z14);
	__m256i x7x15 = UPKL64(x7y7,ZERO);    __m256i y7y15 = UPKH64(x7y7,ZERO);   __m256i x7z15 = BLEND32(x7y7,z7,0xCC);  C[7] = ADD(ADD(SHUF(SUB(z7,x7x15),0x4E),y7y15),x7z15);
}

#undef mul_school_2x2
#undef mul_karat_4x4
#undef mul_school_4x4
#undef mul_karat_8x8
#undef mul_karate_8x8
#undef sqr_school_4x4
#undef sqr_karat_8x8
#undef sqr_karate_8x8
/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_Element_2w_h0h8(__m256i *  C, __m256i * A, __m256i *  B)
{
	mul_karatsuba_2w_h0h8(C,A,B);
}

/**
 *
 * @param C
 */
static void sqr_Element_2w_h0h8(__m256i *  C)
{
	sqr_karatsuba_2w_h0h8(C);
}

/**
 *
 * @param C
 */
static void compress_Element_2w_h0h8(__m256i * C)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask = _mm256_set_epi32(0, ones, 0, ones,0, ones, 0, ones);

	__m256i c0 = C[0];
	__m256i c1 = C[1];
	__m256i c2 = C[2];
	__m256i c3 = C[3];
	__m256i c4 = C[4];
	__m256i c5 = C[5];
	__m256i c6 = C[6];
	__m256i c7 = C[7];

	__m256i h0_h8,  h1_h9,  h2_h10, h3_h11,
			h4_h12, h5_h13, h6_h14, h7_h15;

	h0_h8 = SHR(c0, BASE_FP448);
	c0 = AND(c0, mask);
	c1 = ADD(c1, h0_h8);

	h1_h9 = SHR(c1, BASE_FP448);
	c1 = AND(c1, mask);
	c2 = ADD(c2, h1_h9);

	h2_h10 = SHR(c2, BASE_FP448);
	c2 = AND(c2, mask);
	c3 = ADD(c3, h2_h10);

	h3_h11 = SHR(c3, BASE_FP448);
	c3 = AND(c3, mask);
	c4 = ADD(c4, h3_h11);

	h4_h12 = SHR(c4, BASE_FP448);
	c4 = AND(c4, mask);
	c5 = ADD(c5, h4_h12);

	h5_h13 = SHR(c5, BASE_FP448);
	c5 = AND(c5, mask);
	c6 = ADD(c6, h5_h13);

	h6_h14 = SHR(c6, BASE_FP448);
	c6 = AND(c6, mask);
	c7 = ADD(c7, h6_h14);

	h7_h15 = SHR(c7, BASE_FP448);
	c7 = AND(c7, mask);

	/* LOW  h7  ->  h0 */
	/* HIGH h15 ->  h8 */
	c0 = ADD(c0,BLEND32(h7_h15,ZERO,0x33));

	/* LOW  h7   \/  h0 */
	/* HIGH h15  /\  h8 */
	c0 = ADD(c0,SHUF(h7_h15,0x4E));

	h0_h8 = SHR(c0, BASE_FP448);
	c0 = AND(c0, mask);
	c1 = ADD(c1, h0_h8);

	C[0] = c0;
	C[1] = c1;
	C[2] = c2;
	C[3] = c3;
	C[4] = c4;
	C[5] = c5;
	C[6] = c6;
	C[7] = c7;
}

/**
 *
 * @param C
 * @param D
 */
static void compress2_Element_2w_h0h8(__m256i * C, __m256i * D)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask = _mm256_set_epi32(0, ones, 0, ones,0, ones, 0, ones);

	__m256i _C_c0 = C[0];                                      __m256i _D_c0 = D[0];
	__m256i _C_c1 = C[1];                                      __m256i _D_c1 = D[1];
	__m256i _C_c2 = C[2];                                      __m256i _D_c2 = D[2];
	__m256i _C_c3 = C[3];                                      __m256i _D_c3 = D[3];
	__m256i _C_c4 = C[4];                                      __m256i _D_c4 = D[4];
	__m256i _C_c5 = C[5];                                      __m256i _D_c5 = D[5];
	__m256i _C_c6 = C[6];                                      __m256i _D_c6 = D[6];
	__m256i _C_c7 = C[7];                                      __m256i _D_c7 = D[7];

	__m256i _C_h0_h8,  _C_h1_h9;                              __m256i _D_h0_h8, _D_h1_h9;
	__m256i _C_h2_h10, _C_h3_h11;                             __m256i _D_h2_h10,_D_h3_h11;
	__m256i _C_h4_h12, _C_h5_h13;                             __m256i _D_h4_h12,_D_h5_h13;
	__m256i _C_h6_h14, _C_h7_h15;                             __m256i _D_h6_h14,_D_h7_h15;

	_C_h0_h8 = SHR(_C_c0, BASE_FP448);                         _D_h0_h8 = SHR(_D_c0, BASE_FP448);
	_C_c0 = AND(_C_c0, mask);                                 _D_c0 = AND(_D_c0, mask);
	_C_c1 = ADD(_C_c1, _C_h0_h8);                             _D_c1 = ADD(_D_c1,_D_h0_h8);

	_C_h1_h9 = SHR(_C_c1, BASE_FP448);                         _D_h1_h9 = SHR(_D_c1, BASE_FP448);
	_C_c1 = AND(_C_c1, mask);                                 _D_c1 = AND(_D_c1, mask);
	_C_c2 = ADD(_C_c2, _C_h1_h9);                             _D_c2 = ADD(_D_c2,_D_h1_h9);

	_C_h2_h10 = SHR(_C_c2, BASE_FP448);                        _D_h2_h10 = SHR(_D_c2, BASE_FP448);
	_C_c2 = AND(_C_c2, mask);                                 _D_c2 = AND(_D_c2, mask);
	_C_c3 = ADD(_C_c3, _C_h2_h10);                            _D_c3 = ADD(_D_c3,_D_h2_h10);

	_C_h3_h11 = SHR(_C_c3, BASE_FP448);                        _D_h3_h11 = SHR(_D_c3, BASE_FP448);
	_C_c3 = AND(_C_c3, mask);                                 _D_c3 = AND(_D_c3, mask);
	_C_c4 = ADD(_C_c4, _C_h3_h11);                            _D_c4 = ADD(_D_c4,_D_h3_h11);

	_C_h4_h12 = SHR(_C_c4, BASE_FP448);                        _D_h4_h12 = SHR(_D_c4, BASE_FP448);
	_C_c4 = AND(_C_c4, mask);                                 _D_c4 = AND(_D_c4, mask);
	_C_c5 = ADD(_C_c5, _C_h4_h12);                            _D_c5 = ADD(_D_c5,_D_h4_h12);

	_C_h5_h13 = SHR(_C_c5, BASE_FP448);                        _D_h5_h13 = SHR(_D_c5, BASE_FP448);
	_C_c5 = AND(_C_c5, mask);                                 _D_c5 = AND(_D_c5, mask);
	_C_c6 = ADD(_C_c6, _C_h5_h13);                            _D_c6 = ADD(_D_c6,_D_h5_h13);

	_C_h6_h14 = SHR(_C_c6, BASE_FP448);                        _D_h6_h14 = SHR(_D_c6, BASE_FP448);
	_C_c6 = AND(_C_c6, mask);                                 _D_c6 = AND(_D_c6, mask);
	_C_c7 = ADD(_C_c7, _C_h6_h14);                            _D_c7 = ADD(_D_c7,_D_h6_h14);

	_C_h7_h15 = SHR(_C_c7, BASE_FP448);                        _D_h7_h15 = SHR(_D_c7, BASE_FP448);
	_C_c7 = AND(_C_c7, mask);                                 _D_c7 = AND(_D_c7, mask);

	/* LOW  h7  ->  h0 */                                      /* LOW  h7  ->  h0 */
	/* HIGH h15 ->  h8 */                                      /* HIGH h15 ->  h8 */
	_C_c0 = ADD(_C_c0, BLEND32(_C_h7_h15, ZERO, 0x33));       _D_c0 = ADD(_D_c0, BLEND32(_D_h7_h15, ZERO, 0x33));

	/* LOW  h7   \/  h0 */                                     /* LOW  h7   \/  h0 */
	/* HIGH h15  /\  h8 */                                     /* HIGH h15  /\  h8 */
	_C_c0 = ADD(_C_c0, SHUF(_C_h7_h15, 0x4E));                _D_c0 = ADD(_D_c0, SHUF(_D_h7_h15, 0x4E));

	_C_h0_h8 = SHR(_C_c0, BASE_FP448);                         _D_h0_h8 = SHR(_D_c0, BASE_FP448);
	_C_c0 = AND(_C_c0, mask);                                 _D_c0 = AND(_D_c0, mask);
	_C_c1 = ADD(_C_c1, _C_h0_h8);                             _D_c1 = ADD(_D_c1,_D_h0_h8);

	C[0] = _C_c0;                                              D[0] =_D_c0;
	C[1] = _C_c1;                                              D[1] =_D_c1;
	C[2] = _C_c2;                                              D[2] =_D_c2;
	C[3] = _C_c3;                                              D[3] =_D_c3;
	C[4] = _C_c4;                                              D[4] =_D_c4;
	C[5] = _C_c5;                                              D[5] =_D_c5;
	C[6] = _C_c6;                                              D[6] =_D_c6;
	C[7] = _C_c7;                                              D[7] =_D_c7;
}

/**
 *
 * @param C
 */
static void compressfast_Element_2w_h0h8(__m256i * C)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask  = _mm256_set_epi32(0, ones, 0, ones, 0, ones, 0, ones);

	__m256i l0,l1,l2,l3,l4,l5,l6,l7;
	__m256i m0,m1,m2,m3,m4,m5,m6,m7;

	l0 = AND(C[0],mask);
	l1 = AND(C[1],mask);
	l2 = AND(C[2],mask);
	l3 = AND(C[3],mask);
	l4 = AND(C[4],mask);
	l5 = AND(C[5],mask);
	l6 = AND(C[6],mask);
	l7 = AND(C[7],mask);

	m0 = SHR(C[0],BASE_FP448);
	m1 = SHR(C[1],BASE_FP448);
	m2 = SHR(C[2],BASE_FP448);
	m3 = SHR(C[3],BASE_FP448);
	m4 = SHR(C[4],BASE_FP448);
	m5 = SHR(C[5],BASE_FP448);
	m6 = SHR(C[6],BASE_FP448);
	m7 = SHR(C[7],BASE_FP448);

	C[0] = ADD(l0,ADD(
			SHUF(m7,0x4E),
			BLEND32(m7,ZERO,0x33)));
	C[1] = ADD(l1,m0);
	C[2] = ADD(l2,m1);
	C[3] = ADD(l3,m2);
	C[4] = ADD(l4,m3);
	C[5] = ADD(l5,m4);
	C[6] = ADD(l6,m5);
	C[7] = ADD(l7,m6);
}

/** Util functions */

/**
 *
 * @param X_Y
 */
static void random_Element_2w_h0h8(argElement_2w X_Y)
{
	Element_1w_Fp448 X,Y;
	random_Element_1w_h0h8(X);
	random_Element_1w_h0h8(Y);
	interleave_2w_h0h8(X_Y,X,Y);
}

/**
 *
 * @param X_Y
 */
static void print_Element_2w_h0h8(argElement_2w X_Y)
{
	Element_1w_Fp448 X,Y;
	deinterleave_2w_h0h8(X,Y,X_Y);
	print_Element_1w_h0h8(X);
	print_Element_1w_h0h8(Y);
}

/**
 *
 * @param X0_X1
 * @param Y0_Y1
 * @return
 */
static int compare_Element_2w_h0h8(argElement_2w X0_X1, argElement_2w Y0_Y1)
{
	Element_1w_Fp448 X0,X1;
	Element_1w_Fp448 Y0,Y1;
	deinterleave_2w_h0h8(X0,X1,X0_X1);
	deinterleave_2w_h0h8(Y0,Y1,Y0_Y1);
	return compare_Element_1w_h0h8(X0, Y0) && compare_Element_1w_h0h8(X1, Y1);
}

static __m256i * new_Element_2w_h0h8()
{
	return (__m256i*) allocate_bytes((NUM_DIGITS_FP448/2) * sizeof(__m256i));
}

static void clean_Element_2w_h0h8(__m256i * A)
{
	deallocate_bytes((void*)A);
}