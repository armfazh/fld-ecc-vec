/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_crypto_avx2.
 *
 * faz_crypto_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_crypto_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_crypto_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Converts between STR_BYTES to 1w_H0H5
 *
 * @param pC
 * @param p8A
 */
static void str_bytes_To_Element_1w_h0h5(uint64_t * pC, uint8_t * p8A)
{
	const uint64_t mask0 = ((uint64_t)1<<(BASE0_FP25519))-1;
	const uint64_t mask1 = ((uint64_t)1<<(BASE1_FP25519))-1;
	const __m128i vmask0 = _mm_set_epi64x(mask1,mask0);
	const __m128i vmask1 = _mm_set_epi64x(mask0,mask1);
	__m128i xA,xF;
	__m128i z0,z1,z2,z3,z4;
	__m128i xB,xC,xD,xE,xG,xH,xI,xJ;
	ALIGN uint64_t pA[4];
	uint64_t last;

	pA[0] = *((uint64_t*)p8A+0);
	pA[1] = *((uint64_t*)p8A+1);
	pA[2] = *((uint64_t*)p8A+2);
	pA[3] = *((uint64_t*)p8A+3);

	last = pA[3]>>63;
	pA[3] &= ((uint64_t)1<<63)-1;
	pA[0] += 19*last;

	xA = _mm_load_si128((__m128i*)pA+0);
	xF = _mm_load_si128((__m128i*)pA+1);

	xC = _mm_srli_epi64(_mm_srli_si128(xA,(BASE0_FP25519+BASE1_FP25519)/8),(BASE0_FP25519+BASE1_FP25519)%8);
	xH = _mm_srli_epi64(_mm_srli_si128(xF,(BASE0_FP25519+BASE1_FP25519)/8),(BASE0_FP25519+BASE1_FP25519)%8);
	xE = _mm_srli_epi64(_mm_srli_si128(xA,(2*(BASE0_FP25519+BASE1_FP25519))/8),(2*(BASE0_FP25519+BASE1_FP25519))%8);
	xJ = _mm_srli_epi64(_mm_srli_si128(xF,(2*(BASE0_FP25519+BASE1_FP25519))/8),(2*(BASE0_FP25519+BASE1_FP25519))%8);

	xB = _mm_srli_epi64(xA,BASE0_FP25519);
	xG = _mm_srli_epi64(xF,BASE1_FP25519);
	xD = _mm_srli_epi64(xC,BASE0_FP25519);
	xI = _mm_srli_epi64(xH,BASE1_FP25519);

	z0 = _mm_unpacklo_epi64(xA,xF);
	z1 = _mm_unpacklo_epi64(xB,xG);
	z2 = _mm_unpacklo_epi64(xC,xH);
	z3 = _mm_unpacklo_epi64(xD,xI);
	z4 = _mm_unpacklo_epi64(xE,xJ);

	z0 = _mm_and_si128(z0,vmask0);
	z1 = _mm_and_si128(z1,vmask1);
	z2 = _mm_and_si128(z2,vmask0);
	z3 = _mm_and_si128(z3,vmask1);
	z4 = _mm_and_si128(z4,vmask0);

	_mm_store_si128((__m128i*)pC+0,z0);
	_mm_store_si128((__m128i*)pC+1,z1);
	_mm_store_si128((__m128i*)pC+2,z2);
	_mm_store_si128((__m128i*)pC+3,z3);
	_mm_store_si128((__m128i*)pC+4,z4);
	_mm_store_si128((__m128i*)pC+5,_mm_setzero_si128());
}

/**
 *
 * @param p8C
 * @param pA
 */
static void singleH0H5_To_str_bytes(uint8_t * p8C, uint64_t * pA)
{
	uint64_t * pC = (uint64_t*)p8C;
	__int128_t a0,a1,a2,a3,a4;

	a0  = (__int128_t) pA[0];
	a0 += (__int128_t)(pA[2] << BASE0_FP25519);
	a0 += (__int128_t)(pA[4] << (BASE0_FP25519+BASE1_FP25519));
	pC[0] = (uint64_t)a0;

	a1  = a0 >> 64;
	a1 += (__int128_t) (((int64_t)pA[4]) >> 13);
	a1 += (__int128_t) (pA[6] << 13);
	a1 += (__int128_t) (pA[8] << (BASE1_FP25519+13));
	a1 += (__int128_t) (((int64_t)pA[2]) >> (BASE1_FP25519+13));
	a1 += (__int128_t) (((int64_t)pA[0]) >> 63); /*carry*/
	pC[1] = (uint64_t)a1;

	a2  = a1 >> 64;
	a2 += (__int128_t) pA[1];
	a2 += (__int128_t) (pA[3] << BASE1_FP25519);
	a2 += (__int128_t) (pA[5] << (BASE0_FP25519+BASE1_FP25519));
	a2 += (__int128_t) (((int64_t)pA[8]) >> BASE0_FP25519);
	a2 += (__int128_t) (((int64_t)pA[6]) >> (BASE0_FP25519+BASE1_FP25519));
	pC[2] = (uint64_t)a2;

	a3  = a2 >> 64;
	a3 += (__int128_t) (pA[7] << 12);
	a3 += (__int128_t) (pA[9] << (BASE0_FP25519+12));
	a3 += (__int128_t) (((int64_t)pA[5]) >> 13);
	a3 += (__int128_t) (((int64_t)pA[3]) >> (BASE0_FP25519+13));
	a3 += (__int128_t) (((int64_t)pA[1]) >> 63); /*carry*/
	pC[3] = (uint64_t)a3;
	pC[3] &= 0x7FFFFFFFFFFFFFFF;


	a4  = a3 >> 64;
	a4 += (__int128_t) (((int64_t)pA[7]) >> (BASE0_FP25519+BASE1_FP25519));
	a4 += (__int128_t) (((int64_t)pA[9]) >> BASE1_FP25519);

	pC[0] += 19 *( /*(a4<<1) |*/ (a3>>63));

}

/**
 *
 * @param pA
 */
static void neg_Element_1w_h0h5(uint64_t * pA)
{
	const __m256i _2P0 = _mm256_set_epi64x(0x7fffffe,0x3fffffe,0x3fffffe,0x7ffffda);
	const __m256i _2P1 = _mm256_set_epi64x(0x7fffffe,0x3fffffe,0x3fffffe,0x7fffffe);
	const __m256i _2P2 = _mm256_set_epi64x(0x0000000,0x0000000,0x3fffffe,0x7fffffe);

	__m256i a0 = LOAD(pA+0);
	__m256i a1 = LOAD(pA+1);
	__m256i a2 = LOAD(pA+2);

	__m256i c0 = SUB(_2P0,a0);
	__m256i c1 = SUB(_2P1,a1);
	__m256i c2 = SUB(_2P2,a2);

	STORE(pA+0,c0);
	STORE(pA+1,c1);
	STORE(pA+2,c2);
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void add_Element_1w_h0h5(uint64_t * pC, uint64_t * pA,uint64_t * pB)
{
	__m256i a0 = LOAD(pA+0);
	__m256i a1 = LOAD(pA+1);
	__m256i a2 = LOAD(pA+2);

	__m256i b0 = LOAD(pB+0);
	__m256i b1 = LOAD(pB+1);
	__m256i b2 = LOAD(pB+2);

	__m256i c0 = ADD(a0,b0);
	__m256i c1 = ADD(a1,b1);
	__m256i c2 = ADD(a2,b2);

	STORE(pC+0,c0);
	STORE(pC+1,c1);
	STORE(pC+2,c2);
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void sub_Element_1w_h0h5(uint64_t * pC, uint64_t * pA,uint64_t * pB)
{
	const __m256i _2P0 = _mm256_set_epi64x(0x7fffffe,0x3fffffe,0x3fffffe,0x7ffffda);
	const __m256i _2P1 = _mm256_set_epi64x(0x7fffffe,0x3fffffe,0x3fffffe,0x7fffffe);
	const __m256i _2P2 = _mm256_set_epi64x(0x0000000,0x0000000,0x3fffffe,0x7fffffe);

	__m256i a0 = LOAD(pA+0);
	__m256i a1 = LOAD(pA+1);
	__m256i a2 = LOAD(pA+2);

	__m256i b0 = LOAD(pB+0);
	__m256i b1 = LOAD(pB+1);
	__m256i b2 = LOAD(pB+2);

	__m256i c0 = SUB(_2P0,b0);
	__m256i c1 = SUB(_2P1,b1);
	__m256i c2 = SUB(_2P2,b2);

	c0 = ADD(a0,c0);
	c1 = ADD(a1,c1);
	c2 = ADD(a2,c2);

	STORE(pC+0,c0);
	STORE(pC+1,c1);
	STORE(pC+2,c2);
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_schoolbook_Element_1w_h0h5(uint64_t * C, uint64_t * A, uint64_t * B)
{
	const __m256i k_19_1 = _mm256_set_epi32(0,1,0,19,0,1,0,19);
	const __m256i k_1_19_1_1 = _mm256_set_epi64x(1,19,1,1);

	__m256i ai, aH, aL;
	__m256i B0,B1,B2;
	__m256i b0,b1,b2,b3,b4;
	__m256i    d1,d2,d3,d4;
	__m256i c0,c1,c2,c3,c4;
	__m128i p0,p1,p2,p3,p4;

	B0 = LOAD(B+0);
	B1 = LOAD(B+1);
	B2 = LOAD(B+2);

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

	ai = PERM64(LOAD(A+0),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	c0 = MUL(b0, aL);
	c1 = MUL(b1, aH);
	c2 = MUL(b2, aL);
	c3 = MUL(b3, aH);
	c4 = MUL(b4, aL);

	ai = PERM64(LOAD(A+0), 0xFA);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x03);
	aH = BLEND32(ai, aH, 0x0C);

	d4 = _mm256_shuffle_epi32(b4,0x4E);
	d4 = MUL(d4,k_19_1);

	c0 = ADD(c0,MUL(d4, aL));
	c1 = ADD(c1,MUL(b0, aH));
	c2 = ADD(c2,MUL(b1, aL));
	c3 = ADD(c3,MUL(b2, aH));
	c4 = ADD(c4,MUL(b3, aL));

	ai = PERM64(LOAD(A+1),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	d3 = _mm256_shuffle_epi32(b3,0x4E);
	d3 = MUL(d3,k_19_1);

	c0 = ADD(c0,MUL(d3, aL));
	c1 = ADD(c1,MUL(d4, aH));
	c2 = ADD(c2,MUL(b0, aL));
	c3 = ADD(c3,MUL(b1, aH));
	c4 = ADD(c4,MUL(b2, aL));

	ai = PERM64(LOAD(A + 1), 0xFA);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x03);
	aH = BLEND32(ai, aH, 0x0C);

	d2 = _mm256_shuffle_epi32(b2,0x4E);
	d2 = MUL(d2,k_19_1);

	c0 = ADD(c0,MUL(d2, aL));
	c1 = ADD(c1,MUL(d3, aH));
	c2 = ADD(c2,MUL(d4, aL));
	c3 = ADD(c3,MUL(b0, aH));
	c4 = ADD(c4,MUL(b1, aL));

	ai = PERM64(LOAD(A+2),0x50);
	aH = ADD(ai, ai);
	aL = BLEND32(ai, aH, 0x30);
	aH = BLEND32(ai, aH, 0xC0);

	d1 = _mm256_shuffle_epi32(b1,0x4E);
	d1 = MUL(d1,k_19_1);

	c0 = ADD(c0,MUL(d1, aL));
	c1 = ADD(c1,MUL(d2, aH));
	c2 = ADD(c2,MUL(d3, aL));
	c3 = ADD(c3,MUL(d4, aH));
	c4 = ADD(c4,MUL(b0, aL));

	p0 = _mm_add_epi64(_mm256_extracti128_si256(c0,1),_mm256_castsi256_si128(c0));
	p1 = _mm_add_epi64(_mm256_extracti128_si256(c1,1),_mm256_castsi256_si128(c1));
	p2 = _mm_add_epi64(_mm256_extracti128_si256(c2,1),_mm256_castsi256_si128(c2));
	p3 = _mm_add_epi64(_mm256_extracti128_si256(c3,1),_mm256_castsi256_si128(c3));
	p4 = _mm_add_epi64(_mm256_extracti128_si256(c4,1),_mm256_castsi256_si128(c4));

	c0 = _mm256_insertf128_si256(_mm256_castsi128_si256(p0),p1,1);
	c1 = _mm256_insertf128_si256(_mm256_castsi128_si256(p2),p3,1);
	c2 = _mm256_castsi128_si256(p4);

	STORE(C+0,c0);
	STORE(C+1,c1);
	STORE(C+2,c2);
}

/**
 *
 * @param pC
 * @param pA
 * @param pB
 */
static void mul_Element_1w_h0h5(uint64_t * pC, uint64_t * pA, uint64_t * pB)
{
	mul_schoolbook_Element_1w_h0h5(pC,pA,pB);
}

#define mul19_128(A)\
	A = _mm_shuffle_epi32(_mm_add_epi64(_mm_add_epi64(_mm_sllv_epi64(A, shift_4),_mm_sllv_epi64(A, shift_1)),A),0x4E);

/**
 *
 * @param A
 * @param times
 */
static void sqrn_Element_1w_h0h5(uint64_t * A, int times)
{
	int it=0;
	const __m128i k_19_1 = _mm_set_epi32(0,1,0,19);

	__m128i ai,aj,ae,ao,ak,al;
	__m128i b0,b1,b2,b3,b4;
	__m128i d0,d1,d2,d3,d4;
	__m128i c0,c1,c2,c3,c4;

	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m128i mask0 = _mm_set_epi32(0, ones25, 0, ones26);
	const __m128i mask1 = _mm_set_epi32(0, ones26, 0, ones25);
	const __m128i shift0 = _mm_set_epi32(0, 25, 0, 26);
	const __m128i shift1 = _mm_set_epi32(0, 26, 0, 25);
	const __m128i shift_4 = _mm_set_epi32(0,4,0,64);
	const __m128i shift_1 = _mm_set_epi32(0,1,0,64);

	b0 = _mm_load_si128((__m128i*)A+0);
	b1 = _mm_load_si128((__m128i*)A+1);
	b2 = _mm_load_si128((__m128i*)A+2);
	b3 = _mm_load_si128((__m128i*)A+3);
	b4 = _mm_load_si128((__m128i*)A+4);

	for(it=0;it<times;it++)
	{
		__m128i A0,A1,A2,A3,A4;
		__m128i h0_5, h1_6, h2_7, h3_8, h4_9;

		A0 = b0;
		A1 = b1;
		A2 = b2;
		A3 = b3;
		A4 = b4;
		ai = _mm_shuffle_epi32(A0, 0x44);		aj = _mm_add_epi64(ai, ai);/* 2a0 */
		ak = _mm_shuffle_epi32(A1, 0x44);		al = _mm_add_epi64(ak, ak);/* 2a1 */
		ak = _mm_blend_epi32(ak, al, 0x3);		ao = _mm_add_epi64(ak, ak);
		ae = _mm_shuffle_epi32(ao, 0x4E);

		d4 = _mm_shuffle_epi32(b4, 0x4E);
		d4 = _mm_mul_epu32(d4, k_19_1);

		c0 = _mm_mul_epu32(b0, ai);		c0 = _mm_add_epi64(c0, _mm_mul_epu32(d4, ao));
		c1 = _mm_mul_epu32(b1, aj);     /*c1 = _mm_add_epi64(c1, _mm_mul_epu32(b0, --));*/
		c2 = _mm_mul_epu32(b2, aj);		c2 = _mm_add_epi64(c2, _mm_mul_epu32(b1, ak));
		c3 = _mm_mul_epu32(b3, aj);		c3 = _mm_add_epi64(c3, _mm_mul_epu32(b2, ae));
		c4 = _mm_mul_epu32(b4, aj);		c4 = _mm_add_epi64(c4, _mm_mul_epu32(b3, ao));

		ai = _mm_shuffle_epi32(A2, 0x44);		aj = _mm_add_epi64(ai, ai);/* 2a2 */
		ak = _mm_shuffle_epi32(A3, 0x44);		al = _mm_add_epi64(ak, ak);/* 2a3 */
		ak = _mm_blend_epi32(ak, al, 0xC);		ao = _mm_add_epi64(ak, ak);
		ao = _mm_shuffle_epi32(ao, 0x4E);

		d3 = _mm_shuffle_epi32(b3, 0x4E);
		d2 = _mm_shuffle_epi32(b2, 0x4E);

		d3 = _mm_mul_epu32(d3, k_19_1);
		d2 = _mm_mul_epu32(d2, k_19_1);
		c0 = _mm_add_epi64(c0, _mm_mul_epu32(d3, aj));     /*c0 = _mm_add_epi64(c0,_mm_mul_epu32(d2,--));*/
		c1 = _mm_add_epi64(c1, _mm_mul_epu32(d4, aj));	   c1 = _mm_add_epi64(c1, _mm_mul_epu32(d3, ak));
		/*c2 = _mm_add_epi64(c2,_mm_mul_epu32(b0,--));*/   c2 = _mm_add_epi64(c2, _mm_mul_epu32(d4, ao));
		/*c3 = _mm_add_epi64(c3,_mm_mul_epu32(b1,--));*/   /*c3 = _mm_add_epi64(c3,_mm_mul_epu32(b0,--));*/
		c4 = _mm_add_epi64(c4, _mm_mul_epu32(b2, ai));     /*c4 = _mm_add_epi64(c4,_mm_mul_epu32(b1,--));*/

		ai = _mm_shuffle_epi32(A4, 0x44);
		aj = _mm_shuffle_epi32(A0, 0xEE);
		ak = _mm_add_epi64(aj, aj);/* 2a5 */
		al = _mm_add_epi64(ak, ak);/* 4a5 */
		aj = _mm_blend_epi32(aj, ak, 0x3);
		ao = _mm_blend_epi32(ak, al, 0xC);
		ae = _mm_blend_epi32(ak, al, 0x3);

		d1 = _mm_shuffle_epi32(b1, 0x4E);
		d0 = _mm_shuffle_epi32(b0, 0x4E);
		d1 = _mm_mul_epu32(d1, k_19_1);
		d0 = _mm_mul_epu32(d0, k_19_1);
		/*c0 = _mm_add_epi64(c0,_mm_mul_epu32(d1,--));*/	c0 = _mm_add_epi64(c0, _mm_mul_epu32(d0, aj));
		/*c1 = _mm_add_epi64(c1,_mm_mul_epu32(d2,--));*/	c1 = _mm_add_epi64(c1, _mm_mul_epu32(d1, ao));
		/*c2 = _mm_add_epi64(c2,_mm_mul_epu32(d3,--));*/	c2 = _mm_add_epi64(c2, _mm_mul_epu32(d2, ae));
		c3 = _mm_add_epi64(c3, _mm_mul_epu32(d4, ai));		c3 = _mm_add_epi64(c3, _mm_mul_epu32(d3, ao));
		/*c4 = _mm_add_epi64(c4,_mm_mul_epu32(b0,--));*/	c4 = _mm_add_epi64(c4, _mm_mul_epu32(d4, ae));

		ai = _mm_shuffle_epi32(A1, 0xEE);		aj = _mm_add_epi64(ai, ai);
		ak = _mm_shuffle_epi32(A2, 0xEE);		al = _mm_add_epi64(ak, ak);/* 2a7 */
		ak = _mm_blend_epi32(ak, al, 0x3);		ao = _mm_add_epi64(ak, ak);
		ae = _mm_shuffle_epi32(ao, 0x4E);

		b4 = _mm_shuffle_epi32(d4, 0x4E);
		b3 = _mm_shuffle_epi32(d3, 0x4E);
		b4 = _mm_mul_epu32(b4, k_19_1);
		b3 = _mm_mul_epu32(b3, k_19_1);
		c0 = _mm_add_epi64(c0, _mm_mul_epu32(b4, aj)); 		c0 = _mm_add_epi64(c0, _mm_mul_epu32(b3, ao));
		/*c1 = _mm_add_epi64(c1,_mm_mul_epu32(d0,--));*/	c1 = _mm_add_epi64(c1, _mm_mul_epu32(b4, ae));
		c2 = _mm_add_epi64(c2, _mm_mul_epu32(d1, ai));  	/*c2 = _mm_add_epi64(c2,_mm_mul_epu32(d0,--));*/
		c3 = _mm_add_epi64(c3, _mm_mul_epu32(d2, aj));  	/*c3 = _mm_add_epi64(c3,_mm_mul_epu32(d1,--));*/
		c4 = _mm_add_epi64(c4, _mm_mul_epu32(d3, aj));		c4 = _mm_add_epi64(c4, _mm_mul_epu32(d2, ak));

		ai = _mm_shuffle_epi32(A3, 0xEE);		aj = _mm_add_epi64(ai, ai);
		ak = _mm_shuffle_epi32(A4, 0xEE);		al = _mm_add_epi64(ak, ak);
		ae = _mm_blend_epi32(ak, al, 0xC);

		/*c0 = _mm_add_epi64(c0,_mm_mul_epu32(b2,--));*/    /*c0 = _mm_add_epi64(c0,_mm_mul_epu32(b1,ae));*/
		c1 = _mm_add_epi64(c1, _mm_mul_epu32(b3, ai));      /*c1 = _mm_add_epi64(c1,_mm_mul_epu32(b2,ao));*/
		c2 = _mm_add_epi64(c2, _mm_mul_epu32(b4, aj));      /*c2 = _mm_add_epi64(c2,_mm_mul_epu32(b3,ae));*/
		/*c3 = _mm_add_epi64(c3,_mm_mul_epu32(d0,--));*/    c3 = _mm_add_epi64(c3, _mm_mul_epu32(b4, ae));
		/*c4 = _mm_add_epi64(c4,_mm_mul_epu32(d1,--));*/    /*c4 = _mm_add_epi64(c4,_mm_mul_epu32(d0,ae));*/

		/* Coefficient reduction */

		h0_5 = _mm_srlv_epi64(c0, shift0);
		c0 = _mm_and_si128(c0, mask0);
		c1 = _mm_add_epi64(c1, h0_5);

		h1_6 = _mm_srlv_epi64(c1, shift1);
		c1 = _mm_and_si128(c1, mask1);
		c2 = _mm_add_epi64(c2, h1_6);

		h2_7 = _mm_srlv_epi64(c2, shift0);
		c2 = _mm_and_si128(c2, mask0);
		c3 = _mm_add_epi64(c3, h2_7);

		h3_8 = _mm_srlv_epi64(c3, shift1);
		c3 = _mm_and_si128(c3, mask1);
		c4 = _mm_add_epi64(c4, h3_8);

		h4_9 = _mm_srlv_epi64(c4, shift0);
		c4 = _mm_and_si128(c4, mask0);

		mul19_128(h4_9);

		c0 = _mm_add_epi64(c0, h4_9);

		h0_5 = _mm_srlv_epi64(c0, shift0);
		c0 = _mm_and_si128(c0, mask0);
		c1 = _mm_add_epi64(c1, h0_5);

		h1_6 = _mm_srlv_epi64(c1, shift1);
		c1 = _mm_and_si128(c1, mask1);
		c2 = _mm_add_epi64(c2, h1_6);

		b0 = c0;
		b1 = c1;
		b2 = c2;
		b3 = c3;
		b4 = c4;
	}
	_mm_store_si128((__m128i *) A + 0, b0);
	_mm_store_si128((__m128i *) A + 1, b1);
	_mm_store_si128((__m128i *) A + 2, b2);
	_mm_store_si128((__m128i *) A + 3, b3);
	_mm_store_si128((__m128i *) A + 4, b4);
}

/**
 *
 * @param A
 */
static void sqr_Element_1w_h0h5(uint64_t * A)
{
	sqrn_Element_1w_h0h5(A,1);
}

/**
 *
 * @param A
 */
static void compress_Element_1w_h0h5(uint64_t * A)
{
	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m128i mask0 = _mm_set_epi32(0, ones25, 0, ones26);
	const __m128i mask1 = _mm_set_epi32(0, ones26, 0, ones25);
	const __m128i shift0 = _mm_set_epi32(0, 25, 0, 26);
	const __m128i shift1 = _mm_set_epi32(0, 26, 0, 25);
	const __m128i shift_4 = _mm_set_epi32(0,4,0,64);
	const __m128i shift_1 = _mm_set_epi32(0,1,0,64);

	__m128i c0 = _mm_load_si128((__m128i*)A+0);
	__m128i c1 = _mm_load_si128((__m128i*)A+1);
	__m128i c2 = _mm_load_si128((__m128i*)A+2);
	__m128i c3 = _mm_load_si128((__m128i*)A+3);
	__m128i c4 = _mm_load_si128((__m128i*)A+4);

	__m128i h0_5, h1_6, h2_7, h3_8, h4_9;

	h0_5 = _mm_srlv_epi64(c0, shift0);
	c0 = _mm_and_si128(c0, mask0);
	c1 = _mm_add_epi64(c1, h0_5);

	h1_6 = _mm_srlv_epi64(c1, shift1);
	c1 = _mm_and_si128(c1, mask1);
	c2 = _mm_add_epi64(c2, h1_6);

	h2_7 = _mm_srlv_epi64(c2, shift0);
	c2 = _mm_and_si128(c2, mask0);
	c3 = _mm_add_epi64(c3, h2_7);

	h3_8 = _mm_srlv_epi64(c3, shift1);
	c3 = _mm_and_si128(c3, mask1);
	c4 = _mm_add_epi64(c4, h3_8);

	h4_9 = _mm_srlv_epi64(c4, shift0);
	c4 = _mm_and_si128(c4, mask0);

	mul19_128(h4_9);

	c0 = _mm_add_epi64(c0, h4_9);

	h0_5 = _mm_srlv_epi64(c0, shift0);
	c0 = _mm_and_si128(c0, mask0);
	c1 = _mm_add_epi64(c1, h0_5);

	_mm_store_si128((__m128i*)A+0,c0);
	_mm_store_si128((__m128i*)A+1,c1);
	_mm_store_si128((__m128i*)A+2,c2);
	_mm_store_si128((__m128i*)A+3,c3);
	_mm_store_si128((__m128i*)A+4,c4);
}

/**
 *
 * @param A
 */
static void compressfast_Element_1w_h0h5(uint64_t * A)
{
	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m128i mask0   = _mm_set_epi32(0, ones25, 0, ones26);
	const __m128i mask1   = _mm_set_epi32(0, ones26, 0, ones25);
	const __m128i shift0  = _mm_set_epi32(0, 25, 0, 26);
	const __m128i shift1  = _mm_set_epi32(0, 26, 0, 25);
	const __m128i shift_4 = _mm_set_epi32(0,4,0,64);
	const __m128i shift_1 = _mm_set_epi32(0,1,0,64);

	__m128i C[5],L[5],M[5];

	C[0] = _mm_load_si128((__m128i*)A+0);
	C[1] = _mm_load_si128((__m128i*)A+1);
	C[2] = _mm_load_si128((__m128i*)A+2);
	C[3] = _mm_load_si128((__m128i*)A+3);
	C[4] = _mm_load_si128((__m128i*)A+4);

	L[0] = _mm_and_si128(C[0],mask0);
	L[1] = _mm_and_si128(C[1],mask1);
	L[2] = _mm_and_si128(C[2],mask0);
	L[3] = _mm_and_si128(C[3],mask1);
	L[4] = _mm_and_si128(C[4],mask0);

	M[0] = _mm_srlv_epi64(C[0],shift0);
	M[1] = _mm_srlv_epi64(C[1],shift1);
	M[2] = _mm_srlv_epi64(C[2],shift0);
	M[3] = _mm_srlv_epi64(C[3],shift1);
	M[4] = _mm_srlv_epi64(C[4],shift0);

	mul19_128(M[4]);

	C[0] = _mm_add_epi64(L[0],M[4]);
	C[1] = _mm_add_epi64(L[1],M[0]);
	C[2] = _mm_add_epi64(L[2],M[1]);
	C[3] = _mm_add_epi64(L[3],M[2]);
	C[4] = _mm_add_epi64(L[4],M[3]);

	_mm_store_si128((__m128i*)A+0,C[0]);
	_mm_store_si128((__m128i*)A+1,C[1]);
	_mm_store_si128((__m128i*)A+2,C[2]);
	_mm_store_si128((__m128i*)A+3,C[3]);
	_mm_store_si128((__m128i*)A+4,C[4]);
}

#define copy_Element_1w_h0h5(C,A)\
	STORE(C+0,LOAD(A+0));\
	STORE(C+1,LOAD(A+1));\
	STORE(C+2,LOAD(A+2));

/**
 *
 * @param pC
 * @param pA
 */
static void inv_Element_1w_h0h5(uint64_t * pC, uint64_t * pA)
{
	Element_1w_Fp25519 x0, x1, x2;
	uint64_t * T[5];

	T[0] = x0;
	T[1] = pC; /* x^(-1) */
	T[2] = x1;
	T[3] = x2;
	T[4] = pA; /* x */

	copy_Element_1w_h0h5(T[1],pA);
	sqrn_Element_1w_h0h5(T[1],1);
	copy_Element_1w_h0h5(T[2],T[1]);
	sqrn_Element_1w_h0h5(T[2],2);
	mul_Element_1w_h0h5(T[0], pA, T[2]);	compress_Element_1w_h0h5(T[0]);
	mul_Element_1w_h0h5(T[1], T[1], T[0]);	compress_Element_1w_h0h5(T[1]);
	copy_Element_1w_h0h5(T[2],T[1]);
	sqrn_Element_1w_h0h5(T[2],1);
	mul_Element_1w_h0h5(T[0], T[0], T[2]);	compress_Element_1w_h0h5(T[0]);
	copy_Element_1w_h0h5(T[2],T[0]);
	sqrn_Element_1w_h0h5(T[2],5);
	mul_Element_1w_h0h5(T[0], T[0], T[2]);	compress_Element_1w_h0h5(T[0]);
	copy_Element_1w_h0h5(T[2],T[0]);
	sqrn_Element_1w_h0h5(T[2],10);
	mul_Element_1w_h0h5(T[2], T[2], T[0]);	compress_Element_1w_h0h5(T[2]);
	copy_Element_1w_h0h5(T[3],T[2]);
	sqrn_Element_1w_h0h5(T[3],20);
	mul_Element_1w_h0h5(T[3], T[3], T[2]);	compress_Element_1w_h0h5(T[3]);
	sqrn_Element_1w_h0h5(T[3],10);
	mul_Element_1w_h0h5(T[3], T[3], T[0]);	compress_Element_1w_h0h5(T[3]);
	copy_Element_1w_h0h5(T[0],T[3]);
	sqrn_Element_1w_h0h5(T[0],50);
	mul_Element_1w_h0h5(T[0], T[0], T[3]);	compress_Element_1w_h0h5(T[0]);
	copy_Element_1w_h0h5(T[2],T[0]);
	sqrn_Element_1w_h0h5(T[2],100);
	mul_Element_1w_h0h5(T[2], T[2], T[0]);	compress_Element_1w_h0h5(T[2]);
	sqrn_Element_1w_h0h5(T[2],50);
	mul_Element_1w_h0h5(T[2], T[2], T[3]);	compress_Element_1w_h0h5(T[2]);
	sqrn_Element_1w_h0h5(T[2],5);
	mul_Element_1w_h0h5(T[1], T[1], T[2]);	compress_Element_1w_h0h5(T[1]);
}

/**
 *
 * @param A
 * @param B
 * @return
 */
static int compare_Element_1w_h0h5(uint64_t * A, uint64_t * B)
{
	ALIGN uint8_t a[SIZE_FP25519];
	ALIGN uint8_t b[SIZE_FP25519];
	compress_Element_1w_h0h5(A);
	compress_Element_1w_h0h5(B);
	singleH0H5_To_str_bytes(a,A);
	singleH0H5_To_str_bytes(b,B);
	return compare_bytes(a,b,SIZE_FP25519);
}


/**
 * Given u,v, it computes (u/v)^(p+3)/8
 * @param uv
 * @param u
 * @param v
 */
static void invsqrt_Element_1w_h0h5(uint64_t * uv_p38, uint64_t * u, uint64_t * v)
{
	const Element_1w_Fp25519 sqrt_minus_one = {
			0x20ea0b0,0x1fbd7a7,
			0x186c9d2,0x2804c9e,
			0x08f189d,0x1e16569,
			0x035697f,0x004fc1d,
			0x0bd0c60,0x0ae0c92};
	Element_1w_Fp25519 v2,uv,uv3,uv7,x2, x9, sqrt;
	uint64_t * Tab[4];

	mul_Element_1w_h0h5(uv,u,v);		compress_Element_1w_h0h5(uv);/* uv */
	copy_Element_1w_h0h5(v2,v);
	sqr_Element_1w_h0h5(v2);			/* v^2 */
	mul_Element_1w_h0h5(uv3,uv,v2);		compress_Element_1w_h0h5(uv3);/* uv^3 */
	sqr_Element_1w_h0h5(v2);			/* v^4 */
	mul_Element_1w_h0h5(uv7,uv3,v2);	compress_Element_1w_h0h5(uv7);/* uv^7 */

	Tab[0] = x2;
	Tab[1] = x9;
	Tab[2] = sqrt;
	Tab[3] = uv7;

	copy_Element_1w_h0h5(Tab[0],uv7);
	sqr_Element_1w_h0h5(Tab[0]);/* x^2 */
	/* 0 */
	copy_Element_1w_h0h5(Tab[1],Tab[0]);
	sqrn_Element_1w_h0h5(Tab[1],2);
	mul_Element_1w_h0h5(Tab[1], Tab[1], Tab[3]);
	compress_Element_1w_h0h5(Tab[1]);
	/* 1 */
	mul_Element_1w_h0h5(Tab[0], Tab[0], Tab[1]);
	compress_Element_1w_h0h5(Tab[0]);
	/* 2 */
	sqrn_Element_1w_h0h5(Tab[0],1);
	mul_Element_1w_h0h5(Tab[0], Tab[0], Tab[1]);
	compress_Element_1w_h0h5(Tab[0]);
	/* 3 */
	copy_Element_1w_h0h5(Tab[1],Tab[0]);
	sqrn_Element_1w_h0h5(Tab[1],5);
	mul_Element_1w_h0h5(Tab[1], Tab[1], Tab[0]);
	compress_Element_1w_h0h5(Tab[1]);
	/* 4 */
	copy_Element_1w_h0h5(Tab[2],Tab[1]);
	sqrn_Element_1w_h0h5(Tab[2],5);
	mul_Element_1w_h0h5(Tab[2], Tab[2], Tab[0]);
	compress_Element_1w_h0h5(Tab[2]);
	/* 5 */
	copy_Element_1w_h0h5(Tab[1],Tab[2]);
	sqrn_Element_1w_h0h5(Tab[1],15);
	mul_Element_1w_h0h5(Tab[1], Tab[1], Tab[2]);
	compress_Element_1w_h0h5(Tab[1]);
	/* 6 */
	copy_Element_1w_h0h5(Tab[2],Tab[1]);
	sqrn_Element_1w_h0h5(Tab[2],10);
	sqrn_Element_1w_h0h5(Tab[2],20);
	mul_Element_1w_h0h5(Tab[2], Tab[2], Tab[1]);
	compress_Element_1w_h0h5(Tab[2]);
	/* 7 */
	copy_Element_1w_h0h5(Tab[1],Tab[2]);
	sqrn_Element_1w_h0h5(Tab[1],10);
	sqrn_Element_1w_h0h5(Tab[1],50);
	mul_Element_1w_h0h5(Tab[1], Tab[1], Tab[2]);
	compress_Element_1w_h0h5(Tab[1]);
	/* 8 */
	sqrn_Element_1w_h0h5(Tab[1],5);
	mul_Element_1w_h0h5(Tab[1], Tab[1], Tab[0]);
	compress_Element_1w_h0h5(Tab[1]);
	/* 9 */
	copy_Element_1w_h0h5(Tab[2],Tab[1]);
	sqrn_Element_1w_h0h5(Tab[2],100);
	sqrn_Element_1w_h0h5(Tab[2],20);
	sqrn_Element_1w_h0h5(Tab[2],5);
	mul_Element_1w_h0h5(Tab[2], Tab[2], Tab[1]);
	compress_Element_1w_h0h5(Tab[2]);
	/* 10 */
	sqrn_Element_1w_h0h5(Tab[2],2);
	mul_Element_1w_h0h5(Tab[2], Tab[2], Tab[3]);
	compress_Element_1w_h0h5(Tab[2]);

	mul_Element_1w_h0h5(uv_p38,sqrt,uv3);    compress_Element_1w_h0h5(uv_p38);
	/**
	 * Checking whether
	 *      v*B^2 == -u
	 **/
	copy_Element_1w_h0h5(uv3,uv_p38);
	sqr_Element_1w_h0h5(uv3);   	/*   B^2 */
	mul_Element_1w_h0h5(uv,uv3,v);	compress_Element_1w_h0h5(uv); /* v*B^2 */
	neg_Element_1w_h0h5(u);

	if( compare_Element_1w_h0h5(uv,u) == 0)
	{
		mul_Element_1w_h0h5(uv_p38, uv_p38, (argElement_1w)sqrt_minus_one);
		compress_Element_1w_h0h5(uv_p38);/* (√-1)*B */
	}
}

/**
 *
 * @param C
 * @param A
 */
static void sqrt_Element_1w_h0h5(uint64_t*C,uint64_t*A)
{
	invsqrt_Element_1w_h0h5(C,C,A);
}

/** Util functions */

/**
 *
 * @param A
 */
static void random_Element_1w_h0h5(uint64_t *A)
{
	ALIGN uint8_t a[SIZE_FP25519];
	random_bytes(a,SIZE_FP25519);
	a[SIZE_FP25519-1] &= 0x7F;
	str_bytes_To_Element_1w_h0h5(A,a);
}

/**
 *
 * @param A
 */
static void print_Element_1w_h0h5(uint64_t * A)
{
	ALIGN uint8_t a[SIZE_FP25519];
	singleH0H5_To_str_bytes(a,A);
	print_bytes(a,SIZE_FP25519);
}

/**
 *
 * @return
 */
static uint64_t * init_Element_1w_h0h5()
{
	return (uint64_t*) allocate_bytes((4*((NUM_DIGITS_FP25519+3)/4)) * sizeof(uint64_t));
}

/**
 *
 * @return
 */
static uint64_t * prime_Element_1w_h0h5()
{
	uint64_t *prime = init_Element_1w_h0h5();
	prime[0] = 0x3ffffed;		prime[1] = 0x1ffffff;
	prime[2] = 0x1ffffff;		prime[3] = 0x3ffffff;
	prime[4] = 0x3ffffff;		prime[5] = 0x1ffffff;
	prime[6] = 0x1ffffff;		prime[7] = 0x3ffffff;
	prime[8] = 0x3ffffff;		prime[9] = 0x1ffffff;
	return prime;
}


