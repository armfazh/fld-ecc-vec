//#include "faz_fp_avx2.h"
#include "ecc_ed448.h"

/**
 * Swap method
 * ===========
 *
 * The flag SWAP_METHOD defines the method used for
 * performing constant time conditional move.
 *  1) PERMUTATION. Uses AVX2 permutation instructions
 *  2) CMOV. Uses the 64-bit CMOV instruction.
 */
#define PERMUTATION 0x1
#define LOGIC       0x2
#define CMOV        0x4
#define SWAP_METHOD PERMUTATION

#if SWAP_METHOD == CMOV
static inline void cmov(int bit, uint64_t * const pX,uint64_t * const pY)
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

static uint8_t * initX448_Key()
{
	return (uint8_t*) allocate_bytes(ECDH448_KEY_SIZE_BYTES * sizeof(uint8_t));
}

/**
 *
 * @param C
 */
static inline void  perm4E_x448(__m256i * C)
{
	int i=0;
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
		C[i] = PERM64(C[i],0x4E);
}

/**
 *
 * @param key
 * @param X2X3
 * @param Z2Z3
 */
static inline void step_ladder(
		uint64_t *__restrict key,
		argElement_2w X2X3,
		argElement_2w Z2Z3)
{
	int i = 0, j = 0, s = 0;
	uint64_t prev = 0;
	const uint64_t param_a24 = 39081;
	const __m256i a24 = _mm256_set1_epi64x(param_a24);
	ALIGN __m256i buffer[(NUM_DIGITS_FP448/2)*4];
	argElement_2w X1 = buffer+(NUM_DIGITS_FP448/2)*0;
	argElement_2w t0 = buffer+(NUM_DIGITS_FP448/2)*1;
	argElement_2w t1 = buffer+(NUM_DIGITS_FP448/2)*2;
	argElement_2w t2 = buffer+(NUM_DIGITS_FP448/2)*3;
	argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H8;

	for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
	{
		X1[s] = X2X3[s];
	}

	j = 63;
	for (i = 6; i >= 0; i--)
	{
		while (j >= 0)
		{
			uint64_t bit = (key[i] >> j) & 0x1;
			uint64_t swap = bit ^ prev;
			prev = bit;

			/* X2X3: [A|C] = [X2|X3] + [Z2|Z3]  */
			/* Z2Z3: [B|D] = [X2|X3] - [Z2|Z3]  */
			addsub_Element_2w_h0h8(X2X3, Z2Z3);
			/* Z2Z3: [D|B] = Permute([B|D])     */
			 perm4E_x448(Z2Z3);

			/* t0:   [DA|CB] = [A|C] * [D|B]    */
			mul_Element_2w_h0h8(t0, X2X3, Z2Z3);
			compress_Element_2w_h0h8(t0);

			/* t1:   [CB|DA] = Perm([DA|CB])    */
			for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
			{
				t1[s] = PERM64(t0[s], 0x4E);
			}

			/* Constant time swap               */
#if SWAP_METHOD == PERMUTATION
			/* Using vector permutation instructions  */
			__m256i perm = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
			__m256i mask = _mm256_set1_epi32((uint32_t) swap << 2);
			__m256i maskbit0 = XOR(perm, mask);
			__m256i maskbit1 = PERM64(maskbit0, 0x4E);
			for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
			{
				X2X3[s] = _mm256_permutevar8x32_epi32(X2X3[s],maskbit0);
				Z2Z3[s] = _mm256_permutevar8x32_epi32(Z2Z3[s],maskbit1);
			}
#elif SWAP_METHOD == LOGIC
			/* Using logic arithmetic instructions    */
            const __m256i mask = _mm256_set1_epi64x(-swap);
            for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
            {
                __m256i U = PERM128(X2X3[s],Z2Z3[s],0x21);
                __m256i V = BLEND32(X2X3[s],Z2Z3[s],0xF0);
                X2X3[s] = XOR(ANDNOT(mask,V),AND(mask,U));
                Z2Z3[s] = PERM64(X2X3[s],0x4E);
            }
#elif SWAP_METHOD == CMOV
            /* Using 64-bit CMOV instruction          */
            uint64_t *pX = (uint64_t *) X2X3;
            cmov(swap,pX,pX + 2);
            pX = (uint64_t *) Z2Z3;
            cmov(1-swap,pX,pX + 2);
#else
#error "Define SWAP_METHOD with PERMUTATION or CMOV or LOGIC."
#endif
			/* t1:  [_|t1] = [CB|DA] + [DA|CB]  */
			/* t0:  [_|t0] = [CB|DA] - [DA|CB]  */
			addsub_Element_2w_h0h8(t1,t0);

			/* t0:  [  A | t0 ] = BLEND( [ A | C ] , [ __ | t0 ] ) */
			/* t1:  [  B | t1 ] = BLEND( [ B | D ] , [ __ | t1 ] ) */
			for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
			{
				t0[s] = BLEND32(X2X3[s], t0[s], 0xF0);
				t1[s] = BLEND32(Z2Z3[s], t1[s], 0xF0);
			}

			compressfast_Element_2w_h0h8(t0);
			compressfast_Element_2w_h0h8(t1);

			/* t0:  [ AA | t0 ] = [  A | t0 ]^2 */
			sqr_Element_2w_h0h8(t0);
			/* t1:  [ BB | t1 ] = [  B | t1 ]^2 */
			sqr_Element_2w_h0h8(t1);

			/*  Reducing coefficient size       */
			compress2_Element_2w_h0h8(t0, t1);

			/* t2:  [ AA |  1 ] = BLEND( [ AA | t0 ] , [ 0 | 0 ] ) */
			for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
			{
				t2[s] = BLEND32(t0[s],ZERO,0xF0);
			}
			t2[0] = OR(t2[0],_mm256_set_epi64x(0,1,0,0));

			/* X2X3: [ X2 | X3 ] = [ AA |  1 ] * [ BB | t1 ]  */
			mul_Element_2w_h0h8(X2X3,t2,t1);
			/*  Reducing coefficient size       */
			compress_Element_2w_h0h8(X2X3);

			/* t2: [  E | __ ] = [ AA | t0 ] - [ BB | t1 ]  */
			/* t1: [a24E| __ ] =         a24 * [  E | __ ]  */
			/* t1: [  F | __ ] = [a24E| __ ] + [ AA | t0 ]  */
			/* t2: [  E | t0 ] = BLEND( [ E | _ ] , [ AA | t0 ] ) */
			/* t1: [  F | X1 ] = BLEND( [ F | _ ] , [ __ | X1 ] ) */
			for (s = 0; s < (NUM_DIGITS_FP448/2); s++)
			{
				t2[s] = ADD(t0[s],SUB(_2P[s],t1[s]));
				t1[s] = ADD(MUL(t2[s], a24),t0[s]);
				t2[s] = BLEND32(t2[s],t0[s],0xF0);
				t1[s] = BLEND32(t1[s],X1[s],0xF0);
			}

			/*  Reducing coefficient size       */
			compressfast_Element_2w_h0h8(t1);
			/* Z2Z3: [ Z2 | Z3 ] = [  E | t0 ] * [  F | X1 ]  */
			mul_Element_2w_h0h8(Z2Z3,t2,t1);
			/*  Reducing coefficient size       */
			compress_Element_2w_h0h8(Z2Z3);
			j--;
		}
		j = 63;
	}
}

/**
 *
 * @param shared_secret
 * @param session_key
 * @param private_key
 * @return
 */
static int x448_shared(
		argECDHX_Key shared_secret,
		argECDHX_Key session_key,
		argECDHX_Key private_key
)  
{
	uint64_t save;
	Element_2w_Fp448 QzPz={ZERO};
	Element_2w_Fp448 QxPx={ZERO};
	Element_1w_Fp448 Z, X, invZ, X1;

	_mm256_zeroupper();
	/** clamp function */
	save = private_key[ECDH448_KEY_SIZE_BYTES-1]<<16 | private_key[0];
	private_key[0] = private_key[0] & (~(uint8_t)0x3);
	private_key[ECDH448_KEY_SIZE_BYTES-1] |= 0x80;

	str_bytes_To_Element_1w_h0h8(X1, session_key);

	/* start with (kP,0P) */
	QzPz[0] = _mm256_set_epi64x(0,1,0,0);
	interleave_2w_h0h8(QxPx,X1,X1);

	/* main-loop */
	step_ladder((uint64_t *) private_key, QxPx, QzPz);
	deinterleave_2w_h0h8(X, X1, QxPx);
	deinterleave_2w_h0h8(Z, X1, QzPz);

	invsqrt_Element_1w_h0h8(invZ, Z,1);
	mul_Element_1w_h0h8(X, X, invZ);
	compress_Element_1w_h0h8(X);
	singleH0H8_To_str_bytes(shared_secret, X);
	private_key[ECDH448_KEY_SIZE_BYTES-1] = (uint8_t)((save>>16) & 0xFF);
	private_key[0] = (uint8_t)(save & 0xFF);
	_mm256_zeroupper();
	return 0;
}

static void point_Edwards2Montgomery_ed448(uint8_t * enc,PointXYZT_2w_H0H8 * P)
{
	/**
	 * Using the 4-isogenus map between
	 * edwards448 -> curve448
	 */
	Element_1w_Fp448 X,Y,inv_X;

	deinterleave_2w_h0h8(X,Y,P->XY);
	invsqrt_Element_1w_h0h8(inv_X,X,1);
	mul_Element_1w_h0h8(X,Y,inv_X);
	compress_Element_1w_h0h8(X);
	sqr_Element_1w_h0h8(X);
	compress_Element_1w_h0h8(X);

	singleH0H8_To_str_bytes(enc,X);
}

/**
 *
 * @param public_key
 * @param private_key
 * @return
 */
static int x448_keygen(
	argECDHX_Key session_key,
	argECDHX_Key private_key
)
{
	int i=0;
	PointXYZT_2w_H0H8 kB;
	ECDH_X448_KEY scalar;

	for(i=0;i<ECDH448_KEY_SIZE_BYTES;i++)
	{
		scalar[i] = private_key[i];
	}

	scalar[0] = scalar[0] & (~(uint8_t)0x3);
	scalar[ECDH448_KEY_SIZE_BYTES-1] |= 0x80;

	fixed_point_multiplication_ed448(&kB,scalar);
	point_Edwards2Montgomery_ed448(session_key,&kB);
	spc_memset(scalar,0,ECDH448_KEY_SIZE_BYTES);
	return 0;
}

