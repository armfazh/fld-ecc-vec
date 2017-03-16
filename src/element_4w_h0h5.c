
/**
 *
 * @param A
 * @param nA
 */
static void str_bytes_to_Element_4w_h0h5(argElement_4w A, __m256i nA[4])
{
	const uint64_t ones0 = ((uint64_t)1<<(BASE0_FP25519))-1;
	const uint64_t ones1 = ((uint64_t)1<<(BASE1_FP25519))-1;

	const __m256i mask0 = SET1_64(ones0);
	const __m256i mask1 = SET1_64(ones1);

	A[0]= AND(nA[0], mask0);
	A[2]= AND(SHR(nA[0], BASE0_FP25519), mask1);
	A[4]= AND(XOR(SHL(nA[1], 13), SHR(nA[0], BASE0_FP25519 + BASE1_FP25519)), mask0);
	A[6]= AND(SHR(nA[1], 13), mask1);
	A[8]= AND(SHR(nA[1], 13 + BASE1_FP25519), mask0);

	A[1]= AND(nA[2], mask1);
	A[3]= AND(SHR(nA[2], BASE1_FP25519), mask0);
	A[5]= AND(XOR(SHL(nA[3], 13), SHR(nA[2], BASE0_FP25519 + BASE1_FP25519)), mask1);
	A[7]= AND(SHR(nA[3], 12), mask0);
	A[9]= AND(SHR(nA[3], 12 + BASE0_FP25519), mask1);
}
/**
 *
 * @param A
 * @param pA
 * @param pB
 * @param pC
 * @param pD
 */
static void zip_Element_4w_h0h5(argElement_4w A,uint64_t*pA,uint64_t*pB,uint64_t*pC,uint64_t*pD)
{
	__m256i A0,B0,C0,D0;
	__m256i AC0,BD0,AC2,BD2;
	__m256i A4,B4,C4,D4;
	__m256i AC4,BD4,AC6,BD6;
	__m256i A8,B8,C8,D8;
	__m256i AC8,BD8;

	A0 = LOAD(pA+0);
	B0 = LOAD(pB+0);
	C0 = LOAD(pC+0);
	D0 = LOAD(pD+0);

	AC0 = PERM128(A0,C0,0x20);
	BD0 = PERM128(B0,D0,0x20);
	AC2 = PERM128(A0,C0,0x31);
	BD2 = PERM128(B0,D0,0x31);

	A[0] = UPKL64(AC0,BD0);
	A[1] = UPKH64(AC0,BD0);
	A[2] = UPKL64(AC2,BD2);
	A[3] = UPKH64(AC2,BD2);

	A4 = LOAD(pA+1);
	B4 = LOAD(pB+1);
	C4 = LOAD(pC+1);
	D4 = LOAD(pD+1);

	AC4 = PERM128(A4,C4,0x20);
	BD4 = PERM128(B4,D4,0x20);
	AC6 = PERM128(A4,C4,0x31);
	BD6 = PERM128(B4,D4,0x31);

	A[4] = UPKL64(AC4,BD4);
	A[5] = UPKH64(AC4,BD4);
	A[6] = UPKL64(AC6,BD6);
	A[7] = UPKH64(AC6,BD6);

	A8 = LOAD(pA+2);
	B8 = LOAD(pB+2);
	C8 = LOAD(pC+2);
	D8 = LOAD(pD+2);

	AC8 = PERM128(A8,C8,0x20);
	BD8 = PERM128(B8,D8,0x20);

	A[8] = UPKL64(AC8,BD8);
	A[9] = UPKH64(AC8,BD8);
}
/**
 *
 * @param pA
 * @param pB
 * @param pC
 * @param pD
 * @param A
 */
void unzip_Element_4w_h0h5(uint64_t*pA,uint64_t*pB,uint64_t*pC,uint64_t*pD,argElement_4w A)
{
	__m256i A02,A13,C02,C13,A0,B0,C0,D0,
			A46,A57,C46,C57,A4,B4,C4,D4,AC8,BD8;

    A02 = PERM128(A[0],A[2],0x20);
	A13 = PERM128(A[1],A[3],0x20);
	C02 = PERM128(A[0],A[2],0x31);
	C13 = PERM128(A[1],A[3],0x31);
	A0 = UPKL64(A02,A13);
	B0 = UPKH64(A02,A13);
	C0 = UPKL64(C02,C13);
	D0 = UPKH64(C02,C13);

	STORE(pA+0,A0);
	STORE(pB+0,B0);
	STORE(pC+0,C0);
	STORE(pD+0,D0);

	A46 = PERM128(A[4],A[6],0x20);
	A57 = PERM128(A[5],A[7],0x20);
	C46 = PERM128(A[4],A[6],0x31);
	C57 = PERM128(A[5],A[7],0x31);
	A4 = UPKL64(A46,A57);
	B4 = UPKH64(A46,A57);
	C4 = UPKL64(C46,C57);
	D4 = UPKH64(C46,C57);

	STORE(pA+1,A4);
	STORE(pB+1,B4);
	STORE(pC+1,C4);
	STORE(pD+1,D4);

	AC8 = UPKL64(A[8],A[9]);
	BD8 = UPKH64(A[8],A[9]);

	STORE(pA+2,_mm256_castsi128_si256(_mm256_extracti128_si256(AC8,0)));
	STORE(pB+2,_mm256_castsi128_si256(_mm256_extracti128_si256(BD8,0)));
	STORE(pC+2,_mm256_castsi128_si256(_mm256_extracti128_si256(AC8,1)));
	STORE(pD+2,_mm256_castsi128_si256(_mm256_extracti128_si256(BD8,1)));
}


/**
 *
 * @param C
 * @param A
 * @param B
 */
static void add_Element_4w_h0h5(__m256i * __restrict C, __m256i * __restrict A, __m256i * __restrict B)
{
	int i=0;
	for(i=0;i<10;i++)
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
static void sub_Element_4w_h0h5(__m256i * __restrict C, __m256i * __restrict A, __m256i * __restrict B)
{
	const __m256i _2P[10] = {
			SET1_64(0x7ffffda),	SET1_64(0x3fffffe),
			SET1_64(0x3fffffe),	SET1_64(0x7fffffe),
			SET1_64(0x7fffffe),	SET1_64(0x3fffffe),
			SET1_64(0x3fffffe),	SET1_64(0x7fffffe),
			SET1_64(0x7fffffe),	SET1_64(0x3fffffe)
	};
	int i=0;
	for(i=0;i<10;i++)
	{
		C[i] = ADD(A[i],SUB(_2P[i],B[i]));
	}
}
/**
 *
 * @param C
 * @param D
 * @param A
 * @param B
 */
static void addsub_Element_4w_h0h5(__m256i * __restrict C,__m256i * __restrict D, __m256i * __restrict A, __m256i * __restrict B)
{
	const __m256i _2P[10] = {
			SET1_64(0x7ffffda),	SET1_64(0x3fffffe),
			SET1_64(0x3fffffe),	SET1_64(0x7fffffe),
			SET1_64(0x7fffffe),	SET1_64(0x3fffffe),
			SET1_64(0x3fffffe),	SET1_64(0x7fffffe),
			SET1_64(0x7fffffe),	SET1_64(0x3fffffe)
	};
	int i=0;
	for(i=0;i<10;i++)
	{
		C[i] = ADD(A[i],B[i]);
		D[i] = ADD(A[i],SUB(_2P[i],B[i]));
	}
}
/**
 *
 * @param A
 * @param B
 */
static void addsub_large_Element_4w_h0h5(__m256i * __restrict A, __m256i * __restrict B)
{
	const __m256i _2_to_34P[10] = {
			SET1_64(0xfffffb400000000),	SET1_64(0x7fffffc00000000),
			SET1_64(0x7fffffc00000000),	SET1_64(0xffffffc00000000),
			SET1_64(0xffffffc00000000),	SET1_64(0x7fffffc00000000),
			SET1_64(0x7fffffc00000000),	SET1_64(0xffffffc00000000),
			SET1_64(0xffffffc00000000),	SET1_64(0x7fffffc00000000)
	};

	int i=0;
	for(i=0;i<10;i++)
	{
		__m256i add = ADD(A[i],B[i]);
		__m256i sub = ADD(A[i],SUB(_2_to_34P[i], B[i]));
		A[i] = add;
		B[i] = sub;
	}
}

#define MUL19(X) ADD(X,ADD(ADD(X,X),SHL(X,4)))

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_schoolbook_Element_4w_h0h5(__m256i * C, __m256i * A, __m256i * B)
{
    const __m256i times19 = SET1_64(19);
    __m256i c0,c1,c2,c3,c4,c5,c6,c7,c8,c9;
    __m256i    d1,d2,d3,d4,d5,d6,d7,d8,d9;
    __m256i b0,b1,b2,b3,b4,b5,b6,b7,b8,b9;
    __m256i ai,a2i;

    b0 = B[0];	b5 = B[1];
    b1 = B[2];	b6 = B[3];
    b2 = B[4];	b7 = B[5];
    b3 = B[6];	b8 = B[7];
    b4 = B[8];	b9 = B[9];

	ai = A[0];
	c0 = MUL(ai,b0);			c5 = MUL(ai,b5);
	c1 = MUL(ai,b1);			c6 = MUL(ai,b6);
	c2 = MUL(ai,b2);			c7 = MUL(ai,b7);
	c3 = MUL(ai,b3);			c8 = MUL(ai,b8);
	c4 = MUL(ai,b4);			c9 = MUL(ai,b9);

    ai = A[2];
    a2i = ADD(A[2],A[2]);
    d9 = MUL(b9,times19);
    c0 = ADD(c0,MUL(a2i,d9));	c5 = ADD(c5,MUL(ai, b4));
    c1 = ADD(c1,MUL(ai, b0));	c6 = ADD(c6,MUL(a2i,b5));
    c2 = ADD(c2,MUL(a2i,b1));	c7 = ADD(c7,MUL(ai, b6));
    c3 = ADD(c3,MUL(ai, b2));	c8 = ADD(c8,MUL(a2i,b7));
    c4 = ADD(c4,MUL(a2i,b3));	c9 = ADD(c9,MUL(ai, b8));

    ai = A[4];
	d8 = MUL(b8,times19);
	c0 = ADD(c0,MUL(ai,d8));	c5 = ADD(c5,MUL(ai,b3));
	c1 = ADD(c1,MUL(ai,d9));	c6 = ADD(c6,MUL(ai,b4));
	c2 = ADD(c2,MUL(ai,b0));	c7 = ADD(c7,MUL(ai,b5));
	c3 = ADD(c3,MUL(ai,b1));	c8 = ADD(c8,MUL(ai,b6));
	c4 = ADD(c4,MUL(ai,b2));	c9 = ADD(c9,MUL(ai,b7));

	ai = A[6];
	a2i = ADD(A[6],A[6]);
	d7 = MUL(b7,times19);
	c0 = ADD(c0,MUL(a2i,d7));	c5 = ADD(c5,MUL(ai,b2));
	c1 = ADD(c1,MUL(ai,d8));	c6 = ADD(c6,MUL(a2i,b3));
	c2 = ADD(c2,MUL(a2i,d9));	c7 = ADD(c7,MUL(ai,b4));
	c3 = ADD(c3,MUL(ai,b0));	c8 = ADD(c8,MUL(a2i,b5));
	c4 = ADD(c4,MUL(a2i,b1));	c9 = ADD(c9,MUL(ai,b6));

	ai = A[8];
	d6 = MUL(b6,times19);
	c0 = ADD(c0,MUL(ai,d6));	c5 = ADD(c5,MUL(ai,b1));
	c1 = ADD(c1,MUL(ai,d7));	c6 = ADD(c6,MUL(ai,b2));
	c2 = ADD(c2,MUL(ai,d8));	c7 = ADD(c7,MUL(ai,b3));
	c3 = ADD(c3,MUL(ai,d9));	c8 = ADD(c8,MUL(ai,b4));
	c4 = ADD(c4,MUL(ai,b0));	c9 = ADD(c9,MUL(ai,b5));

	ai = A[1];
	a2i = ADD(A[1],A[1]);
	d5 = MUL(b5,times19);
	c0 = ADD(c0,MUL(a2i,d5));	c5 = ADD(c5,MUL(ai,b0));
	c1 = ADD(c1,MUL(ai,d6));	c6 = ADD(c6,MUL(a2i,b1));
	c2 = ADD(c2,MUL(a2i,d7));	c7 = ADD(c7,MUL(ai,b2));
	c3 = ADD(c3,MUL(ai,d8));	c8 = ADD(c8,MUL(a2i,b3));
	c4 = ADD(c4,MUL(a2i,d9));	c9 = ADD(c9,MUL(ai,b4));

	ai = A[3];
	d4 = MUL(b4,times19);
	c0 = ADD(c0,MUL(ai,d4));	c5 = ADD(c5,MUL(ai,d9));
	c1 = ADD(c1,MUL(ai,d5));	c6 = ADD(c6,MUL(ai,b0));
	c2 = ADD(c2,MUL(ai,d6));	c7 = ADD(c7,MUL(ai,b1));
	c3 = ADD(c3,MUL(ai,d7));	c8 = ADD(c8,MUL(ai,b2));
	c4 = ADD(c4,MUL(ai,d8));	c9 = ADD(c9,MUL(ai,b3));

	ai = A[5];
	a2i = ADD(A[5],A[5]);
	d3 = MUL(b3,times19);
	c0 = ADD(c0,MUL(a2i,d3));	c5 = ADD(c5,MUL(ai,d8));
	c1 = ADD(c1,MUL(ai,d4));	c6 = ADD(c6,MUL(a2i,d9));
	c2 = ADD(c2,MUL(a2i,d5));	c7 = ADD(c7,MUL(ai,b0));
	c3 = ADD(c3,MUL(ai,d6));	c8 = ADD(c8,MUL(a2i,b1));
	c4 = ADD(c4,MUL(a2i,d7));	c9 = ADD(c9,MUL(ai,b2));

	ai = A[7];
	d2 = MUL(b2,times19);
	c0 = ADD(c0,MUL(ai,d2));	c5 = ADD(c5,MUL(ai,d7));
	c1 = ADD(c1,MUL(ai,d3));	c6 = ADD(c6,MUL(ai,d8));
	c2 = ADD(c2,MUL(ai,d4));	c7 = ADD(c7,MUL(ai,d9));
	c3 = ADD(c3,MUL(ai,d5));	c8 = ADD(c8,MUL(ai,b0));
	c4 = ADD(c4,MUL(ai,d6));	c9 = ADD(c9,MUL(ai,b1));

	ai = A[9];
	a2i = ADD(A[9],A[9]);
	d1 = MUL(b1,times19);
	c0 = ADD(c0,MUL(a2i,d1));	c5 = ADD(c5,MUL(ai,d6));
	c1 = ADD(c1,MUL(ai,d2));	c6 = ADD(c6,MUL(a2i,d7));
	c2 = ADD(c2,MUL(a2i,d3));	c7 = ADD(c7,MUL(ai,d8));
	c3 = ADD(c3,MUL(ai,d4));	c8 = ADD(c8,MUL(a2i,d9));
	c4 = ADD(c4,MUL(a2i,d5));	c9 = ADD(c9,MUL(ai,b0));

    C[0] = c0;	C[1] = c5;
    C[2] = c1;	C[3] = c6;
    C[4] = c2;	C[5] = c7;
    C[6] = c3;	C[7] = c8;
    C[8] = c4;	C[9] = c9;
}
/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_Element_4w_h0h5(__m256i *C, __m256i *A, __m256i *B)
{
	mul_schoolbook_Element_4w_h0h5(C,A,B);
}
/**
 *
 * @param C
 * @param A
 */
static void sqr_Element_4w_h0h5(__m256i * C)
{
	const __m256i times19 = SET1_64(19);
	__m256i c0,c1,c2,c3,c4,c5,c6,c7,c8,c9;
	__m256i          d3,d4,d5,d6,d7,d8,d9;
	__m256i b0,b1,b2,b3,b4,b5,b6,b7,b8,b9;
	__m256i ai, aj, ak;

	b0 = C[0];	b5 = C[1];
	b1 = C[2];	b6 = C[3];
	b2 = C[4];	b7 = C[5];
	b3 = C[6];	b8 = C[7];
	b4 = C[8];	b9 = C[9];

	ai = C[0];
	aj = ADD(ai,ai);
	c0 = MUL(ai,b0);				c5 = MUL(ai,b5);
	c1 = MUL(aj,b1);				c6 = MUL(aj,b6);
	c2 = MUL(aj,b2);				c7 = MUL(aj,b7);
	c3 = MUL(aj,b3);				c8 = MUL(aj,b8);
	c4 = MUL(aj,b4);				c9 = MUL(aj,b9);

	ai = C[2];
	aj = ADD(ai,ai);
	ak = ADD(aj, aj);
	d9 = MUL(b9,times19);
	c0 = ADD(c0,MUL(ak,d9));		c5 = ADD(c5,MUL(aj,b4));
	/*c1 = ADD(c1,MUL(ai,b0));		c6 = ADD(c6,MUL(aj,b5));*/
	c2 = ADD(c2,MUL(aj,b1));		c7 = ADD(c7,MUL(ai,b6));
	c3 = ADD(c3,MUL(aj,b2));		c8 = ADD(c8,MUL(ak,b7));
	c4 = ADD(c4,MUL(ak,b3));		c9 = ADD(c9,MUL(aj,b8));

	ai = C[4];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d8 = MUL(b8,times19);
	c0 = ADD(c0,MUL(aj,d8));		c5 = ADD(c5,MUL(aj,b3));
	c1 = ADD(c1,MUL(aj,d9));		c6 = ADD(c6,MUL(aj,b4));
	/*c2 = ADD(c2,MUL(ai,b0));		c7 = ADD(c7,MUL(ai,b5));*/
	/*c3 = ADD(c3,MUL(ai,b1));		c8 = ADD(c8,MUL(ai,b6));*/
	c4 = ADD(c4,MUL(ai,b2));		c9 = ADD(c9,MUL(ai,b7));

	ai = C[6];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d7 = MUL(b7,times19);
	/*c0 = ADD(c0,MUL(aj,d7));		c5 = ADD(c5,MUL(ai,b2));*/
	c1 = ADD(c1,MUL(ai,d8));		c6 = ADD(c6,MUL(aj,b3));
	c2 = ADD(c2,MUL(ak,d9));		c7 = ADD(c7,MUL(aj,b4));
	/*c3 = ADD(c3,MUL(ai,b0));		c8 = ADD(c8,MUL(aj,b5));*/
	/*c4 = ADD(c4,MUL(aj,b1));		c9 = ADD(c9,MUL(ai,b6));*/

	ai = C[8];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d6 = MUL(b6,times19);
	/*c0 = ADD(c0,MUL(ai,d6));		c5 = ADD(c5,MUL(ai,b1));*/
	/*c1 = ADD(c1,MUL(ai,d7));		c6 = ADD(c6,MUL(ai,b2));*/
	/*c2 = ADD(c2,MUL(ai,d8));		c7 = ADD(c7,MUL(ai,b3));*/
	c3 = ADD(c3,MUL(ai,d9));		c8 = ADD(c8,MUL(ai,b4));
	/*c4 = ADD(c4,MUL(ai,b0));		c9 = ADD(c9,MUL(ai,b5));*/

	ai = C[1];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d5 = MUL(b5,times19);
	c0 = ADD(c0,MUL(aj,d5));		c5 = ADD(c5,MUL(ai,b0));
	c1 = ADD(c1,MUL(aj,d6));		c6 = ADD(c6,MUL(ak,b1));
	c2 = ADD(c2,MUL(ak,d7));		c7 = ADD(c7,MUL(aj,b2));
	c3 = ADD(c3,MUL(aj,d8));		c8 = ADD(c8,MUL(ak,b3));
	c4 = ADD(c4,MUL(ak,d9));		c9 = ADD(c9,MUL(aj,b4));

	ai = C[3];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d4 = MUL(b4,times19);
	c0 = ADD(c0,MUL(aj,d4));		c5 = ADD(c5,MUL(aj,d9));
	/*c1 = ADD(c1,MUL(ai,d5));		c6 = ADD(c6,MUL(ai,b0));*/
	c2 = ADD(c2,MUL(ai,d6));		c7 = ADD(c7,MUL(ai,b1));
	c3 = ADD(c3,MUL(aj,d7));		c8 = ADD(c8,MUL(aj,b2));
	c4 = ADD(c4,MUL(aj,d8));		c9 = ADD(c9,MUL(aj,b3));

	ai = C[5];
	aj = ADD(ai,ai);
	ak = ADD(aj,aj);
	d3 = MUL(b3,times19);
	c0 = ADD(c0,MUL(ak,d3));		c5 = ADD(c5,MUL(aj,d8));
	c1 = ADD(c1,MUL(aj,d4));		c6 = ADD(c6,MUL(ak,d9));
	/*c2 = ADD(c2,MUL(aj,d5));		c7 = ADD(c7,MUL(ai,b0));*/
	/*c3 = ADD(c3,MUL(ai,d6));		c8 = ADD(c8,MUL(aj,b1));*/
	c4 = ADD(c4,MUL(aj,d7));		c9 = ADD(c9,MUL(ai,b2));

	ai = C[7];
	aj = ADD(ai,ai);
	/*ak = ADD(aj,aj);*/
	/*c0 = ADD(c0,MUL(ai,d2));		c5 = ADD(c5,MUL(ai,d7));*/
	c1 = ADD(c1,MUL(ai,d3));		c6 = ADD(c6,MUL(ai,d8));
	c2 = ADD(c2,MUL(aj,d4));		c7 = ADD(c7,MUL(aj,d9));
	/*c3 = ADD(c3,MUL(ai,d5));		c8 = ADD(c8,MUL(ai,b0));*/
	/*c4 = ADD(c4,MUL(ai,d6));		c9 = ADD(c9,MUL(ai,b1));*/

	ai = C[9];
	aj = ADD(ai,ai);
	/*ak = ADD(aj,aj);*/
	/*c0 = ADD(c0,MUL(aj,d1));		c5 = ADD(c5,MUL(ai, d6));*/
	/*c1 = ADD(c1,MUL(ai,d2));		c6 = ADD(c6,MUL(aj, d7));*/
	/*c2 = ADD(c2,MUL(aj,d3));		c7 = ADD(c7,MUL(ai, d8));*/
	c3 = ADD(c3,MUL(ai,d4));		c8 = ADD(c8,MUL(aj, d9));
	/*c4 = ADD(c4,MUL(aj,d5));		c9 = ADD(c9,MUL(ai, b0));*/

	C[0] = c0;	C[1] = c5;
	C[2] = c1;	C[3] = c6;
	C[4] = c2;	C[5] = c7;
	C[6] = c3;	C[7] = c8;
	C[8] = c4;	C[9] = c9;
}
/**
 *
 * @param C
 */
static void compress_Element_4w_h0h5(__m256i * C)
{
	const uint64_t ones26 = ((uint64_t) 1 << BASE0_FP25519) - 1;
	const uint64_t ones25 = ((uint64_t) 1 << BASE1_FP25519) - 1;
	const __m256i mask26  = SET1_64(ones26);
	const __m256i mask25  = SET1_64(ones25);

	__m256i c0 = LOAD(C+0);__m256i c5 = LOAD(C+1);
	__m256i c1 = LOAD(C+2);__m256i c6 = LOAD(C+3);
	__m256i c2 = LOAD(C+4);__m256i c7 = LOAD(C+5);
	__m256i c3 = LOAD(C+6);__m256i c8 = LOAD(C+7);
	__m256i c4 = LOAD(C+8);__m256i c9 = LOAD(C+9);

	__m256i h0,h1,h2,h3,h4,h5,h6,h7,h8,h9;

	h0 = SHR(c0, BASE0_FP25519);
	c0 = AND(c0, mask26);
	c1 = ADD(c1, h0);

	h1 = SHR(c1, BASE1_FP25519);
	c1 = AND(c1, mask25);
	c2 = ADD(c2, h1);

	h2 = SHR(c2, BASE0_FP25519);
	c2 = AND(c2, mask26);
	c3 = ADD(c3, h2);

	h3 = SHR(c3, BASE1_FP25519);
	c3 = AND(c3, mask25);
	c4 = ADD(c4, h3);

	h4 = SHR(c4, BASE0_FP25519);
	c4 = AND(c4, mask26);
	c5 = ADD(c5, h4);

	h5 = SHR(c5, BASE1_FP25519);
	c5 = AND(c5, mask25);
	c6 = ADD(c6, h5);

	h6 = SHR(c6, BASE0_FP25519);
	c6 = AND(c6, mask26);
	c7 = ADD(c7, h6);

	h7 = SHR(c7, BASE1_FP25519);
	c7 = AND(c7, mask25);
	c8 = ADD(c8, h7);

	h8 = SHR(c8, BASE0_FP25519);
	c8 = AND(c8, mask26);
	c9 = ADD(c9, h8);

	h9 = SHR(c9, BASE1_FP25519);
	c9 = AND(c9, mask25);
	c0 = ADD(c0, MUL19(h9));

	h0 = SHR(c0, BASE0_FP25519);
	c0 = AND(c0, mask26);
	c1 = ADD(c1, h0);

	STORE(C+0,c0);	STORE(C+1,c5);
	STORE(C+2,c1);	STORE(C+3,c6);
	STORE(C+4,c2);	STORE(C+5,c7);
	STORE(C+6,c3);	STORE(C+7,c8);
	STORE(C+8,c4);	STORE(C+9,c9);
}

/**
 *
 * @param C
 * @param D
 */
static void compress2_Element_4w_h0h5(__m256i * C,__m256i * D)
{
	const uint64_t ones26 = ((uint64_t) 1 << BASE0_FP25519) - 1;
	const uint64_t ones25 = ((uint64_t) 1 << BASE1_FP25519) - 1;
	const __m256i mask26  = SET1_64(ones26);
	const __m256i mask25  = SET1_64(ones25);

	__m256i c0 = LOAD(C+0);__m256i c5 = LOAD(C+1);        __m256i d0 = LOAD(D+0);__m256i d5 = LOAD(D+1);
	__m256i c1 = LOAD(C+2);__m256i c6 = LOAD(C+3);        __m256i d1 = LOAD(D+2);__m256i d6 = LOAD(D+3);
	__m256i c2 = LOAD(C+4);__m256i c7 = LOAD(C+5);        __m256i d2 = LOAD(D+4);__m256i d7 = LOAD(D+5);
	__m256i c3 = LOAD(C+6);__m256i c8 = LOAD(C+7);        __m256i d3 = LOAD(D+6);__m256i d8 = LOAD(D+7);
	__m256i c4 = LOAD(C+8);__m256i c9 = LOAD(C+9);        __m256i d4 = LOAD(D+8);__m256i d9 = LOAD(D+9);

	__m256i h0,h1,h2,h3,h4,h5,h6,h7,h8,h9;                __m256i _D_h0,_D_h1,_D_h2,_D_h3,_D_h4,_D_h5,_D_h6,_D_h7,_D_h8,_D_h9;

	h0 = SHR(c0, BASE0_FP25519);                             _D_h0 = SHR(d0, BASE0_FP25519);
	c0 = AND(c0, mask26);                                 d0 = AND(d0, mask26);
	c1 = ADD(c1, h0);                                     d1 = ADD(d1, _D_h0);

	h1 = SHR(c1, BASE1_FP25519);                             _D_h1 = SHR(d1, BASE1_FP25519);
	c1 = AND(c1, mask25);                                 d1 = AND(d1, mask25);
	c2 = ADD(c2, h1);                                     d2 = ADD(d2, _D_h1);

	h2 = SHR(c2, BASE0_FP25519);                             _D_h2 = SHR(d2, BASE0_FP25519);
	c2 = AND(c2, mask26);                                 d2 = AND(d2, mask26);
	c3 = ADD(c3, h2);                                     d3 = ADD(d3, _D_h2);

	h3 = SHR(c3, BASE1_FP25519);                             _D_h3 = SHR(d3, BASE1_FP25519);
	c3 = AND(c3, mask25);                                 d3 = AND(d3, mask25);
	c4 = ADD(c4, h3);                                     d4 = ADD(d4, _D_h3);

	h4 = SHR(c4, BASE0_FP25519);                             _D_h4 = SHR(d4, BASE0_FP25519);
	c4 = AND(c4, mask26);                                 d4 = AND(d4, mask26);
	c5 = ADD(c5, h4);                                     d5 = ADD(d5, _D_h4);

	h5 = SHR(c5, BASE1_FP25519);                             _D_h5 = SHR(d5, BASE1_FP25519);
	c5 = AND(c5, mask25);                                 d5 = AND(d5, mask25);
	c6 = ADD(c6, h5);                                     d6 = ADD(d6, _D_h5);

	h6 = SHR(c6, BASE0_FP25519);                             _D_h6 = SHR(d6, BASE0_FP25519);
	c6 = AND(c6, mask26);                                 d6 = AND(d6, mask26);
	c7 = ADD(c7, h6);                                     d7 = ADD(d7, _D_h6);

	h7 = SHR(c7, BASE1_FP25519);                             _D_h7 = SHR(d7, BASE1_FP25519);
	c7 = AND(c7, mask25);                                 d7 = AND(d7, mask25);
	c8 = ADD(c8, h7);                                     d8 = ADD(d8, _D_h7);

	h8 = SHR(c8, BASE0_FP25519);                             _D_h8 = SHR(d8, BASE0_FP25519);
	c8 = AND(c8, mask26);                                 d8 = AND(d8, mask26);
	c9 = ADD(c9, h8);                                     d9 = ADD(d9, _D_h8);

	h9 = SHR(c9, BASE1_FP25519);                             _D_h9 = SHR(d9, BASE1_FP25519);
	c9 = AND(c9, mask25);                                 d9 = AND(d9, mask25);
	c0 = ADD(c0, MUL19(h9));                              d0 = ADD(d0, MUL19(_D_h9));

	h0 = SHR(c0, BASE0_FP25519);                             _D_h0 = SHR(d0, BASE0_FP25519);
	c0 = AND(c0, mask26);                                 d0 = AND(d0, mask26);
	c1 = ADD(c1, h0);                                     d1 = ADD(d1, _D_h0);

	STORE(C+0,c0);	STORE(C+1,c5);                        STORE(D+0,d0);	STORE(D+1,d5);
	STORE(C+2,c1);	STORE(C+3,c6);                        STORE(D+2,d1);	STORE(D+3,d6);
	STORE(C+4,c2);	STORE(C+5,c7);                        STORE(D+4,d2);	STORE(D+5,d7);
	STORE(C+6,c3);	STORE(C+7,c8);                        STORE(D+6,d3);	STORE(D+7,d8);
	STORE(C+8,c4);	STORE(C+9,c9);                        STORE(D+8,d4);	STORE(D+9,d9);
}
/**
 *
 * @param C
 * @param D
 */
static void new_compressfast2_Element_4w_h0h5(__m256i * C, __m256i * D)
{
	const uint64_t ones26 = ((uint64_t) 1 << BASE0_FP25519) - 1;
	const uint64_t ones25 = ((uint64_t) 1 << BASE1_FP25519) - 1;
	const __m256i mask26  = SET1_64(ones26);
	const __m256i mask25  = SET1_64(ones25);
	const __m256i times19 = SET1_64(19);

	__m256i c0 = LOAD(C+0);__m256i c5 = LOAD(C+1);             __m256i d0 = LOAD(D+0);__m256i d5 = LOAD(D+1);
	__m256i c1 = LOAD(C+2);__m256i c6 = LOAD(C+3);             __m256i d1 = LOAD(D+2);__m256i d6 = LOAD(D+3);
	__m256i c2 = LOAD(C+4);__m256i c7 = LOAD(C+5);             __m256i d2 = LOAD(D+4);__m256i d7 = LOAD(D+5);
	__m256i c3 = LOAD(C+6);__m256i c8 = LOAD(C+7);             __m256i d3 = LOAD(D+6);__m256i d8 = LOAD(D+7);
	__m256i c4 = LOAD(C+8);__m256i c9 = LOAD(C+9);             __m256i d4 = LOAD(D+8);__m256i d9 = LOAD(D+9);
	                                                                                                         
	__m256i l0,l1,l2,l3,l4,l5,l6,l7,l8,l9;                     __m256i _D_l0,_D_l1,_D_l2,_D_l3,_D_l4,_D_l5,_D_l6,_D_l7,_D_l8,_D_l9;
	__m256i m0,m1,m2,m3,m4,m5,m6,m7,m8,m9;                     __m256i _D_m0,_D_m1,_D_m2,_D_m3,_D_m4,_D_m5,_D_m6,_D_m7,_D_m8,_D_m9;
	                                                                                                         
	m0 = SHR(c0, BASE0_FP25519);                                  _D_m0 = SHR(d0, BASE0_FP25519);                     
	m1 = SHR(c1, BASE1_FP25519);                                  _D_m1 = SHR(d1, BASE1_FP25519);                     
	m2 = SHR(c2, BASE0_FP25519);                                  _D_m2 = SHR(d2, BASE0_FP25519);                     
	m3 = SHR(c3, BASE1_FP25519);                                  _D_m3 = SHR(d3, BASE1_FP25519);                     
	m4 = SHR(c4, BASE0_FP25519);                                  _D_m4 = SHR(d4, BASE0_FP25519);                     
	m5 = SHR(c5, BASE1_FP25519);                                  _D_m5 = SHR(d5, BASE1_FP25519);                     
	m6 = SHR(c6, BASE0_FP25519);                                  _D_m6 = SHR(d6, BASE0_FP25519);                     
	m7 = SHR(c7, BASE1_FP25519);                                  _D_m7 = SHR(d7, BASE1_FP25519);                     
	m8 = SHR(c8, BASE0_FP25519);                                  _D_m8 = SHR(d8, BASE0_FP25519);                     
	m9 = SHR(c9, BASE1_FP25519);                                  _D_m9 = SHR(d9, BASE1_FP25519);                     
	                                                                                                         
	l0 = AND(c0, mask26);                                      _D_l0 = AND(d0, mask26);                         
	l1 = AND(c1, mask25);                                      _D_l1 = AND(d1, mask25);                         
	l2 = AND(c2, mask26);                                      _D_l2 = AND(d2, mask26);                         
	l3 = AND(c3, mask25);                                      _D_l3 = AND(d3, mask25);                         
	l4 = AND(c4, mask26);                                      _D_l4 = AND(d4, mask26);                         
	l5 = AND(c5, mask25);                                      _D_l5 = AND(d5, mask25);                         
	l6 = AND(c6, mask26);                                      _D_l6 = AND(d6, mask26);                         
	l7 = AND(c7, mask25);                                      _D_l7 = AND(d7, mask25);                         
	l8 = AND(c8, mask26);                                      _D_l8 = AND(d8, mask26);                         
	l9 = AND(c9, mask25);                                      _D_l9 = AND(d9, mask25);                         
	                                                                                                         
	c0 = ADD(l0,MUL(m9,times19));                              d0 = ADD(_D_l0,MUL(_D_m9,times19));                 
	c1 = ADD(l1,m0);                                           d1 = ADD(_D_l1,_D_m0);                              
	c2 = ADD(l2,m1);                                           d2 = ADD(_D_l2,_D_m1);                              
	c3 = ADD(l3,m2);                                           d3 = ADD(_D_l3,_D_m2);                              
	c4 = ADD(l4,m3);                                           d4 = ADD(_D_l4,_D_m3);                              
	c5 = ADD(l5,m4);                                           d5 = ADD(_D_l5,_D_m4);                              
	c6 = ADD(l6,m5);                                           d6 = ADD(_D_l6,_D_m5);                              
	c7 = ADD(l7,m6);                                           d7 = ADD(_D_l7,_D_m6);                              
	c8 = ADD(l8,m7);                                           d8 = ADD(_D_l8,_D_m7);                              
	c9 = ADD(l9,m8);                                           d9 = ADD(_D_l9,_D_m8);                              
	                                                                                                         
	STORE(C+0,c0);	STORE(C+1,c5);                             STORE(D+0,d0);	STORE(D+1,d5);
	STORE(C+2,c1);	STORE(C+3,c6);                             STORE(D+2,d1);	STORE(D+3,d6);
	STORE(C+4,c2);	STORE(C+5,c7);                             STORE(D+4,d2);	STORE(D+5,d7);
	STORE(C+6,c3);	STORE(C+7,c8);                             STORE(D+6,d3);	STORE(D+7,d8);
	STORE(C+8,c4);	STORE(C+9,c9);                             STORE(D+8,d4);	STORE(D+9,d9);
}

/** Util Functions */

/**
 *
 * @param X_Y_Z_T
 */
static void random_Element_4w_h0h5(argElement_4w X_Y_Z_T)
{
	Element_1w_Fp25519 X,Y,Z,T;
	random_Element_1w_h0h5(X);
	random_Element_1w_h0h5(Y);
	random_Element_1w_h0h5(Z);
	random_Element_1w_h0h5(T);
	zip_Element_4w_h0h5(X_Y_Z_T,X,Y,Z,T);
}
/**
 *
 * @param X_Y_Z_T
 */
static void print_Element_4w_h0h5(argElement_4w X_Y_Z_T)
{
	Element_1w_Fp25519 X,Y,Z,T;
	unzip_Element_4w_h0h5(X,Y,Z,T,X_Y_Z_T);
	print_Element_1w_h0h5(X);
	print_Element_1w_h0h5(Y);
	print_Element_1w_h0h5(Z);
	print_Element_1w_h0h5(T);
}
/**
 *
 * @param X0_X1_X2_X3
 * @param Y0_Y1_Y2_Y3
 * @return
 */
static int compare_Element_4w_h0h5(argElement_4w X0_X1_X2_X3, argElement_4w Y0_Y1_Y2_Y3)
{
	Element_1w_Fp25519 X0,X1,X2,X3;
	Element_1w_Fp25519 Y0,Y1,Y2,Y3;
	unzip_Element_4w_h0h5(X0,X1,X2,X3,X0_X1_X2_X3);
	unzip_Element_4w_h0h5(Y0,Y1,Y2,Y3,Y0_Y1_Y2_Y3);
	return  compare_Element_1w_h0h5(X0,Y0) &&
			compare_Element_1w_h0h5(X1,Y1) &&
			compare_Element_1w_h0h5(X2,Y2) &&
			compare_Element_1w_h0h5(X3,Y3);
}

static __m256i * new_Element_4w_h0h5()
{
	return (__m256i*) allocate_bytes(NUM_DIGITS_FP25519 * sizeof(__m256i));
}

