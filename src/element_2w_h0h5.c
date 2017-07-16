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


#define copy_Element_2w_h0h5(C,A) \
do{                               \
	C[0] = A[0];                  \
	C[1] = A[1];                  \
	C[2] = A[2];                  \
	C[3] = A[3];                  \
	C[4] = A[4];                  \
}while(0)

/**
 *
 * @param C
 * @param LOW
 * @param HIGH
 */
static void interleave_2w_h0h5(argElement_2w C, argElement_1w LOW, argElement_1w HIGH)
{
	__m256i A0 = LOAD(LOW+0);
	__m256i A4 = LOAD(LOW+1);
	__m256i A8 = LOAD(LOW+2);

	__m256i B0 = LOAD(HIGH+0);
	__m256i B4 = LOAD(HIGH+1);
	__m256i B8 = LOAD(HIGH+2);
	C[0] = _mm256_permute2f128_si256(A0,B0,0x20);
	C[1] = _mm256_permute2f128_si256(A0,B0,0x31);
	C[2] = _mm256_permute2f128_si256(A4,B4,0x20);
	C[3] = _mm256_permute2f128_si256(A4,B4,0x31);
	C[4] = _mm256_permute2f128_si256(A8,B8,0x20);
}

/**
 *
 * @param LOW
 * @param HIGH
 * @param A
 */
void deinterleave_2w_h0h5(argElement_1w LOW, argElement_1w HIGH, argElement_2w A)
{
	STORE(LOW  + 0, _mm256_permute2f128_si256(A[0], A[1], 0x20));
	STORE(HIGH + 0, _mm256_permute2f128_si256(A[0], A[1], 0x31));
	STORE(LOW  + 1, _mm256_permute2f128_si256(A[2], A[3], 0x20));
	STORE(HIGH + 1, _mm256_permute2f128_si256(A[2], A[3], 0x31));
	STORE(LOW  + 2, A[4]);
	STORE(HIGH + 2, _mm256_castsi128_si256(_mm256_extractf128_si256(A[4], 1)));
}

/**
 *
 * @param A
 * @param low
 * @param high
 */
static void Element_1w_x64_to_Element_2w_h0h5(argElement_2w A, uint64_t * __restrict low, uint64_t * __restrict high)
{
	Element_1w_Fp25519 LOW,HIGH;
	str_bytes_To_Element_1w_h0h5(LOW,(uint8_t*)low);
	str_bytes_To_Element_1w_h0h5(HIGH,(uint8_t*)high);
	interleave_2w_h0h5(A,LOW,HIGH);
}

/**
 *
 * @param C
 * @param A
 * @param B
 */
static void add_Element_2w_h0h5(argElement_2w C,argElement_2w A,argElement_2w B)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
		C[i] = ADD(A[i],B[i]);
}

static const ALIGN uint64_t CONST_2P_2P_H0H5[20] = {
		0x7ffffda,0x3fffffe,0x7ffffda,0x3fffffe,
		0x3fffffe,0x7fffffe,0x3fffffe,0x7fffffe,
		0x7fffffe,0x3fffffe,0x7fffffe,0x3fffffe,
		0x3fffffe,0x7fffffe,0x3fffffe,0x7fffffe,
		0x7fffffe,0x3fffffe,0x7fffffe,0x3fffffe};

static const ALIGN uint64_t CONST_2P_00_H0H5[20] = {
		0x7ffffda,0x3fffffe,0x0000000,0x0000000,
		0x3fffffe,0x7fffffe,0x0000000,0x0000000,
		0x7fffffe,0x3fffffe,0x0000000,0x0000000,
		0x3fffffe,0x7fffffe,0x0000000,0x0000000,
		0x7fffffe,0x3fffffe,0x0000000,0x0000000};
/**
 *
 * @param C
 * @param A
 * @param B
 */
static void sub_Element_2w_h0h5(argElement_2w __restrict C, argElement_2w __restrict A, argElement_2w __restrict B)
{
	argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H5;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
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
static void subadd_Element_2w_h0h5(
		argElement_2w __restrict C,
		argElement_2w __restrict A,
		const int permutation)
{
	const __m256i mask_subadd = _mm256_set_epi64x(0,0,-1, -1);
	argElement_2w _2P_00 = (argElement_2w)CONST_2P_00_H0H5;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
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
 * Given
 * 		A = [X |Y ]
 * Returns
 * 		C = [2P-X | Y]
 * @param C
 * @param A
 */
static void negZZ_Element_2w_h0h5(argElement_2w C,argElement_2w A)
{
	const __m256i mask_subadd = _mm256_set_epi64x(0,0,-1, -1);
	argElement_2w _2P_00 = (argElement_2w)CONST_2P_00_H0H5;
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		C[i] = ADD(_2P_00[i],SUB(XOR(A[i],mask_subadd),mask_subadd));
	}
}
/**
 *
 * @param A
 * @param B
 */
static __inline void addsub_Element_2w_h0h5(argElement_2w __restrict A, argElement_2w __restrict B)
{
	argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H5;

	int i=0;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i D,C;
		D = ADD(A[i], B[i]);
		C = ADD(A[i],SUB(_2P[i],B[i]));
		A[i] = D;
		B[i] = C;
	}
}

/**
 * Given:
 *      A = [A0|A1]
 *      B = [B0|B1]
 * Computes:
 * 		C0 <-- [A0*B0 | A1*B1]
 * 		C1 <-- [A1*B0 | A0*B1]
 * This function precomputes the shared operand
 * to be used in both multiplications.
 *
 * @param C0
 * @param C1
 * @param A
 * @param B
 */
static void mul2_Element_2w_h0h5(
		__m256i * C0, __m256i * C1,
		__m256i * A,  __m256i *  B)
{
	const __m256i times_19 = SET1_64(19);
	const __m256i sh_0 = _mm256_set_epi64x(0,1,0,1);
	const __m256i sh_1 = _mm256_set_epi64x(1,0,1,0);

	__m256i ai,aj,ak,al;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;                 __m256i p0,p1,p2,p3,p4;
	__m256i k0,k1,k2,k3,k4;                 __m256i m0,m1,m2,m3,m4;

	b0 = B[0];
	b1 = B[1];
	b2 = B[2];
	b3 = B[3];
	b4 = B[4];

    e0 = MUL(b0,times_19); d0 = ALIGNR(b0,e0);
    e1 = MUL(b1,times_19); d1 = ALIGNR(b1,e1);
    e2 = MUL(b2,times_19); d2 = ALIGNR(b2,e2);
    e3 = MUL(b3,times_19); d3 = ALIGNR(b3,e3);
    e4 = MUL(b4,times_19); d4 = ALIGNR(b4,e4);

	ai = SHUF(A[0],0x44);                   ak = PERM64(A[0],0x0A);
	c0 = MUL(b0,ai);       		            p0 = MUL(b0,ak);
	c1 = MUL(b1,ai);       		            p1 = MUL(b1,ak);
	c2 = MUL(b2,ai);       		            p2 = MUL(b2,ak);
	c3 = MUL(b3,ai);       		            p3 = MUL(b3,ak);
	c4 = MUL(b4,ai);       		            p4 = MUL(b4,ak);
                                            
	ai = SHUF(A[2],0x44);                   ak = PERM64(A[2],0x0A);
	c0 = ADD(c0,MUL(d3,ai));	            p0 = ADD(p0,MUL(d3,ak));
	c1 = ADD(c1,MUL(d4,ai));	            p1 = ADD(p1,MUL(d4,ak));
	c2 = ADD(c2,MUL(b0,ai));	            p2 = ADD(p2,MUL(b0,ak));
	c3 = ADD(c3,MUL(b1,ai));	            p3 = ADD(p3,MUL(b1,ak));
	c4 = ADD(c4,MUL(b2,ai));	            p4 = ADD(p4,MUL(b2,ak));
                                            
	ai = SHUF(A[4],0x44);                   ak = PERM64(A[4],0x0A);
	c0 = ADD(c0,MUL(d1,ai));	            p0 = ADD(p0,MUL(d1,ak));
	c1 = ADD(c1,MUL(d2,ai));	            p1 = ADD(p1,MUL(d2,ak));
	c2 = ADD(c2,MUL(d3,ai));	            p2 = ADD(p2,MUL(d3,ak));
	c3 = ADD(c3,MUL(d4,ai));	            p3 = ADD(p3,MUL(d4,ak));
	c4 = ADD(c4,MUL(b0,ai));	            p4 = ADD(p4,MUL(b0,ak));
                                            
	ai = SHUF(A[1],0xEE);                   ak = PERM64(A[1],0x5F);
	c0 = ADD(c0,MUL(e4,ai));	            p0 = ADD(p0,MUL(e4,ak));
	c1 = ADD(c1,MUL(d0,ai));	            p1 = ADD(p1,MUL(d0,ak));
	c2 = ADD(c2,MUL(d1,ai));	            p2 = ADD(p2,MUL(d1,ak));
	c3 = ADD(c3,MUL(d2,ai));	            p3 = ADD(p3,MUL(d2,ak));
	c4 = ADD(c4,MUL(d3,ai));	            p4 = ADD(p4,MUL(d3,ak));
                                            
	ai = SHUF(A[3],0xEE);                   ak = PERM64(A[3],0x5F);
	c0 = ADD(c0,MUL(e2,ai));	            p0 = ADD(p0,MUL(e2,ak));
	c1 = ADD(c1,MUL(e3,ai));	            p1 = ADD(p1,MUL(e3,ak));
	c2 = ADD(c2,MUL(e4,ai));	            p2 = ADD(p2,MUL(e4,ak));
	c3 = ADD(c3,MUL(d0,ai));	            p3 = ADD(p3,MUL(d0,ak));
	c4 = ADD(c4,MUL(d1,ai));	            p4 = ADD(p4,MUL(d1,ak));

	C0[0] = c0;        		                C1[0] = p0;
	C0[1] = c1;        		                C1[1] = p1;
	C0[2] = c2;        		                C1[2] = p2;
	C0[3] = c3;        		                C1[3] = p3;
	C0[4] = c4;        		                C1[4] = p4;

	aj = SHUF(A[1],0x44);                   al = PERM64(A[1],0x0A);
	k0 = MUL(d4,aj);        				m0 = MUL(d4,al);
	k1 = MUL(b0,aj);        				m1 = MUL(b0,al);
	k2 = MUL(b1,aj);        				m2 = MUL(b1,al);
	k3 = MUL(b2,aj);        				m3 = MUL(b2,al);
	k4 = MUL(b3,aj);        				m4 = MUL(b3,al);

	aj = SHUF(A[3],0x44);                   al = PERM64(A[3],0x0A);
	k0 = ADD(k0,MUL(d2,aj));				m0 = ADD(m0,MUL(d2,al));
	k1 = ADD(k1,MUL(d3,aj));				m1 = ADD(m1,MUL(d3,al));
	k2 = ADD(k2,MUL(d4,aj));				m2 = ADD(m2,MUL(d4,al));
	k3 = ADD(k3,MUL(b0,aj));				m3 = ADD(m3,MUL(b0,al));
	k4 = ADD(k4,MUL(b1,aj));				m4 = ADD(m4,MUL(b1,al));

    aj = SHUF(A[0],0xEE);                   al = PERM64(A[0],0x5F);
    k0 = ADD(k0,MUL(d0,aj));		        m0 = ADD(m0,MUL(d0,al));
    k1 = ADD(k1,MUL(d1,aj));		        m1 = ADD(m1,MUL(d1,al));
    k2 = ADD(k2,MUL(d2,aj));		        m2 = ADD(m2,MUL(d2,al));
    k3 = ADD(k3,MUL(d3,aj));		        m3 = ADD(m3,MUL(d3,al));
    k4 = ADD(k4,MUL(d4,aj));		        m4 = ADD(m4,MUL(d4,al));

    aj = SHUF(A[2],0xEE);                   al = PERM64(A[2],0x5F);
    k0 = ADD(k0,MUL(e3,aj));		        m0 = ADD(m0,MUL(e3,al));
    k1 = ADD(k1,MUL(e4,aj));		        m1 = ADD(m1,MUL(e4,al));
    k2 = ADD(k2,MUL(d0,aj));		        m2 = ADD(m2,MUL(d0,al));
    k3 = ADD(k3,MUL(d1,aj));		        m3 = ADD(m3,MUL(d1,al));
    k4 = ADD(k4,MUL(d2,aj));		        m4 = ADD(m4,MUL(d2,al));

    aj = SHUF(A[4],0xEE);                   al = PERM64(A[4],0x5F);
    k0 = ADD(k0,MUL(e1,aj));		        m0 = ADD(m0,MUL(e1,al));
    k1 = ADD(k1,MUL(e2,aj));		        m1 = ADD(m1,MUL(e2,al));
    k2 = ADD(k2,MUL(e3,aj));		        m2 = ADD(m2,MUL(e3,al));
    k3 = ADD(k3,MUL(e4,aj));		        m3 = ADD(m3,MUL(e4,al));
    k4 = ADD(k4,MUL(d0,aj));		        m4 = ADD(m4,MUL(d0,al));

	C0[0] = ADD(C0[0],SHLV(k0,sh_0));       C1[0] = ADD(C1[0],SHLV(m0,sh_0));
	C0[1] = ADD(C0[1],SHLV(k1,sh_1));       C1[1] = ADD(C1[1],SHLV(m1,sh_1));
	C0[2] = ADD(C0[2],SHLV(k2,sh_0));       C1[2] = ADD(C1[2],SHLV(m2,sh_0));
	C0[3] = ADD(C0[3],SHLV(k3,sh_1));       C1[3] = ADD(C1[3],SHLV(m3,sh_1));
	C0[4] = ADD(C0[4],SHLV(k4,sh_0));       C1[4] = ADD(C1[4],SHLV(m4,sh_0));
}

/**
 *
 * B must be compressed before to call mul_h0h5
 * in order to avoid overflow on multiplications by 19 or 38..
 *
 * @param C
 * @param A
 * @param B
 */
static void mul_Element_2w_h0h5(__m256i *  C, __m256i * A, __m256i *  B)
{
	const __m256i times_19 = SET1_64(19);
	const __m256i sh_0 = _mm256_set_epi64x(0,1,0,1);
	__m256i sh_1;

	__m256i ai,aj;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;
	__m256i k0,k1,k2,k3,k4;
	__m256i t0,t1,t2,t3,t4;

	b0 = B[0];
	b1 = B[1];
	b2 = B[2];
	b3 = B[3];
	b4 = B[4];

	e4 = MUL(b4,times_19); d4 = ALIGNR(b4,e4);
	e3 = MUL(b3,times_19); d3 = ALIGNR(b3,e3);
	e2 = MUL(b2,times_19); d2 = ALIGNR(b2,e2);
	e1 = MUL(b1,times_19); d1 = ALIGNR(b1,e1);
	e0 = MUL(b0,times_19); d0 = ALIGNR(b0,e0);

	ai = SHUF(A[0],0x44);  			aj = SHUF(A[1],0x44);
	c0 = MUL(b0,ai);       			k0 = MUL(d4,aj);
	c1 = MUL(b1,ai);       			k1 = MUL(b0,aj);
	c2 = MUL(b2,ai);       			k2 = MUL(b1,aj);
	c3 = MUL(b3,ai);       			k3 = MUL(b2,aj);
	c4 = MUL(b4,ai);       			k4 = MUL(b3,aj);

	ai = SHUF(A[2], 0x44);    		aj = SHUF(A[3], 0x44);
	c0 = ADD(c0,MUL(d3,ai));		k0 = ADD(k0,MUL(d2,aj));
	c1 = ADD(c1,MUL(d4,ai));		k1 = ADD(k1,MUL(d3,aj));
	c2 = ADD(c2,MUL(b0,ai));		k2 = ADD(k2,MUL(d4,aj));
	c3 = ADD(c3,MUL(b1,ai));		k3 = ADD(k3,MUL(b0,aj));
	c4 = ADD(c4,MUL(b2,ai));		k4 = ADD(k4,MUL(b1,aj));

	ai = SHUF(A[4],0x44);           aj = SHUF(A[0],0xEE);
	c0 = ADD(c0,MUL(d1,ai));        k0 = ADD(k0,MUL(d0,aj));
	c1 = ADD(c1,MUL(d2,ai));        k1 = ADD(k1,MUL(d1,aj));
	c2 = ADD(c2,MUL(d3,ai));        k2 = ADD(k2,MUL(d2,aj));
	c3 = ADD(c3,MUL(d4,ai));        k3 = ADD(k3,MUL(d3,aj));
	c4 = ADD(c4,MUL(b0,ai));        k4 = ADD(k4,MUL(d4,aj));

	ai = SHUF(A[1],0xEE);           aj = SHUF(A[2],0xEE);
	c0 = ADD(c0,MUL(e4,ai));        k0 = ADD(k0,MUL(e3,aj));
	c1 = ADD(c1,MUL(d0,ai));        k1 = ADD(k1,MUL(e4,aj));
	c2 = ADD(c2,MUL(d1,ai));        k2 = ADD(k2,MUL(d0,aj));
	c3 = ADD(c3,MUL(d2,ai));        k3 = ADD(k3,MUL(d1,aj));
	c4 = ADD(c4,MUL(d3,ai));        k4 = ADD(k4,MUL(d2,aj));

	ai = SHUF(A[3],0xEE);           aj = SHUF(A[4],0xEE);
	c0 = ADD(c0,MUL(e2,ai));        k0 = ADD(k0,MUL(e1,aj));
	c1 = ADD(c1,MUL(e3,ai));        k1 = ADD(k1,MUL(e2,aj));
	c2 = ADD(c2,MUL(e4,ai));        k2 = ADD(k2,MUL(e3,aj));
	c3 = ADD(c3,MUL(d0,ai));        k3 = ADD(k3,MUL(e4,aj));
	c4 = ADD(c4,MUL(d1,ai));        k4 = ADD(k4,MUL(d0,aj));

	sh_1 = SHUF(sh_0,0x4E);
	t0 = SHLV(k0,sh_0);
	t1 = SHLV(k1,sh_1);
	t2 = SHLV(k2,sh_0);
	t3 = SHLV(k3,sh_1);
	t4 = SHLV(k4,sh_0);

	C[0] = ADD(c0,t0);
	C[1] = ADD(c1,t1);
	C[2] = ADD(c2,t2);
	C[3] = ADD(c3,t3);
	C[4] = ADD(c4,t4);
}

/**
 *
 * @param C
 */
static void sqr_Element_2w_h0h5(__m256i *  C)
{
	const __m256i times_19 = SET1_64(19);
	const __m256i sh_0 = _mm256_set_epi64x(0,1,0,1);
	__m256i sh_1;

	__m256i ai,aj,a2i,a2j;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;
	__m256i k0,k1,k2,k3,k4;
	__m256i t0,t1,t2,t3,t4;

	b0 = C[0];
	b1 = C[1];
	b2 = C[2];
	b3 = C[3];
	b4 = C[4];

	e4 = MUL(b4,times_19); d4 = ALIGNR(b4,e4);
	e3 = MUL(b3,times_19); d3 = ALIGNR(b3,e3);
	e2 = MUL(b2,times_19); d2 = ALIGNR(b2,e2);
	e1 = MUL(b1,times_19); d1 = ALIGNR(b1,e1);
	e0 = MUL(b0,times_19); d0 = ALIGNR(b0,e0);

	ai = SHUF(C[0],0x44);  			aj = SHUF(C[1],0x44);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = MUL(b0,ai);       			k0 = MUL(d4,a2j);
	c1 = MUL(b1,a2i);      			/*k1 = MUL(b0,aj);*/
	c2 = MUL(b2,a2i);      			k2 = MUL(b1,aj);
	c3 = MUL(b3,a2i);      			k3 = MUL(b2,a2j);
	c4 = MUL(b4,a2i);      			k4 = MUL(b3,a2j);

	ai = SHUF(C[2], 0x44);    		aj = SHUF(C[3], 0x44);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = ADD(c0,MUL(d3,a2i));		/*k0 = ADD(k0, MUL(d2,aj));*/
	c1 = ADD(c1,MUL(d4,a2i));		k1 = MUL(d3,aj);
	/*c2 = ADD(c2,MUL(b0,ai));*/	k2 = ADD(k2,MUL(d4,a2j));
	/*c3 = ADD(c3,MUL(b1,ai));*/	/*k3 = ADD(k3,MUL(b0,aj));*/
	c4 = ADD(c4,MUL(b2,ai));		/*k4 = ADD(k4,MUL(b1,aj));*/

	ai = SHUF(C[4],0x44);           aj = SHUF(C[0],0xEE);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	/*c0 = ADD(c0,MUL(d1,ai));*/    k0 = ADD(k0,MUL(d0,aj));
	/*c1 = ADD(c1,MUL(d2,ai));*/    k1 = ADD(k1,MUL(d1,a2j));
	/*c2 = ADD(c2,MUL(d3,ai));*/    k2 = ADD(k2,MUL(d2,a2j));
	c3 = ADD(c3,MUL(d4,ai));        k3 = ADD(k3,MUL(d3,a2j));
	/*c4 = ADD(c4,MUL(b0,ai));*/    k4 = ADD(k4,MUL(d4,a2j));

	ai = SHUF(C[1],0xEE);           aj = SHUF(C[2],0xEE);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = ADD(c0,MUL(e4,a2i));       k0 = ADD(k0,MUL(e3,a2j));
	/*c1 = ADD(c1,MUL(d0,ai));*/    k1 = ADD(k1,MUL(e4,a2j));
	c2 = ADD(c2,MUL(d1,ai));        /*k2 = ADD(k2,MUL(d0,aj));*/
	c3 = ADD(c3,MUL(d2,a2i));       /*k3 = ADD(k3,MUL(d1,aj));*/
	c4 = ADD(c4,MUL(d3,a2i));       k4 = ADD(k4,MUL(d2,aj));

	ai = SHUF(C[3],0xEE);           aj = SHUF(C[4],0xEE);
	a2i = ADD(ai,ai);
	/*c0 = ADD(c0,MUL(e2,ai));*/    /*k0 = ADD(k0,MUL(e1,aj));*/
	c1 = ADD(c1,MUL(e3,ai));        /*k1 = ADD(k1,MUL(e2,aj));*/
	c2 = ADD(c2,MUL(e4,a2i));       /*k2 = ADD(k2,MUL(e3,aj));*/
	/*c3 = ADD(c3,MUL(d0,ai));*/    k3 = ADD(k3,MUL(e4,aj));
	/*c4 = ADD(c4,MUL(d1,ai));*/    /*k4 = ADD(k4,MUL(d0,aj));*/

	sh_1 = SHUF(sh_0,0x4E);
	t0 = SHLV(k0,sh_0);
	t1 = SHLV(k1,sh_1);
	t2 = SHLV(k2,sh_0);
	t3 = SHLV(k3,sh_1);
	t4 = SHLV(k4,sh_0);

	C[0] = ADD(c0,t0);
	C[1] = ADD(c1,t1);
	C[2] = ADD(c2,t2);
	C[3] = ADD(c3,t3);
	C[4] = ADD(c4,t4);
}

#define mul19(A)\
	A = _mm256_shuffle_epi32(_mm256_add_epi64(_mm256_add_epi64(_mm256_sllv_epi64(A, shift_4),_mm256_sllv_epi64(A, shift_1)),A),0x4E);
/**
 *
 * @param C
 */
static void compress_Element_2w_h0h5(__m256i *  C)
{
	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m256i mask0 = _mm256_set_epi32(0, ones25, 0, ones26,0, ones25, 0, ones26);
	const __m256i mask1 = _mm256_set_epi32(0, ones26, 0, ones25,0, ones26, 0, ones25);
	const __m256i shift0 = _mm256_set_epi32(0, 25, 0, 26,0, 25, 0, 26);
	const __m256i shift1 = _mm256_set_epi32(0, 26, 0, 25,0, 26, 0, 25);
	const __m256i shift_4 = _mm256_set_epi32(0,4,0,64,0,4,0,64);
	const __m256i shift_1 = _mm256_set_epi32(0,1,0,64,0,1,0,64);

	__m256i c0 = C[0];
	__m256i c1 = C[1];
	__m256i c2 = C[2];
	__m256i c3 = C[3];
	__m256i c4 = C[4];

	__m256i h0_5, h1_6, h2_7, h3_8, h4_9;

	h0_5 = _mm256_srlv_epi64(c0, shift0);
	c0 = _mm256_and_si256(c0, mask0);
	c1 = _mm256_add_epi64(c1, h0_5);

	h1_6 = _mm256_srlv_epi64(c1, shift1);
	c1 = _mm256_and_si256(c1, mask1);
	c2 = _mm256_add_epi64(c2, h1_6);

	h2_7 = _mm256_srlv_epi64(c2, shift0);
	c2 = _mm256_and_si256(c2, mask0);
	c3 = _mm256_add_epi64(c3, h2_7);

	h3_8 = _mm256_srlv_epi64(c3, shift1);
	c3 = _mm256_and_si256(c3, mask1);
	c4 = _mm256_add_epi64(c4, h3_8);

	h4_9 = _mm256_srlv_epi64(c4, shift0);
	c4 = _mm256_and_si256(c4, mask0);

	mul19(h4_9);

	c0 = _mm256_add_epi64(c0, h4_9);

	h0_5 = _mm256_srlv_epi64(c0, shift0);
	c0 = _mm256_and_si256(c0, mask0);
	c1 = _mm256_add_epi64(c1, h0_5);

	C[0] = c0;
	C[1] = c1;
	C[2] = c2;
	C[3] = c3;
	C[4] = c4;
}

/**
 *
 * @param C
 * @param D
 */
static void compress2_Element_2w_h0h5(__m256i *__restrict  C, __m256i *__restrict  D)
{
	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m256i mask0 = _mm256_set_epi32(0, ones25, 0, ones26,0, ones25, 0, ones26);
	const __m256i mask1 = _mm256_set_epi32(0, ones26, 0, ones25,0, ones26, 0, ones25);
	const __m256i shift0 = _mm256_set_epi32(0, 25, 0, 26,0, 25, 0, 26);
	const __m256i shift1 = _mm256_set_epi32(0, 26, 0, 25,0, 26, 0, 25);
	const __m256i shift_4 = _mm256_set_epi32(0,4,0,64,0,4,0,64);
	const __m256i shift_1 = _mm256_set_epi32(0,1,0,64,0,1,0,64);

	__m256i c0 = C[0];                 __m256i d0 = D[0];
	__m256i c1 = C[1];                 __m256i d1 = D[1];
	__m256i c2 = C[2];                 __m256i d2 = D[2];
	__m256i c3 = C[3];                 __m256i d3 = D[3];
	__m256i c4 = C[4];                 __m256i d4 = D[4];

	__m256i h0_5, h1_6, h2_7, h3_8, h4_9;           __m256i H0_5, H1_6, H2_7, H3_8, H4_9;

	h0_5 = SHRV(c0, shift0);           H0_5 = SHRV(d0, shift0);
	c0 = AND(c0, mask0);               d0 = AND(d0, mask0);
	c1 = ADD(c1, h0_5);                d1 = ADD(d1, H0_5);

	h1_6 = SHRV(c1, shift1);           H1_6 = SHRV(d1, shift1);
	c1 = AND(c1, mask1);               d1 = AND(d1, mask1);
	c2 = ADD(c2, h1_6);                d2 = ADD(d2, H1_6);

	h2_7 = SHRV(c2, shift0);           H2_7 = SHRV(d2, shift0);
	c2 = AND(c2, mask0);               d2 = AND(d2, mask0);
	c3 = ADD(c3, h2_7);                d3 = ADD(d3, H2_7);

	h3_8 = SHRV(c3, shift1);           H3_8 = SHRV(d3, shift1);
	c3 = AND(c3, mask1);               d3 = AND(d3, mask1);
	c4 = ADD(c4, h3_8);                d4 = ADD(d4, H3_8);

	h4_9 = SHRV(c4, shift0);           H4_9 = SHRV(d4, shift0);
	c4 = AND(c4, mask0);               d4 = AND(d4, mask0);

	mul19(h4_9);                       mul19(H4_9);

	c0 = ADD(c0, h4_9);                d0 = ADD(d0, H4_9);

	h0_5 = SHRV(c0, shift0);           H0_5 = SHRV(d0, shift0);
	c0 = AND(c0, mask0);               d0 = AND(d0, mask0);
	c1 = ADD(c1, h0_5);                d1 = ADD(d1, H0_5);

	C[0] = c0;                         D[0] = d0;
	C[1] = c1;                         D[1] = d1;
	C[2] = c2;                         D[2] = d2;
	C[3] = c3;                         D[3] = d3;
	C[4] = c4;                         D[4] = d4;
}

/**
 *
 * @param C
 */
static void compressfast_Element_2w_h0h5(__m256i *  C)
{
	const uint64_t ones25 = ((uint64_t) 1 << 25) - 1;
	const uint64_t ones26 = ((uint64_t) 1 << 26) - 1;
	const __m256i mask0 = _mm256_set_epi32(0, ones25, 0, ones26,0, ones25, 0, ones26);
	const __m256i mask1 = _mm256_set_epi32(0, ones26, 0, ones25,0, ones26, 0, ones25);
	const __m256i shift0 = _mm256_set_epi32(0, 25, 0, 26,0, 25, 0, 26);
	const __m256i shift1 = _mm256_set_epi32(0, 26, 0, 25,0, 26, 0, 25);
	const __m256i shift_4 = _mm256_set_epi32(0,4,0,64,0,4,0,64);
	const __m256i shift_1 = _mm256_set_epi32(0,1,0,64,0,1,0,64);

	__m256i L[5],M[5];

	L[0] = AND(C[0],mask0);
	L[1] = AND(C[1],mask1);
	L[2] = AND(C[2],mask0);
	L[3] = AND(C[3],mask1);
	L[4] = AND(C[4],mask0);

	M[0] = SHRV(C[0],shift0);
	M[1] = SHRV(C[1],shift1);
	M[2] = SHRV(C[2],shift0);
	M[3] = SHRV(C[3],shift1);
	M[4] = SHRV(C[4],shift0);

	mul19(M[4]);

	C[0] = ADD(L[0],M[4]);
	C[1] = ADD(L[1],M[0]);
	C[2] = ADD(L[2],M[1]);
	C[3] = ADD(L[3],M[2]);
	C[4] = ADD(L[4],M[3]);
}

/** Util functions */

/**
 *
 * @param X_Y
 */
static void random_Element_2w_h0h5(argElement_2w X_Y)
{
	Element_1w_Fp25519 X,Y;
	random_Element_1w_h0h5(X);
	random_Element_1w_h0h5(Y);
	interleave_2w_h0h5(X_Y,X,Y);
}

/**
 *
 * @param X_Y
 */
static void print_Element_2w_h0h5(argElement_2w X_Y)
{
	Element_1w_Fp25519 X,Y;
	deinterleave_2w_h0h5(X,Y,X_Y);
	print_Element_1w_h0h5(X);
	print_Element_1w_h0h5(Y);
}

/**
 *
 * @param X0_X1
 * @param Y0_Y1
 * @return
 */
static int compare_Element_2w_h0h5(argElement_2w X0_X1, argElement_2w Y0_Y1)
{
	Element_1w_Fp25519 X0,X1;
	Element_1w_Fp25519 Y0,Y1;
	deinterleave_2w_h0h5(X0,X1,X0_X1);
	deinterleave_2w_h0h5(Y0,Y1,Y0_Y1);
	return compare_Element_1w_h0h5(X0,Y0) && compare_Element_1w_h0h5(X1,Y1);
}

/**
 *
 * @return
 */
static __m256i * init_Element_2w_h0h5()
{
	return (__m256i*) allocate_bytes((NUM_DIGITS_FP25519/2) * sizeof(__m256i));
}
