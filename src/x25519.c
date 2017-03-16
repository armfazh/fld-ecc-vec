#include "faz_ecdh_avx2.h"
/****** Implementation of Montgomery Ladder Algorithm ************/
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


static uint8_t * newX25519_Key()
{
	return (uint8_t*) allocate_bytes(ECDH25519_KEY_SIZE_BYTES * sizeof(uint8_t));
}

static __inline void  perm4E_x25519(__m256i * C)
{
    int i=0;
    for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
        C[i] = PERM64(C[i],0x4E);
}

#if SWAP_METHOD == CMOV
static __inline void cmov(int bit, uint64_t * const pX,uint64_t * const pY)
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
    :
    :  "r" (bit),"r" (pX),"r" (pY)
    : "memory", "cc", "%rax", "%rcx"
    );
}
#endif


static __inline void select_cmov_x64(uint64_t bit, uint64_t *const px, uint64_t *const py)
{
	__asm__ __volatile__(
		"test        %2,     %2 \n\t"
		"movq      (%0),  %%rax \n\t"
		"cmovnz    (%1),  %%rax \n\t"
		"movq     %%rax,   (%0) \n\t"

		"movq     8(%0),  %%rax \n\t"
		"cmovnz   8(%1),  %%rax \n\t"
		"movq     %%rax,  8(%0) \n\t"

		"movq    16(%0), %%rax  \n\t"
		"cmovnz  16(%1), %%rax  \n\t"
		"movq    %%rax , 16(%0) \n\t"

		"movq    24(%0), %%rax  \n\t"
		"cmovnz  24(%1), %%rax  \n\t"
		"movq     %%rax, 24(%0) \n\t"
	:
	: "r" (px), "r" (py), "r"  (bit)
	: "cc", "%rax"
	);
}


static const ALIGN uint64_t CONST_2P_2P_Element_2w_Fp25519[2*NUM_DIGITS_FP25519] = {
		0x7ffffda,0x3fffffe,0x7ffffda,0x3fffffe,
		0x3fffffe,0x7fffffe,0x3fffffe,0x7fffffe,
		0x7fffffe,0x3fffffe,0x7fffffe,0x3fffffe,
		0x3fffffe,0x7fffffe,0x3fffffe,0x7fffffe,
		0x7fffffe,0x3fffffe,0x7fffffe,0x3fffffe
};

static __inline void step_ladder_x25519(
		uint64_t * __restrict key,
		 argElement_2w X2X3,
		 argElement_2w Z2Z3)
{
	int i = 0, j = 0, s = 0;
    uint64_t prev = 0;
    const uint64_t param_a24 = 121665;
    const __m256i a24 = _mm256_set1_epi64x(param_a24);

    ALIGN __m256i buffer[5*4];
     argElement_2w X1 = buffer+5*0;
     argElement_2w t0 = buffer+5*1;
     argElement_2w t1 = buffer+5*2;
     argElement_2w t2 = buffer+5*3;
     argElement_2w _2P = ( argElement_2w)CONST_2P_2P_Element_2w_Fp25519;

    for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
    {
        X1[s] = X2X3[s];
    }

	j = 62;
	for (i = 3; i >= 0; i--)
	{
		while (j >= 0) {
            uint64_t bit = (key[i] >> j) & 0x1;
            uint64_t swap = bit ^prev;
            prev = bit;

            /* X2X3: [A|C] = [X2|X3] + [Z2|Z3]  */
            /* Z2Z3: [B|D] = [X2|X3] - [Z2|Z3]  */
			addsub_Element_2w_h0h5(X2X3, Z2Z3);
            /* Z2Z3: [D|B] = Permute([B|D])     */
             perm4E_x25519(Z2Z3);

            /* t0:   [DA|CB] = [A|C] * [D|B]    */
			mul_Element_2w_h0h5(t0, X2X3, Z2Z3);
			compress_Element_2w_h0h5(t0);
            /* t1:   [CB|DA] = Perm([DA|CB])    */
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
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
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
            {
                X2X3[s] = _mm256_permutevar8x32_epi32(X2X3[s],maskbit0);
                Z2Z3[s] = _mm256_permutevar8x32_epi32(Z2Z3[s],maskbit1);
            }
#elif SWAP_METHOD == LOGIC
            /* Using logic arithmetic instructions    */
            const __m256i mask = _mm256_set1_epi64x(-swap);
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
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
			addsub_Element_2w_h0h5(t1,t0);

            /* t0:  [  A | t0 ] = BLEND( [ A | C ] , [ __ | t0 ] ) */
            /* t1:  [  B | t1 ] = BLEND( [ B | D ] , [ __ | t1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
            {
                t0[s] = BLEND32(X2X3[s], t0[s], 0xF0);
                t1[s] = BLEND32(Z2Z3[s], t1[s], 0xF0);
            }

            /* t0:  [ AA | t0 ] = [  A | t0 ]^2 */
			sqr_Element_2w_h0h5(t0);
            /* t1:  [ BB | t1 ] = [  B | t1 ]^2 */
			sqr_Element_2w_h0h5(t1);
            /*  Reducing coefficient size       */
			compress2_Element_2w_h0h5(t0,t1);

            /* t2:  [ AA |  1 ] = BLEND( [ AA | t0 ] , [ 0 | 0 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
            {
                t2[s] = BLEND32(t0[s],ZERO,0xF0);
            }
            t2[0] = OR(t2[0],_mm256_set_epi64x(0,1,0,0));

            /* X2X3: [ X2 | X3 ] = [ AA |  1 ] * [ BB | t1 ]  */
			mul_Element_2w_h0h5(X2X3,t2,t1);
            /*  Reducing coefficient size       */
			compress_Element_2w_h0h5(X2X3);

            /* t2: [  E | __ ] = [ AA | t0 ] - [ BB | t1 ]  */
            /* t1: [a24E| __ ] =         a24 * [  E | __ ]  */
            /* t1: [  F | __ ] = [a24E| __ ] + [ AA | t0 ]  */
            /* t2: [  E | t0 ] = BLEND( [ E | _ ] , [ AA | t0 ] ) */
            /* t1: [  F | X1 ] = BLEND( [ F | _ ] , [ __ | X1 ] ) */
            for (s = 0; s < (NUM_DIGITS_FP25519/2); s++)
            {
                t2[s] = ADD(t0[s],SUB(_2P[s],t1[s]));
                t1[s] = ADD(MUL(t2[s], a24),t0[s]);
                t2[s] = BLEND32(t2[s],t0[s],0xF0);
                t1[s] = BLEND32(t1[s],X1[s],0xF0);
            }

            /*  Reducing coefficient size       */
			compressfast_Element_2w_h0h5(t1);
            /* Z2Z3: [ Z2 | Z3 ] = [  E | t0 ] * [  F | X1 ]  */
			mul_Element_2w_h0h5(Z2Z3,t2,t1);
            /*  Reducing coefficient size       */
			compress_Element_2w_h0h5(Z2Z3);
			j--;
		}
		j = 63;
	}
}

static int x25519_shared_avx2(
		argECDHX_Key shared_secret,
		argECDHX_Key session_key,
		argECDHX_Key private_key
)
{
	uint64_t save;
	Element_2w_Fp25519 QxPx = {ZERO};
	Element_2w_Fp25519 QzPz = {ZERO};
	Element_1w_x64 Z, X, invZ;
	Element_1w_Fp25519 ZZ, XX, X1;

	_mm256_zeroupper();

	/* clampC function */
	save = private_key[ECDH25519_KEY_SIZE_BYTES-1]<<16 | private_key[0];
	private_key[0] = private_key[0] & (~(uint8_t)0x7);
	private_key[ECDH25519_KEY_SIZE_BYTES-1] = (uint8_t)64 | (private_key[ECDH25519_KEY_SIZE_BYTES-1] & (uint8_t)0x7F);
	/**
	 * As in the draft:
	 * When receiving such an array, implementations of curve25519
	 * MUST mask the most-significant bit in the final byte. This
	 * is done to preserve compatibility with point formats which
	 * reserve the sign bit for use in other protocols and to
	 * increase resistance to implementation fingerprinting
	 **/
	session_key[ECDH25519_KEY_SIZE_BYTES-1] &= (1<<(255%8))-1;

	str_bytes_To_Element_1w_h0h5(X1, session_key);

	/* start with (kP,0P) */
	QzPz[0] = _mm256_set_epi64x(0,1,0,0);
	interleave_2w_h0h5(QxPx,X1,X1);

	/* main-loop */
	step_ladder_x25519((uint64_t*)private_key, QxPx, QzPz);
	deinterleave_2w_h0h5(XX, X1, QxPx);
	deinterleave_2w_h0h5(ZZ, X1, QzPz);
	_mm256_zeroupper();
	singleH0H5_To_str_bytes((uint8_t*)X,XX);
	singleH0H5_To_str_bytes((uint8_t*)Z,ZZ);

	inv_Element_1w_x64(invZ, Z);
	mul_Element_1w_x64((uint64_t*)shared_secret, X, invZ);
	fred_Element_1w_x64((uint64_t*)shared_secret);
	private_key[ECDH25519_KEY_SIZE_BYTES-1] = (uint8_t)((save>>16) & 0xFF);
	private_key[0]  = (uint8_t)(save & 0xFF);
	return 0;
}
static void point_Edwards2Montgomery(uint8_t * enc,PointXYZT_2w_H0H5 * P)
{
	Element_1w_x64 add,sub,inv_sub;
	Element_1w_Fp25519 t0,t1,t2;
	Element_2w_Fp25519 addZY,subZY;

	add_Element_2w_h0h5(addZY,P->TZ,P->XY);
	sub_Element_2w_h0h5(subZY,P->TZ,P->XY);
	compress_Element_2w_h0h5(addZY);
	compress_Element_2w_h0h5(subZY);

	deinterleave_2w_h0h5(t2,t0,addZY);
	deinterleave_2w_h0h5(t2,t1,subZY);

	singleH0H5_To_str_bytes((uint8_t*)add,t0);
	singleH0H5_To_str_bytes((uint8_t*)sub,t1);

//	printf("X: ");Fp.fp25519._1way_x64.print(X);
//	printf("Y: ");Fp.fp25519._1way_x64.print(Y);
//	printf("Z: ");Fp.fp25519._1way_x64.print(Z);

	inv_Element_1w_x64(inv_sub,sub);
	mul_Element_1w_x64((uint64_t*)enc,add,inv_sub);
	fred_Element_1w_x64((uint64_t*)enc);

//	printf("x: ");Fp.fp25519._1way_x64.print(enc);
}

#define div_8_256(r)                \
{                               \
    uint64_t bit3 = r[3]<<61;   \
    uint64_t bit2 = r[2]<<61;   \
    uint64_t bit1 = r[1]<<61;   \
    r[3] = (r[3]>>3);           \
    r[2] = (r[2]>>3) | bit3;    \
    r[1] = (r[1]>>3) | bit2;    \
    r[0] = (r[0]>>3) | bit1;    \
}

static __inline int x25519_keygen_avx2(
		argECDHX_Key session_key,
		argECDHX_Key private_key
)
{
	int i=0;
	PointXYZT_2w_H0H5 kB;
	ECDH_X25519_KEY scalar;
	uint64_t * ptrScalar = (uint64_t*)scalar;

	for(i=0;i<ECDH25519_KEY_SIZE_BYTES;i++)
	{
		scalar[i] = private_key[i];
	}
	/* clampC function */
	scalar[0] = scalar[0] & (~(uint8_t)0x7);
	scalar[ECDH25519_KEY_SIZE_BYTES-1] = (uint8_t)64 | (scalar[ECDH25519_KEY_SIZE_BYTES-1] & (uint8_t)0x7F);

	div_8_256(ptrScalar);
	point_multiplication_ed25519(&kB,scalar);
	_1way_doubling_2w_H0H5(&kB);
	_1way_doubling_2w_H0H5(&kB);
	_1way_doubling_2w_H0H5(&kB);

	point_Edwards2Montgomery(session_key,&kB);
	spc_memset(scalar,0,ECDH25519_KEY_SIZE_BYTES);
	return 0;
}

static int x25519_shared_x64(
		argECDHX_Key shared_secret,
		argECDHX_Key session_key,
		argECDHX_Key private_key
)
{
	ALIGN uint64_t buffer[4*NUM_DIGITS_FP25519];
	ALIGN uint64_t coordinates[4*NUM_DIGITS_FP25519];
	ALIGN uint64_t workspace[6*NUM_DIGITS_FP25519];
	Element_1w_x64 X1;
	uint64_t save;

	int i=0, j=0;
	uint64_t prev = 0;
	uint64_t *const key = (uint64_t*)private_key;
	uint64_t *const Px = coordinates+0;
	uint64_t *const Pz = coordinates+4;
	uint64_t *const Qx = coordinates+8;
	uint64_t *const Qz = coordinates+12;
	uint64_t *const X2 = Qx;
	uint64_t *const Z2 = Qz;
	uint64_t *const X3 = Px;
	uint64_t *const Z3 = Pz;
	uint64_t *const X2Z2 = Qx;
	uint64_t *const X3Z3 = Px;

	uint64_t *const A  = workspace+0;
	uint64_t *const B  = workspace+4;
	uint64_t *const D  = workspace+8;
	uint64_t *const C  = workspace+12;
	uint64_t *const DA = workspace+16;
	uint64_t *const CB = workspace+20;
	uint64_t *const AB   = A;
	uint64_t *const DC   = D;
	uint64_t *const DACB = DA;
//	uint64_t *const buffer_1w = buffer;
	uint64_t *const buffer_2w = buffer;

	/* clampC function */
	save = private_key[ECDH25519_KEY_SIZE_BYTES-1]<<16 | private_key[0];
	private_key[0] = private_key[0] & (~(uint8_t)0x7);
	private_key[ECDH25519_KEY_SIZE_BYTES-1] = (uint8_t)64 | (private_key[ECDH25519_KEY_SIZE_BYTES-1] & (uint8_t)0x7F);

	/**
	* As in the draft:
	* When receiving such an array, implementations of curve25519
	* MUST mask the most-significant bit in the final byte. This
	* is done to preserve compatibility with point formats which
	* reserve the sign bit for use in other protocols and to
	* increase resistance to implementation fingerprinting
	**/
	session_key[ECDH25519_KEY_SIZE_BYTES-1] &= (1<<(255%8))-1;

	for(i=0;i<NUM_DIGITS_FP25519_X64;i++)
	{
		Px[i] = X1[i] = ((uint64_t*)session_key)[i];
		Pz[i] = 0;
		Qx[i] = 0;
		Qz[i] = 0;
	}

	Pz[0] = 1;
	Qx[0] = 1;

	/* main-loop */
	j = 62;
	for(i=3;i>=0;i--)
	{
		while(j >= 0)
		{
			uint64_t bit = (key[i]>>j)&0x1;
			uint64_t swap = bit^prev;
			prev = bit;

			add_Element_1w_x64(A,X2,Z2);       /* A = (X2+Z2)                   */
			sub_Element_1w_x64(B,X2,Z2);       /* B = (X2-Z2)                   */
			add_Element_1w_x64(C,X3,Z3);       /* C = (X3+Z3)                   */
			sub_Element_1w_x64(D,X3,Z3);       /* D = (X3-Z3)                   */

			mul_Element_2w_x64(DACB,AB,DC);   /* [DA|CB] = [A|B]*[D|C]         */

			select_cmov_x64(swap, A, C);
			select_cmov_x64(swap, B, D);
			sqr_Element_2w_x64(AB);           /* [AA|BB] = [A^2|B^2]           */

			add_Element_1w_x64(X3,DA,CB);      /* X3 = (DA+CB)                  */
			sub_Element_1w_x64(Z3,DA,CB);      /* Z3 = (DA-CB)                  */
			sqr_Element_2w_x64(X3Z3);         /* [X3|Z3] = [(DA+CB)|(DA+CB)]^2 */

			copy_Element_1w_x64(X2,B);         /* X2 = B^2                      */
			sub_Element_1w_x64(Z2,A,B);        /* Z2 = E = AA-BB                */
			mul_a24_Element_1w_x64(B,Z2);      /*  B = a24*E                    */
			add_Element_1w_x64(B,B,X2);        /* B = a24*E+B                   */
			mul_Element_2w_x64(X2Z2,X2Z2,AB); /* [X2|Z2] = [B|E]*[A|a24*E+B]   */

			mul_Element_1w_x64(Z3,Z3,X1);      /* Z3 = Z3*X1                    */

			j--;
		}
		j = 63;
	}
	inv_Element_1w_x64(A,Qz);
	mul_Element_1w_x64((uint64_t*)shared_secret,Qx,A);
	fred_Element_1w_x64((uint64_t*)shared_secret);
	private_key[ECDH25519_KEY_SIZE_BYTES-1] = (uint8_t)((save>>16) & 0xFF);
	private_key[0]  = (uint8_t)(save & 0xFF);
	return 0;
}

