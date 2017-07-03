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

#define copy_Element_1w_h0h8(C,A)\
	STORE(C+0,LOAD(A+0));\
	STORE(C+1,LOAD(A+1));\
	STORE(C+2,LOAD(A+2));\
	STORE(C+3,LOAD(A+3));

/**
 * Converts between STR_BYTES to SingleH0H5
 *
 * @param pC
 * @param p8A
 */
static void str_bytes_To_Element_1w_h0h8(uint64_t *__restrict pC, uint8_t *__restrict p8A)
{
	int i;
	const uint64_t mask = (((uint64_t)1)<<BASE_FP448)-1;
	const uint64_t *p64A = (uint64_t*)p8A;

	pC[0 ] =  p64A[0]>>0;
	pC[2 ] = (p64A[0]>>BASE_FP448);
	pC[4 ] = (p64A[0]>>(2*BASE_FP448)) | (p64A[1]<<8);

	pC[6 ] = (p64A[1]>>20);
	pC[8 ] = (p64A[1]>>(20+BASE_FP448)) | (p64A[2]<<16);

	pC[10] = (p64A[2]>>12);
	pC[12] = (p64A[2]>>(12+BASE_FP448)) | (p64A[3]<<24);

	pC[14] = (p64A[3]>>4);
	pC[1 ] = (p64A[3]>>(4+BASE_FP448));
	pC[3 ] = (p64A[3]>>(4+2*BASE_FP448)) | (p64A[4]<<4);

	pC[5 ] = (p64A[4]>>24);
	pC[7 ] = (p64A[4]>>(24+BASE_FP448)) | (p64A[5]<<12);

	pC[9 ] = (p64A[5]>>16);
	pC[11] = (p64A[5]>>(16+BASE_FP448)) | (p64A[6]<<20);

	pC[13] = (p64A[6]>>8);
	pC[15] = (p64A[6]>>(8+BASE_FP448));

	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		pC[i] &= mask;
	}
}


/**
 *
 * @param p8C
 * @param puA
 */
static void singleH0H8_To_str_bytes(uint8_t *p8C, uint64_t *puA)
{
	/**
	* It only accepts positive coefficients.
	* In order to remove negative coefficients, compute compress()
	*/
	int i;
	int64_t *pA = (int64_t*)puA;
	__int128_t a[NUM_DIGITS_FP448];
	__int128_t c[7];
	int64_t * p64C = (int64_t*)p8C;
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

	for(i=0;i<7;i++)
	{
		p64C[i]=c[i];
	}
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void add_Element_1w_h0h8(uint64_t *pC, uint64_t *pA, uint64_t *pB)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/4);i++)
	{
		__m256i A = LOAD(pA+i);
		__m256i B = LOAD(pB+i);
		STORE(pC+i,ADD(A,B));
	}
}

static const Element_1w_Fp448 CONST_2P_ELEMENT = {
		0x1ffffffe,0x3ffffffc,
		0x1ffffffe,0x1ffffffc,
		0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe,
		0x1ffffffe,0x1ffffffe};

/**
 *
 * @param pA
 */
static inline void neg_Element_1w_h0h8(uint64_t *pA)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/4);i++)
	{
		__m256i A = LOAD(pA+i);
		__m256i _2P = LOAD(CONST_2P_ELEMENT+i);
		STORE(pA+i,SUB(_2P,A));
	}
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void sub_Element_1w_h0h8(uint64_t *pC, uint64_t *pA, uint64_t *pB)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/4);i++)
	{
		__m256i A = LOAD(pA+i);
		__m256i B = LOAD(pB+i);
		__m256i _2P = LOAD(CONST_2P_ELEMENT+i);
		STORE(pC+i,ADD(A,SUB(_2P,B)));
	}
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_schoolbook_Element_1w_h0h8(uint64_t *C, uint64_t *A, uint64_t *B)
{
	int i;
	__m256i ai,tmp;
	__m256i b0,b1,b2,b3,b4,b5,b6,b7;
	__m256i c0,c1,c2,c3,c4,c5,c6,c7;

	__m256i y0 = LOAD(B+0);
	__m256i y1 = LOAD(B+1);
	__m256i y2 = LOAD(B+2);
	__m256i y3 = LOAD(B+3);

	__m256i x0 = ADD(BLEND32(y0, ZERO, 0x33), SHUF(y0, 0x4E));
	__m256i x1 = ADD(BLEND32(y1, ZERO, 0x33), SHUF(y1, 0x4E));
	__m256i x2 = ADD(BLEND32(y2, ZERO, 0x33), SHUF(y2, 0x4E));
	__m256i x3 = ADD(BLEND32(y3, ZERO, 0x33), SHUF(y3, 0x4E));

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
		b0 = ADD(BLEND32(tmp, ZERO, 0x33), SHUF(tmp, 0x4E));

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
		b0 = ADD(BLEND32(tmp, ZERO, 0x33), SHUF(tmp, 0x4E));
	}

	__m128i p0 = _mm_add_epi64(_mm256_extracti128_si256(c0,1),_mm256_castsi256_si128(c0));
	__m128i p1 = _mm_add_epi64(_mm256_extracti128_si256(c1,1),_mm256_castsi256_si128(c1));
	__m128i p2 = _mm_add_epi64(_mm256_extracti128_si256(c2,1),_mm256_castsi256_si128(c2));
	__m128i p3 = _mm_add_epi64(_mm256_extracti128_si256(c3,1),_mm256_castsi256_si128(c3));
	__m128i p4 = _mm_add_epi64(_mm256_extracti128_si256(c4,1),_mm256_castsi256_si128(c4));
	__m128i p5 = _mm_add_epi64(_mm256_extracti128_si256(c5,1),_mm256_castsi256_si128(c5));
	__m128i p6 = _mm_add_epi64(_mm256_extracti128_si256(c6,1),_mm256_castsi256_si128(c6));
	__m128i p7 = _mm_add_epi64(_mm256_extracti128_si256(c7,1),_mm256_castsi256_si128(c7));

	c0 = _mm256_insertf128_si256(_mm256_castsi128_si256(p0),p1,1);
	c1 = _mm256_insertf128_si256(_mm256_castsi128_si256(p2),p3,1);
	c2 = _mm256_insertf128_si256(_mm256_castsi128_si256(p4),p5,1);
	c3 = _mm256_insertf128_si256(_mm256_castsi128_si256(p6),p7,1);

	STORE(C+0,c0);
	STORE(C+1,c1);
	STORE(C+2,c2);
	STORE(C+3,c3);
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void mul_Element_1w_h0h8(uint64_t *pC, uint64_t *pA, uint64_t *pB)
{
	mul_schoolbook_Element_1w_h0h8(pC, pA, pB);
}

/**
 *
 * @param A
 * @param times
 */
static void sqrn_Element_1w_h0h8(uint64_t * A, const int times)
{
	int i;
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask  = _mm256_set1_epi64x(ones);

	__m256i ai,aj,a2i,a2j;
	__m256i b0,b1,b2,b3,b4,b5,b6,b7;
	__m256i c0,c1,c2,c3,c4,c5,c6,c7;
	__m256i d4,d5,d6,d7;
	__m256i y0,y1,y2,y3;

	c0 = LOAD(A+0);
	c1 = LOAD(A+1);
	c2 = LOAD(A+2);
	c3 = LOAD(A+3);

	for(i=0;i<times;i++)
	{
		y0 = c0;
		y1 = c1;
		y2 = c2;
		y3 = c3;

		__m256i x0 = ADD(BLEND32(y0, ZERO, 0x33), SHUF(y0, 0x4E));
		__m256i x1 = ADD(BLEND32(y1, ZERO, 0x33), SHUF(y1, 0x4E));
		__m256i x2 = ADD(BLEND32(y2, ZERO, 0x33), SHUF(y2, 0x4E));
		__m256i x3 = ADD(BLEND32(y3, ZERO, 0x33), SHUF(y3, 0x4E));

		b0 = PERM128(y0,x0,0x20);
		b1 = PERM128(y0,x0,0x31);
		b2 = PERM128(y1,x1,0x20);
		b3 = PERM128(y1,x1,0x31);
		b4 = PERM128(y2,x2,0x20);
		b5 = PERM128(y2,x2,0x31);
		b6 = PERM128(y3,x3,0x20);
		b7 = PERM128(y3,x3,0x31);

		d7 = SHUF(ADD(b7,SHR_128(b7)),0x4E);

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

		d6 = SHUF(ADD(b6,SHR_128(b6)),0x4E);
		d5 = SHUF(ADD(b5,SHR_128(b5)),0x4E);

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

		d4 = SHUF(ADD(b4,SHR_128(b4)),0x4E);

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

		__m128i p0 = _mm_add_epi64(_mm256_extracti128_si256(c0,1),_mm256_castsi256_si128(c0));
		__m128i p1 = _mm_add_epi64(_mm256_extracti128_si256(c1,1),_mm256_castsi256_si128(c1));
		__m128i p2 = _mm_add_epi64(_mm256_extracti128_si256(c2,1),_mm256_castsi256_si128(c2));
		__m128i p3 = _mm_add_epi64(_mm256_extracti128_si256(c3,1),_mm256_castsi256_si128(c3));
		__m128i p4 = _mm_add_epi64(_mm256_extracti128_si256(c4,1),_mm256_castsi256_si128(c4));
		__m128i p5 = _mm_add_epi64(_mm256_extracti128_si256(c5,1),_mm256_castsi256_si128(c5));
		__m128i p6 = _mm_add_epi64(_mm256_extracti128_si256(c6,1),_mm256_castsi256_si128(c6));
		__m128i p7 = _mm_add_epi64(_mm256_extracti128_si256(c7,1),_mm256_castsi256_si128(c7));

		c0 = _mm256_inserti128_si256(_mm256_castsi128_si256(p0),p1,1);
		c1 = _mm256_inserti128_si256(_mm256_castsi128_si256(p2),p3,1);
		c2 = _mm256_inserti128_si256(_mm256_castsi128_si256(p4),p5,1);
		c3 = _mm256_inserti128_si256(_mm256_castsi128_si256(p6),p7,1);

		/* compress */

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

		_m3 = SHUF(ADD(m3,SHR_128(m3)),0x4E);
		h3 = SHUF(ADD(h3,SHR_128(h3)),0x4E);

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

/**
 *
 * @param A
 */
static void sqr_Element_1w_h0h8(uint64_t *A)
{
	sqrn_Element_1w_h0h8(A, 1);
}

/**
 *
 * @param A
 */
static void compress_Element_1w_h0h8(uint64_t *A)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m128i mask = _mm_set_epi32(0, ones, 0, ones);

	__m128i c0 = _mm_load_si128((__m128i*)A+0);
	__m128i c1 = _mm_load_si128((__m128i*)A+1);
	__m128i c2 = _mm_load_si128((__m128i*)A+2);
	__m128i c3 = _mm_load_si128((__m128i*)A+3);
	__m128i c4 = _mm_load_si128((__m128i*)A+4);
	__m128i c5 = _mm_load_si128((__m128i*)A+5);
	__m128i c6 = _mm_load_si128((__m128i*)A+6);
	__m128i c7 = _mm_load_si128((__m128i*)A+7);

	__m128i h0_h8,  h1_h9,  h2_h10, h3_h11,
			h4_h12, h5_h13, h6_h14, h7_h15;

	h0_h8 = _mm_srli_epi64(c0, BASE_FP448);
	c0 = _mm_and_si128(c0, mask);
	c1 = _mm_add_epi64(c1, h0_h8);

	h1_h9 = _mm_srli_epi64(c1, BASE_FP448);
	c1 = _mm_and_si128(c1, mask);
	c2 = _mm_add_epi64(c2, h1_h9);

	h2_h10 = _mm_srli_epi64(c2, BASE_FP448);
	c2 = _mm_and_si128(c2, mask);
	c3 = _mm_add_epi64(c3, h2_h10);

	h3_h11 = _mm_srli_epi64(c3, BASE_FP448);
	c3 = _mm_and_si128(c3, mask);
	c4 = _mm_add_epi64(c4, h3_h11);

	h4_h12 = _mm_srli_epi64(c4, BASE_FP448);
	c4 = _mm_and_si128(c4, mask);
	c5 = _mm_add_epi64(c5, h4_h12);

	h5_h13 = _mm_srli_epi64(c5, BASE_FP448);
	c5 = _mm_and_si128(c5, mask);
	c6 = _mm_add_epi64(c6, h5_h13);

	h6_h14 = _mm_srli_epi64(c6, BASE_FP448);
	c6 = _mm_and_si128(c6, mask);
	c7 = _mm_add_epi64(c7, h6_h14);

	h7_h15 = _mm_srli_epi64(c7, BASE_FP448);
	c7 = _mm_and_si128(c7, mask);

	/**
	 * LOW  h7  ->  h0
	 * HIGH h15 ->  h8
	 **/
	c0 = _mm_add_epi64(c0,_mm_blend_epi32(h7_h15,_mm_setzero_si128(),0x3));
	/**
	 * LOW  h7   \/  h0
	 * HIGH h15  /\  h8
	 **/
	c0 = _mm_add_epi64(c0,_mm_shuffle_epi32(h7_h15,0x4E));

	h0_h8 = _mm_srli_epi64(c0, BASE_FP448);
	c0 = _mm_and_si128(c0, mask);
	c1 = _mm_add_epi64(c1, h0_h8);

	_mm_store_si128((__m128i*)A+0,c0);
	_mm_store_si128((__m128i*)A+1,c1);
	_mm_store_si128((__m128i*)A+2,c2);
	_mm_store_si128((__m128i*)A+3,c3);
	_mm_store_si128((__m128i*)A+4,c4);
	_mm_store_si128((__m128i*)A+5,c5);
	_mm_store_si128((__m128i*)A+6,c6);
	_mm_store_si128((__m128i*)A+7,c7);
}

/**
 *
 * @param pA
 */
static void compressfast_Element_1w_h0h8(uint64_t *pA)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask  = _mm256_set1_epi64x(ones);

	__m256i A0,A1,A2,A3;
	__m256i l0,l1,l2,l3;
	__m256i m0,m1,m2,m3;
	__m256i _m0,_m1,_m2,_m3;
	__m256i h0,h1,h2,h3;
	__m256i c0,c1,c2,c3;

	A0 = LOAD(pA+0);
	A1 = LOAD(pA+1);
	A2 = LOAD(pA+2);
	A3 = LOAD(pA+3);

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

	_m3 = SHUF(ADD(m3,SHR_128(m3)),0x4E);
	h3 = SHUF(ADD(h3,SHR_128(h3)),0x4E);

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

	STORE(pA+0,c0);
	STORE(pA+1,c1);
	STORE(pA+2,c2);
	STORE(pA+3,c3);
}

/**
 *
 * @param pA
 */
static void new_compressfast_Element_1w_h0h8(uint64_t * pA)
{
	const uint64_t ones = ((uint64_t) 1 << BASE_FP448) - 1;
	const __m256i mask  = _mm256_set1_epi64x(ones);

	__m256i A0,A1,A2,A3;
	__m256i l0,l1,l2,l3;
	__m256i m0,m1,m2,m3;
	__m256i _m0,_m1,_m2,_m3;
	__m256i c0,c1,c2,c3;

	A0 = LOAD(pA+0);
	A1 = LOAD(pA+1);
	A2 = LOAD(pA+2);
	A3 = LOAD(pA+3);

	l0 = AND(A0,mask);
	l1 = AND(A1,mask);
	l2 = AND(A2,mask);
	l3 = AND(A3,mask);

	m0 = SHR(A0,BASE_FP448);
	m1 = SHR(A1,BASE_FP448);
	m2 = SHR(A2,BASE_FP448);
	m3 = SHR(A3,BASE_FP448);

	_m3 = SHUF(ADD(m3,SHR_128(m3)),0x4E);

	_m0 = PERM128(_m3,m0,0x21);
	_m1 = PERM128(m0,m1,0x21);
	_m2 = PERM128(m1,m2,0x21);
	_m3 = PERM128(m2,m3,0x21);

	c0 = ADD(l0,_m0);
	c1 = ADD(l1,_m1);
	c2 = ADD(l2,_m2);
	c3 = ADD(l3,_m3);

	STORE(pA+0,c0);
	STORE(pA+1,c1);
	STORE(pA+2,c2);
	STORE(pA+3,c3);
}

/**
 *
 * @param pC
 * @param pA
 * @param only_inverse
 */
static void invsqrt_Element_1w_h0h8(uint64_t * __restrict pC, uint64_t * __restrict pA, const int only_inverse)
{
	Element_1w_Fp448 x0,x1;
	uint64_t * T[4];

	T[0] = x0;
	T[1] = pC;
	T[2] = x1;
	T[3] = pA;

	copy_Element_1w_h0h8(T[1],T[3]);
	sqrn_Element_1w_h0h8(T[1],1);
	mul_Element_1w_h0h8(T[1],T[1],T[3]);
	compressfast_Element_1w_h0h8(T[1]);

	copy_Element_1w_h0h8(T[0],T[1]);
	sqrn_Element_1w_h0h8(T[0],1);
	mul_Element_1w_h0h8(T[0],T[0],T[3]);
	compressfast_Element_1w_h0h8(T[0]);

	copy_Element_1w_h0h8(T[1],T[0]);
	sqrn_Element_1w_h0h8(T[1],3);
	mul_Element_1w_h0h8(T[1],T[1],T[0]);
	compressfast_Element_1w_h0h8(T[1]);

	copy_Element_1w_h0h8(T[2],T[1]);
	sqrn_Element_1w_h0h8(T[2],6);
	mul_Element_1w_h0h8(T[2],T[2],T[1]);
	compressfast_Element_1w_h0h8(T[2]);

	copy_Element_1w_h0h8(T[1],T[2]);
	sqrn_Element_1w_h0h8(T[1],12);
	mul_Element_1w_h0h8(T[1],T[1],T[2]);
	compressfast_Element_1w_h0h8(T[1]);

	sqrn_Element_1w_h0h8(T[1],3);
	mul_Element_1w_h0h8(T[1],T[1],T[0]);
	compressfast_Element_1w_h0h8(T[1]);

	copy_Element_1w_h0h8(T[2],T[1]);
	sqrn_Element_1w_h0h8(T[2],27);
	mul_Element_1w_h0h8(T[2],T[2],T[1]);
	compressfast_Element_1w_h0h8(T[2]);

	copy_Element_1w_h0h8(T[1],T[2]);
	sqrn_Element_1w_h0h8(T[1],54);
	mul_Element_1w_h0h8(T[1],T[1],T[2]);
	compressfast_Element_1w_h0h8(T[1]);

	sqrn_Element_1w_h0h8(T[1],3);
	mul_Element_1w_h0h8(T[1],T[1],T[0]);
	compressfast_Element_1w_h0h8(T[1]);

	copy_Element_1w_h0h8(T[2],T[1]);
	sqrn_Element_1w_h0h8(T[2],111);
	mul_Element_1w_h0h8(T[2],T[2],T[1]);
	compressfast_Element_1w_h0h8(T[2]);

	copy_Element_1w_h0h8(T[1],T[2]);
	sqrn_Element_1w_h0h8(T[1],1);
	mul_Element_1w_h0h8(T[1],T[1],T[3]);
	compressfast_Element_1w_h0h8(T[1]);

	sqrn_Element_1w_h0h8(T[1],223);
	mul_Element_1w_h0h8(T[1],T[1],T[2]);
	compressfast_Element_1w_h0h8(T[1]);

	if(only_inverse != 0)
	{
		sqrn_Element_1w_h0h8(T[1],2);
		mul_Element_1w_h0h8(T[1],T[1],T[3]);
		compressfast_Element_1w_h0h8(T[1]);
	}
}

/**
 *
 * @param pC
 * @param pA
 */
static void inv_Element_1w_h0h8(uint64_t * pC, uint64_t * pA)
{
	invsqrt_Element_1w_h0h8(pC,pA,1);
}

/**
 *
 * @param pC
 * @param pA
 */
static void sqrt_Element_1w_h0h8(uint64_t * pC, uint64_t * pA)
{
	invsqrt_Element_1w_h0h8(pC,pA,0);
}

/*** Util functions ***/
/**
 *
 * @param A
 */
static void random_Element_1w_h0h8(uint64_t *A)
{
	ALIGN uint8_t a[SIZE_FP448];
	random_bytes(a,SIZE_FP448);
	a[SIZE_FP448-1] = 0x0;
	str_bytes_To_Element_1w_h0h8(A, a);
}

/**
 *
 * @param A
 * @param B
 * @return
 */
static int compare_Element_1w_h0h8(uint64_t *A, uint64_t *B)
{
	ALIGN uint8_t a[SIZE_FP448];
	ALIGN uint8_t b[SIZE_FP448];

	compress_Element_1w_h0h8(A);
	compress_Element_1w_h0h8(B);
	singleH0H8_To_str_bytes(a, A);
	singleH0H8_To_str_bytes(b, B);
	return compare_bytes(a,b,SIZE_FP448);
}

/**
 *
 * @param A
 */
static void print_Element_1w_h0h8(uint64_t *A)
{
	ALIGN uint8_t a[SIZE_FP448];
	singleH0H8_To_str_bytes(a, A);
	print_bytes(a,SIZE_FP448);
}

/**
 *
 * @return
 */
static uint64_t * init_Element_1w_h0h8()
{
	return (uint64_t*) allocate_bytes(NUM_DIGITS_FP448 * sizeof(uint64_t));
}

/**
 *
 * @return
 */
static uint64_t * prime_Element_1w_h0h8()
{
	uint64_t *prime = init_Element_1w_h0h8();
	prime[ 0] = 0xfffffff;		prime[ 1] = 0xffffffe;
	prime[ 2] = 0xfffffff;		prime[ 3] = 0xfffffff;
	prime[ 4] = 0xfffffff;		prime[ 5] = 0xfffffff;
	prime[ 6] = 0xfffffff;		prime[ 7] = 0xfffffff;
	prime[ 8] = 0xfffffff;		prime[ 9] = 0xfffffff;
	prime[10] = 0xfffffff;		prime[11] = 0xfffffff;
	prime[12] = 0xfffffff;		prime[13] = 0xfffffff;
	prime[14] = 0xfffffff;		prime[15] = 0xfffffff;

	return prime;
}

