#include "ecc_ed25519.h"
#include <table_verf_w7_ed25519.h>

#if   LOOKUP_TABLE_SIZE == LUT_12KB
#include <table_sign_w4_12k_ed25519.h>
#elif LOOKUP_TABLE_SIZE == LUT_24KB
#include <table_sign_w4_24k_ed25519.h>
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif

/**
 * This version is intended to compute four independent addition points
 * on four pair of points.
 * Taken from Hisil, Wong,Carter,Dawson
 * Section 3.1 page 6
 *
 * @param Q
 * @param P
 */
void _4way_mixadd_ed25519(PointXYZT_4way_Fp25519 *Q, Point_precmp_4way_Fp25519 *P)
{
	__m256i * const X1 = Q->X;
	__m256i * const Y1 = Q->Y;
	__m256i * const T1 = Q->T;
	__m256i * const Z1 = Q->Z;
	__m256i * const addY2X2 = P->addYX;
	__m256i * const subY2X2 = P->subYX;
	__m256i * const dT2 = P->_2dYX;

	Element_4w_Fp25519 A,B,C,D;
	Element_4w_Fp25519 addY1X1, subY1X1;

	addsub_Element_4w_h0h5(addY1X1,subY1X1,Y1,X1);
	mul_Element_4w_h0h5(A,subY1X1,subY2X2);
	mul_Element_4w_h0h5(B,addY1X1,addY2X2);
	mul_Element_4w_h0h5(C,dT2,T1);
	add_Element_4w_h0h5(D,Z1,Z1);

	addsub_large_Element_4w_h0h5(B,A);
	addsub_large_Element_4w_h0h5(D,C);
	compress2_Element_4w_h0h5(B,A);
	compress2_Element_4w_h0h5(D,C);

	mul_Element_4w_h0h5(X1,A,C);
	mul_Element_4w_h0h5(T1,A,B);
	mul_Element_4w_h0h5(Z1,D,C);
	mul_Element_4w_h0h5(Y1,D,B);

    compress2_Element_4w_h0h5(X1,T1);
    compress2_Element_4w_h0h5(Y1,Z1);
}

/**
 * Addition law for twisted Edwards curves
 * Hisil Section 3.1 page 6
 *
 * @param Q
 * @param P
 */
void _2way_fulladd(PointXYZT_2way_Fp25519 *Q, PointXYZT_2way_Fp25519 *P)
{
	__m256i * const X1 = Q->X; __m256i * const X2 = P->X;
	__m256i * const Y1 = Q->Y; __m256i * const Y2 = P->Y;
	__m256i * const Z1 = Q->Z; __m256i * const Z2 = P->Z;
	__m256i * const T1 = Q->T; __m256i * const T2 = P->T;

	Element_2w_Fp25519 A,B,C,D;
	const Element_2w_Fp25519 param_2d_H0H5 = {
			_mm256_set_epi64x(0x0f3d130,0x2b2f159,0x0f3d130,0x2b2f159),
			_mm256_set_epi64x(0x3407977,0x1a6e509,0x3407977,0x1a6e509),
			_mm256_set_epi64x(0x19ce331,0x22add7a,0x19ce331,0x22add7a),
			_mm256_set_epi64x(0x1c56dff,0x0d4141d,0x1c56dff,0x0d4141d),
			_mm256_set_epi64x(0x0901b67,0x0038052,0x0901b67,0x0038052)};

	add_Element_2w_h0h5(Z1,Z1,Z1);
	addsub_Element_2w_h0h5(Y1,X1);
	addsub_Element_2w_h0h5(Y2,X2);
	compressfast_Element_2w_h0h5(X2);
	compressfast_Element_2w_h0h5(Y2);

	mul_Element_2w_h0h5(A,X1,X2);		    compress_Element_2w_h0h5(A);
	mul_Element_2w_h0h5(B,Y1,Y2);		    compress_Element_2w_h0h5(B);
	mul_Element_2w_h0h5(C,T1,T2);           compress_Element_2w_h0h5(C);
	mul_Element_2w_h0h5(C,C,(argElement_2w)param_2d_H0H5);	compress_Element_2w_h0h5(C);
	mul_Element_2w_h0h5(D,Z1,Z2);			compress_Element_2w_h0h5(D);

	addsub_Element_2w_h0h5(B,A);
	addsub_Element_2w_h0h5(D,C);/* [TODO] put this additions before reduction */
	compressfast_Element_2w_h0h5(C);
	compressfast_Element_2w_h0h5(B);

	mul_Element_2w_h0h5(X1,A,C);
	mul_Element_2w_h0h5(T1,A,B);
	mul_Element_2w_h0h5(Z1,D,C);
	mul_Element_2w_h0h5(Y1,D,B);

	compress_Element_2w_h0h5(X1);
	compress_Element_2w_h0h5(T1);
	compress_Element_2w_h0h5(Z1);
	compress_Element_2w_h0h5(Y1);
}





#if LOOKUP_TABLE_SIZE == LUT_12KB

/**
 * Recoding to a signed radix 2^OMEGA (fold4w4)
 * Given:
 *      r   A 256-bit integer
 * Returns:
 *      (-1)^(S[i])*K[1] for i \in [0,64]
 * such that
 *      K[i] \in {0, 1, 2, ..., 2^OMEGA}
 *      S[i]=000..0  if k[i] is positive
 *      S[i]=111..1  if k[i] is negative
 *
 * K[i] = 64 bits (quad-word)
 * K_i  = 4 bits (nibble)
 *
 * K[ 0, 1, 2, 3] = [ n_0, n_1,   n_2,  n_3]
 * K[ 4, 5, 6, 7] = [ n_4, n_5,   n_6,  n_7]
 * K[ 8, 9,10,11] = [ n_8, n_9,  n_10, n_11]
 * K[12,13,14,15] = [n_12, n_13, n_14, n_15]
 * ...
 * K[60,61,62,63] = [n_60, n_61, n_62, n_63]
 *
 * @param list_signs
 * @param list_digits
 * @param r
 */
void recoding_signed_scalar_fold4w4_ed25519(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r)
{
	const int OMEGA = 4;
	int i,j;
	int8_t carry=0;
	uint8_t nibble_low, nibble_hig;
	int8_t value,digit;

	for(i=0;i<2;i++)
	{
		for(j=0;j<16;j++)
		{
			nibble_low = r[16*i+j] & 0xF;
			value = nibble_low + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[32*i+2*j+0] = (int64_t) digit;
			list_signs [32*i+2*j+0] = (int64_t) -carry;

			nibble_hig = (r[16*i+j] >> 4) & 0xF;
			value = nibble_hig + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[32*i+2*j+1] = (int64_t) digit;
			list_signs [32*i+2*j+1] = (int64_t) -carry;
		}
	}/* list_digits[64] = carry; */ /* This is always equal to 0 iff r < 2**253 */
}

/**
 * Query Table
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
void query_table_fold4w4_ed25519(Point_precmp_4way_Fp25519 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
	const __m256i _P[10] = {
			SET1_64(0x3ffffed),	SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),	SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),	SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),	SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),	SET1_64(0x1ffffff)
	};
	const __m256i one = SET1_64(1);
	int i,j;
	__m256i mask[9];
	__m256i P_addYX[4], P_subYX[4], P__2dYX[4];
	__m256i digits = LOAD(secret_digits);
	__m256i signs  = LOAD(secret_signs);
	__m256i iiii = ZERO;
	uint64_t * p64Table = (uint64_t*)table;

	/* create a set of masks */
	for(i=0;i<9;i++)
	{
		/**
		 * Determine if iiii is different from digits
		 * using they following relation:
		 *    mask = (x-y) OR (y-x)
		 *    63th bit = 1   if X   equal   Y
		 *    63th bit = 0   if X not equal Y
		 * */
		mask[i] = OR(SUB(digits,iiii),SUB(iiii,digits));
		/* propagate 63th bit to the whole 64-bit word. */
		mask[i] = _mm256_shuffle_epi32(_mm256_srai_epi32(mask[i],32),0xE5);
		iiii = ADD(iiii,one);
	}
	/**
	 * Queries to the table for points 1B,2B,3B,...,8B.
	 * acc = \sum 1B&¬mask[1] + 2B&¬mask[2] + 3B&¬mask[3] + ... + 8B&¬mask[8]
	 * if mask[i] == all_zeros for some i € {1,2,...,8}
	 * 	     then iB was selected
	 * if mask[i] == all_ones for all i
	 * 	     then no point was selected
	 */
	for(j=0;j<4;j++)//num of 64-bit words
	{
		P_addYX[j] = ZERO;
		P_subYX[j] = ZERO;
		P__2dYX[j] = ZERO;
		for(i=0;i<8;i++) /* num of multiples {1B,2B,3B,...,8B} */
		{
			__m256i addYX,subYX,_2dYX;
			BROADCASTQ(addYX,p64Table+24*j+3*i+0);
			BROADCASTQ(subYX,p64Table+24*j+3*i+1);
			BROADCASTQ(_2dYX,p64Table+24*j+3*i+2);

			P_addYX[j] = XOR(P_addYX[j],ANDNOT(mask[i+1],addYX));
			P_subYX[j] = XOR(P_subYX[j],ANDNOT(mask[i+1],subYX));
			P__2dYX[j] = XOR(P__2dYX[j],ANDNOT(mask[i+1],_2dYX));
		}
	}
	/**
	 * In case that the multiple queried be the 0B,
	 * then to construct the point at infinity
	 * we just append a 1 (one).
	 */
	P_addYX[0] = XOR(P_addYX[0], ANDNOT(mask[0],XOR(P_addYX[0],one)));
	P_subYX[0] = XOR(P_subYX[0], ANDNOT(mask[0],XOR(P_subYX[0],one)));
	/**
	 * Point Sign Verification (Part 1/2)
	 * if secret_sign == 1
	 *    addYX <- subYX
	 *    subYX <- addYX
	 * otherwise
	 *    they remain unchanged
	 */
	for(j=0;j<4;j++)
	{
		__m256i swap = AND(XOR(P_addYX[j], P_subYX[j]), signs);
		P_addYX[j] = XOR(P_addYX[j], swap);
		P_subYX[j] = XOR(P_subYX[j], swap);
	}

	str_bytes_to_Element_4w_h0h5(P->addYX,P_addYX);
	str_bytes_to_Element_4w_h0h5(P->subYX,P_subYX);
	str_bytes_to_Element_4w_h0h5(P->_2dYX,P__2dYX);

	/**
	 * Point Sign Verification (Part 2/2)
	 * if signs == 111111111
	 *    _2dYX <-- -_2dYX + P
	 * otherwise
	 *    _2dYX <--  _2dYX + P
	 */
	for(i=0;i<10;i++)
	{
		P->_2dYX[i] = ADD( XOR(P->_2dYX[i],signs), SUB(_P[i],signs));
	}
}

/**
 * This function will use a pre-computed table of 12KB.
 * Folding 4 means four queries at the same time.
 * Using w=4, means that scalar will be partitioned in
 * 4-bit signed digits.
 * This produces to compute 16 iterations in the main loop.
 * as follows:
 *		S_0 = r_0 + 16^4 r_4 + 16^8 r_8  + ... + 16^60 r_60
 *		S_1 = r_1 + 16^4 r_5 + 16^8 r_9  + ... + 16^61 r_61
 *		S_2 = r_2 + 16^4 r_6 + 16^8 r_10 + ... + 16^62 r_62
 *		S_3 = r_3 + 16^4 r_7 + 16^8 r_11 + ... + 16^63 r_63
 * Then recombining partial sums with:
 *		S = S_0 + 16^1 S_1 + 16^2 S_2 + 16^3 S_3.
 * @param rB
 * @param r
 */
void point_multiplication_fold4w4_ed25519(uint8_t *rB, uint8_t *r)
{
	int i;
	PointXYZT_4way_Fp25519 Q;
	Point_precmp_4way_Fp25519 P;
	PointXYZT_2w_H0H5 Q0,Q1,Q2,Q3;
	Element_1w_Fp25519 XX, YY, ZZ, invZZ;
	Element_1w_x64 X, Y, Z, invZ;
	ALIGN uint64_t K[64];
	ALIGN uint64_t S[64];
	const Element_4w_Fp25519 one_half = {
			SET1_64(0x3fffff7),SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),SET1_64(0x0ffffff)
	};

	recoding_signed_scalar_fold4w4_ed25519(S,K,r);

	Q.Z[0] = SET1_64(2);
	for(i=1;i<10;i++)
	{
		Q.Z[i] = ZERO;
	}

	query_table_fold4w4_ed25519(&P, ((uint8_t*)TableSign_w4_12k),S,K);
	sub_Element_4w_h0h5(Q.X,P.addYX,P.subYX);
	add_Element_4w_h0h5(Q.Y,P.addYX,P.subYX);
	new_compressfast2_Element_4w_h0h5(Q.X,Q.Y);
	mul_Element_4w_h0h5(Q.T,Q.X,Q.Y);						compress_Element_4w_h0h5(Q.T);
	mul_Element_4w_h0h5(Q.T,Q.T,(argElement_4w)one_half);	compress_Element_4w_h0h5(Q.T);

	for(i=1;i<16;i++)
	{
		query_table_fold4w4_ed25519(&P, ((uint8_t*)TableSign_w4_12k)+768*i,S+4*i,K+4*i);
		_4way_mixadd_ed25519(&Q, &P);
	}

	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i X_C50A50 = UPKL64(Q.X[2*i],Q.X[2*i+1]);
		__m256i Y_C50A50 = UPKL64(Q.Y[2*i],Q.Y[2*i+1]);
		__m256i Z_C50A50 = UPKL64(Q.Z[2*i],Q.Z[2*i+1]);
		__m256i T_C50A50 = UPKL64(Q.T[2*i],Q.T[2*i+1]);

		__m256i X_D50B50 = UPKH64(Q.X[2*i],Q.X[2*i+1]);
		__m256i Y_D50B50 = UPKH64(Q.Y[2*i],Q.Y[2*i+1]);
		__m256i Z_D50B50 = UPKH64(Q.Z[2*i],Q.Z[2*i+1]);
		__m256i T_D50B50 = UPKH64(Q.T[2*i],Q.T[2*i+1]);

		Q0.XY[i] = PERM128(X_C50A50,Y_C50A50,0x20);
		Q2.XY[i] = PERM128(X_C50A50,Y_C50A50,0x31);
		Q0.TZ[i] = PERM128(T_C50A50,Z_C50A50,0x20);
		Q2.TZ[i] = PERM128(T_C50A50,Z_C50A50,0x31);

		Q1.XY[i] = PERM128(X_D50B50,Y_D50B50,0x20);
		Q3.XY[i] = PERM128(X_D50B50,Y_D50B50,0x31);
		Q1.TZ[i] = PERM128(T_D50B50,Z_D50B50,0x20);
		Q3.TZ[i] = PERM128(T_D50B50,Z_D50B50,0x31);
	}

	_1way_doubling_2w_H0H5(&Q3);
	_1way_doubling_2w_H0H5(&Q3);
	_1way_doubling_2w_H0H5(&Q3);
	_1way_doubling_2w_H0H5(&Q3);
	_1way_fulladd_2w_H0H5(&Q2,&Q3);
	_1way_doubling_2w_H0H5(&Q2);
	_1way_doubling_2w_H0H5(&Q2);
	_1way_doubling_2w_H0H5(&Q2);
	_1way_doubling_2w_H0H5(&Q2);
	_1way_fulladd_2w_H0H5(&Q1,&Q2);
	_1way_doubling_2w_H0H5(&Q1);
	_1way_doubling_2w_H0H5(&Q1);
	_1way_doubling_2w_H0H5(&Q1);
	_1way_doubling_2w_H0H5(&Q1);
	_1way_fulladd_2w_H0H5(&Q0,&Q1);

	deinterleave_2w_h0h5(XX,YY,Q0.XY);
	deinterleave_2w_h0h5(invZZ,ZZ,Q0.TZ);
	singleH0H5_To_str_bytes((uint8_t*)X,XX);
	singleH0H5_To_str_bytes((uint8_t*)Y,YY);
	singleH0H5_To_str_bytes((uint8_t*)Z,ZZ);

	inv_Element_1w_x64(invZ,Z);
	mul_Element_1w_x64(X,X,invZ);
	mul_Element_1w_x64((uint64_t*)rB,Y,invZ);
	fred_Element_1w_x64(X);
	fred_Element_1w_x64((uint64_t*)rB);

	rB[31] = rB[31] | (uint8_t)((X[0]&0x1)<<7);
}

#elif LOOKUP_TABLE_SIZE == LUT_24KB
/**
 * Recoding to a signed radix 2^OMEGA (fold2w4)
 * Given:
 *      r   A 256-bit integer
 * Returns:
 *      (-1)^(S[i])*K[1] for i \in [0,64]
 * such that
 *      K[i] \in {0, 1, 2, ..., 2^OMEGA}
 *      S[i]=000..0  if k[i] is positive
 *      S[i]=111..1  if k[i] is negative
 *
 * K[i] = 64 bits (quad-word)
 * K_i  = 4 bits (nibble)
 *
 * K[ 0, 1, 2, 3] = [n_0,n_32,n_1,n_33]
 * K[ 4, 5, 6, 7] = [n_2,n_34,n_3,n_35]
 * K[ 8, 9,10,11] = [n_4,n_36,n_5,n_37]
 * K[12,13,14,15] = [n_6,n_38,n_7,n_39]
 * ...
 * K[60,61,62,63] = [n_30,n_62,n_31,n_63]
 * @param list_signs
 * @param list_digits
 * @param r
 */
void recoding_signed_scalar_fold2w4_ed25519(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r)
{
	const int OMEGA = 4;
	int i,j;
	int8_t carry=0;
	uint8_t nibble_low, nibble_hig;
	int8_t value,digit;

	for(i=0;i<2;i++)
	{
		for(j=0;j<16;j++)
		{
			nibble_low = r[16*i+j] & 0xF;
			value = nibble_low + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[4*j+i+0] = (int64_t) digit;
			list_signs [4*j+i+0] = (int64_t) -carry;

			nibble_hig = (r[16*i+j] >> 4) & 0xF;
			value = nibble_hig + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[4*j+i+2] = (int64_t) digit;
			list_signs [4*j+i+2] = (int64_t) -carry;
		}
	}
	/* list_digits[64] = carry;*/ /*This is always equal to 0 iff r < 2**253 */
}

/**
 * Query table
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
static void query_table_fold2w4_ed25519(Point_precmp_4way_Fp25519 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
	const __m256i _P[10] = {
			SET1_64(0x3ffffed),	SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),	SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),	SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),	SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),	SET1_64(0x1ffffff)
	};
	const __m256i one = SET1_64(1);
	int i,j;
	__m256i mask[8+1];
	__m256i P_addYX[4], P_subYX[4], P__2dYX[4];
	__m256i digits = LOAD(secret_digits);
	__m256i signs  = LOAD(secret_signs);
	__m256i iiii = ZERO;

	/* create a set of masks */
	for(i=0;i<8+1;i++)
	{
		/**
		 * Determine if iiii is different from digits
		 * using they following relation:
		 *    mask = (x-y) OR (y-x)
		 *    63th bit = 1   if X   equal   Y
		 *    63th bit = 0   if X not equal Y
		 * */
		mask[i] = OR(SUB(digits,iiii),SUB(iiii,digits));
		/* propagate 63th bit to the whole 64-bit word. */
		mask[i] = _mm256_shuffle_epi32(_mm256_srai_epi32(mask[i],32),0xE5);
		iiii = ADD(iiii,one);
	}
	/**
	 * Queries to the table for points 1B,2B,3B,...,8B.
	 * acc = \sum 1B&¬mask[1] + 2B&¬mask[2] + 3B&¬mask[3] + ... + 8B&¬mask[8]
	 * if mask[i] == all_zeros for some i € {1,2,...,8}
	 * 	     then iB was selected
	 * if mask[i] == all_ones for all i
	 * 	     then no point was selected
	 */
	for(j=0;j<4;j++)/* num of 64-bit words */
	{
		P_addYX[j] = ZERO;
		P_subYX[j] = ZERO;
		P__2dYX[j] = ZERO;
		for(i=0;i<8;i++)/* num of multiples {1B,2B,3B,...,8B} */
		{
			__m256i addYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)table + 24*j + 3*i + 0));
			__m256i subYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)table + 24*j + 3*i + 1));
			__m256i _2dYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)table + 24*j + 3*i + 2));

			P_addYX[j] = XOR(P_addYX[j],ANDNOT(mask[i+1],addYX));
			P_subYX[j] = XOR(P_subYX[j],ANDNOT(mask[i+1],subYX));
			P__2dYX[j] = XOR(P__2dYX[j],ANDNOT(mask[i+1],_2dYX));
		}
	}
	/**
	 * In case that the multiple queried be the 0B,
	 * then to construct the point at infinity
	 * we just append a 1 (one).
	 */
	P_addYX[0] = XOR(P_addYX[0], ANDNOT(mask[0],XOR(P_addYX[0],one)));
	P_subYX[0] = XOR(P_subYX[0], ANDNOT(mask[0],XOR(P_subYX[0],one)));
	/**
	 * Point Sign Verification (Part 1/2)
	 * if secret_sign == 1
	 *    addYX <- subYX
	 *    subYX <- addYX
	 * otherwise
	 *    they remain unchanged
	 */
	for(j=0;j<4;j++)
	{
		__m256i swap = AND(XOR(P_addYX[j], P_subYX[j]), signs);
		P_addYX[j] = XOR(P_addYX[j], swap);
		P_subYX[j] = XOR(P_subYX[j], swap);
	}

	str_bytes_to_Element_4w_h0h5(P->addYX,P_addYX);
	str_bytes_to_Element_4w_h0h5(P->subYX,P_subYX);
	str_bytes_to_Element_4w_h0h5(P->_2dYX,P__2dYX);

	/**
	 * Point Sign Verification (Part 2/2)
	 * if signs == 111111111
	 *    _2dYX <-- -_2dYX + P
	 * otherwise
	 *    _2dYX <--  _2dYX + P
	 */
	for(i=0;i<10;i++)
	{
		P->_2dYX[i] = ADD( XOR(P->_2dYX[i],signs), SUB(_P[i],signs));
	}
}

/**
 *
 * @param pA
 * @param pB
 * @param A
 */
static void unzip_2w_Element_4w_h0h5(argElement_2w pA,argElement_2w pB,argElement_4w A)
{
	pA[0] = UPKL64(A[0],A[1]);	pB[0] = UPKH64(A[0],A[1]);
	pA[1] = UPKL64(A[2],A[3]);	pB[1] = UPKH64(A[2],A[3]);
	pA[2] = UPKL64(A[4],A[5]);	pB[2] = UPKH64(A[4],A[5]);
	pA[3] = UPKL64(A[6],A[7]);	pB[3] = UPKH64(A[6],A[7]);
	pA[4] = UPKL64(A[8],A[9]);	pB[4] = UPKH64(A[8],A[9]);
}


/**
 * Given Q = [P3,P2,P1,P0]
 * Computes Q0 as:
 * 		R0 <-- [P0,P2]
 * 		R1 <-- [P1,P3]
 * 		R0 <-- R0 + R1         (2-way)
 * 		[Q0,Q1] <-- R0
 * 		Q1 <-- (2**4)*Q1       (1-way)
 * 		Q0 <-- Q0+Q1 		   (1-way)
 * @param T0
 * @param Q
 */
static void join_points(PointXYZT_2w_H0H5 *T0, PointXYZT_4way_Fp25519 *Q)
{
	int i;
	PointXYZT_2way_Fp25519 R0,R1;
	PointXYZT_2w_H0H5 T1;
	unzip_2w_Element_4w_h0h5(R0.X, R1.X, Q->X);
	unzip_2w_Element_4w_h0h5(R0.Y, R1.Y, Q->Y);
	unzip_2w_Element_4w_h0h5(R0.Z, R1.Z, Q->Z);
	unzip_2w_Element_4w_h0h5(R0.T, R1.T, Q->T);

	_2way_fulladd(&R0, &R1);

	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		T0->XY[i] = PERM128(R0.X[i],R0.Y[i],0x20);
		T1.XY[i]  = PERM128(R0.X[i],R0.Y[i],0x31);
		T0->TZ[i] = PERM128(R0.T[i],R0.Z[i],0x20);
		T1.TZ[i]  = PERM128(R0.T[i],R0.Z[i],0x31);
	}

	_1way_doubling_2w_H0H5(&T1);
	_1way_doubling_2w_H0H5(&T1);
	_1way_doubling_2w_H0H5(&T1);
	_1way_doubling_2w_H0H5(&T1);

	_1way_fulladd_2w_H0H5(T0,&T1);
}

/**
 * This function will use a pre-computed table of 24KB.
 * Folding 2 means two queries at the same time.
 * Using w=4, means that scalar will be partitioned in
 * 4-bit signed digits.
 * This produces to compute 16 iterations in the main loop.
 * as follows:
 *		S_0 = r_0  + 16^2  r_2  + 16^4  r_4  + ... + 16^30 r_30
 *		S_1 = r_1  + 16^3  r_3  + 16^5  r_5  + ... + 16^31 r_31
 *		S_2 = r_32 + 16^34 r_34 + 16^36 r_36 + ... + 16^62 r_62
 *		S_3 = r_33 + 16^35 r_35 + 16^37 r_37 + ... + 16^63 r_63
 * Then recombining partial sums with:
 *		S = S_0 + 16^1 S_1 + S_2 + 16^1 S_3
 * @param rB
 * @param r
 */
static void point_multiplication_fold2w4_ed25519(uint8_t *rB, uint8_t *r)
{
	int i;
	PointXYZT_4way_Fp25519 Q;
	Point_precmp_4way_Fp25519 P;
	PointXYZT_2w_H0H5 Q0;
	Element_1w_Fp25519 XX, YY, ZZ, invZZ;
	Element_1w_x64 X, Y, Z, invZ;
	ALIGN uint64_t K[64];
	ALIGN uint64_t S[64];
	const Element_4w_Fp25519 one_half = {
			SET1_64(0x3fffff7),SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),SET1_64(0x1ffffff),
			SET1_64(0x1ffffff),SET1_64(0x3ffffff),
			SET1_64(0x3ffffff),SET1_64(0x0ffffff)
	};

	recoding_signed_scalar_fold2w4_ed25519(S,K,r);

	Q.Z[0] = SET1_64(2);
	for(i=1;i<10;i++)
	{
		Q.Z[i] = ZERO;
	}

	query_table_fold2w4_ed25519(&P, ((uint8_t*)TableSign_w4_24k),S,K);
	sub_Element_4w_h0h5(Q.X,P.addYX,P.subYX);
	add_Element_4w_h0h5(Q.Y,P.addYX,P.subYX);
	new_compressfast2_Element_4w_h0h5(Q.X,Q.Y);
	mul_Element_4w_h0h5(Q.T,Q.X,Q.Y);						compress_Element_4w_h0h5(Q.T);
	mul_Element_4w_h0h5(Q.T,Q.T,(argElement_4w)one_half);	compress_Element_4w_h0h5(Q.T);

	for(i=1;i<16;i++)
	{
		query_table_fold2w4_ed25519(&P, ((uint8_t*)TableSign_w4_24k)+1536*i,S+4*i,K+4*i);
		_4way_mixadd_ed25519(&Q, &P);
	}

	join_points(&Q0, &Q);

	deinterleave_2w_h0h5(XX,YY,Q0.XY);
	deinterleave_2w_h0h5(invZZ,ZZ,Q0.TZ);
	singleH0H5_To_str_bytes((uint8_t*)X,XX);
	singleH0H5_To_str_bytes((uint8_t*)Y,YY);
	singleH0H5_To_str_bytes((uint8_t*)Z,ZZ);

	inv_Element_1w_x64(invZ,Z);
	mul_Element_1w_x64(X,X,invZ);
	mul_Element_1w_x64((uint64_t*)rB,Y,invZ);
	fred_Element_1w_x64(X);
	fred_Element_1w_x64((uint64_t*)rB);

	rB[31] = rB[31] | (uint8_t)((X[0]&0x1)<<7);
}
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif

/**
 *
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
void query_table_ed25519(Point_precmp_4way_Fp25519 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
#if LOOKUP_TABLE_SIZE == LUT_12KB
	query_table_fold4w4_ed25519(P,table,secret_signs,secret_digits);
#elif LOOKUP_TABLE_SIZE == LUT_24KB
	query_table_fold2w4_ed25519(P,table,secret_signs,secret_digits);
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif
}

/**
 * Given:
 *      STR_BYTES r
 * Computes
 *      rB = [r]B
 * Ensuring that B is the generator of Ed25519.
 * @param rB
 * @param r
 */
static void point_multiplication_ed25519(uint8_t *rB, uint8_t *r)
{
#if LOOKUP_TABLE_SIZE == LUT_12KB
	point_multiplication_fold4w4_ed25519(rB,r);
#elif LOOKUP_TABLE_SIZE == LUT_24KB
	point_multiplication_fold2w4_ed25519(rB,r);
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif
}


/**
 * Given a compressed point xy,
 * this function returns:
 * 		P = (-x,y)
 * @param P
 * @param A
 */
static void point_decoding_ed25519(PointXYZT_2w_H0H5 *P, const uint8_t *A)
{
	Element_1w_x64 param_curve_d = {
		 0x75eb4dca135978a3,
		 0x00700a4d4141d8ab,
		 0x8cc740797779e898,
		 0x52036cee2b6ffe73
	};
	const uint64_t *const p64_A =(const uint64_t*)A;
	Element_1w_x64 X,Y,T,u,v;
	Element_1w_x64 one = {0};
	uint8_t bit_X;

	one[0] = 1;
	bit_X = A[31]>>7;
	copy_Element_1w_x64(u,p64_A);
	u[3] &= ((uint64_t)1<<63)-1;
	copy_Element_1w_x64(Y,u);
	/*  u = y^2    */
	sqr_Element_1w_x64(u);
	/*  v = dy^2   */
	mul_Element_1w_x64(v,u,param_curve_d);
	/*  u =  y^2-1 */
	sub_Element_1w_x64(u,u,one);
	/*  v = dy^2+1 */
	add_Element_1w_x64(v,v,one);
	/*  X = sqrt(u/v) */
	invsqrt_Element_1w_x64(X,u,v);
	/*  Checking whether
	 *  X is positive or negative
	 */
	fred_Element_1w_x64(X);
	if(bit_X == (X[0]&0x1))
	{
		neg_Element_1w_x64(X);
	}
	/* Point represented in extended coordinates */
	mul_Element_1w_x64(T,X,Y);
	Element_1w_x64_to_Element_2w_h0h5(P->XY,X,Y);
	Element_1w_x64_to_Element_2w_h0h5(P->TZ,T,one);
}

#define is_greater_equal_one_256(r) ((r[3]!=0)||(r[2]!=0)||(r[1]!=0)||(r[0]>=(uint64_t)1))
#define is_odd(r) ((r[0]&0x1)==1)
#define div_2_256(r)                \
{                               \
    uint64_t bit3 = r[3]<<63;   \
    uint64_t bit2 = r[2]<<63;   \
    uint64_t bit1 = r[1]<<63;   \
    r[3] = (r[3]>>1);           \
    r[2] = (r[2]>>1) | bit3;    \
    r[1] = (r[1]>>1) | bit2;    \
    r[0] = (r[0]>>1) | bit1;    \
}
#define SUB256(r3,r2,r1,r0,b3,b2,b1,b0)       \
	__asm__ __volatile__ (                    \
		"subq %7, %3 \n\t"                    \
		"sbbq %6, %2 \n\t"                    \
		"sbbq %5, %1 \n\t"                    \
		"sbbq %4, %0 \n\t"                    \
		: "+r"(r3),"+r"(r2),"+r"(r1),"+r"(r0) \
		: "r"(b3),"r"(b2),"r"(b1),"r"(b0)     \
        : "memory","cc")
/**
 *
 * @param K
 * @param p8_r
 * @param w
 * @return
 */
int wnaf_256bits(int8_t *K, const uint8_t *p8_r, int w)
{
	int i = 0;
	int64_t value;
	ALIGN uint64_t r[4];
	r[0] = ((uint64_t *)p8_r)[0];
	r[1] = ((uint64_t *)p8_r)[1];
	r[2] = ((uint64_t *)p8_r)[2];
	r[3] = ((uint64_t *)p8_r)[3];

	while(is_greater_equal_one_256(r))
	{
		if(is_odd(r))
		{
			value = r[0] & (((uint64_t)1<<w)-1);
			if(value>(1<<(w-1)))
			{
				value = value - (1<<w);
			}
			SUB256(r[3],r[2],r[1],r[0],value>>63,value>>63,value>>63,value);
		}
		else
		{
			value = 0;
		}
		K[i] = (int8_t)(value&0xFF);
		div_2_256(r);
		i++;
	}
	return i;
}
/**
 *
 * @param table
 * @param P
 */
static void precompute_points_ed25519(PointXYZT_precompute_2w_H0H5 * table, PointXYZT_2w_H0H5* P)
{
	const uint64_t param_curve_2d_02[20] = {
			0x2b2f159,0x0f3d130,0x2,0x0,
			0x1a6e509,0x3407977,0x0,0x0,
			0x22add7a,0x19ce331,0x0,0x0,
			0x0d4141d,0x1c56dff,0x0,0x0,
			0x0038052,0x0901b67,0x0,0x0};
	const int num = (1<<(OMEGA_DYNAMIC-2));

	int i=0;
	PointXYZT_2w_H0H5 iP;
	PointXYZT_precompute_2w_H0H5 _2P_precmp;

	copy_Element_2w_h0h5(iP.XY,P->XY);
	copy_Element_2w_h0h5(iP.TZ,P->TZ);
	_1way_doubling_2w_H0H5(P);

	subadd_Element_2w_h0h5(_2P_precmp.subaddYX,P->XY,1);
	mul_Element_2w_h0h5(_2P_precmp._2dT_2Z,P->TZ,(argElement_2w)param_curve_2d_02);
	compress2_Element_2w_h0h5(_2P_precmp.subaddYX,_2P_precmp._2dT_2Z);

	subadd_Element_2w_h0h5(table[i].subaddYX,iP.XY,1);
	mul_Element_2w_h0h5(table[i]._2dT_2Z,iP.TZ,(argElement_2w)param_curve_2d_02);
	compress2_Element_2w_h0h5(table[i].subaddYX,table[i]._2dT_2Z);
	for(i=1;i<num;i++)
	{
		_1way_mixadd_2w_H0H5(&iP, &_2P_precmp);
		subadd_Element_2w_h0h5(table[i].subaddYX,iP.XY,1);
		mul_Element_2w_h0h5(table[i]._2dT_2Z,iP.TZ,(argElement_2w)param_curve_2d_02);
		compress2_Element_2w_h0h5(table[i].subaddYX,table[i]._2dT_2Z);
	}
}

static const uint64_t CONST_2_to_35P[2*NUM_DIGITS_FP25519] = {
        0x1fffff6800000000,0x0ffffff800000000,0x1fffff6800000000,0x0ffffff800000000,
        0x0ffffff800000000,0x1ffffff800000000,0x0ffffff800000000,0x1ffffff800000000,
        0x1ffffff800000000,0x0ffffff800000000,0x1ffffff800000000,0x0ffffff800000000,
        0x0ffffff800000000,0x1ffffff800000000,0x0ffffff800000000,0x1ffffff800000000,
        0x1ffffff800000000,0x0ffffff800000000,0x1ffffff800000000,0x0ffffff800000000
};
/**
 * Computes the doubling of ONE point
 * stored in P = {XY,ZT}
 * @param P
 */
void _1way_doubling_2w_H0H5(PointXYZT_2w_H0H5 *P)
{
	int i;
    argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P;
	argElement_2w XY = P->XY;
	argElement_2w TZ = P->TZ;
    Element_2w_Fp25519 EG,HF;

	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		EG[i] = PERM64(TZ[i],0xEE);
	}
	sqr_Element_2w_h0h5(XY);            /* [A|B] = [X^2 | Y^2 ] */
	mul_Element_2w_h0h5(TZ,TZ,EG);		/* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
	add_Element_2w_h0h5(TZ,TZ,TZ);		/* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i _BG,AC,_0B,BA;
		BA    = PERM64(XY[i], 0x4E);			/* [B|A] */
		_0B   = BLEND32(XY[i], ZERO,  0x0F); 	/* [0|B] */
		AC    = BLEND32(XY[i], TZ[i], 0xF0);	/* [A|C] */
		_BG   = SUB(_0B,BA);					/* [-B|G] = [0|B] - [B|A] = [-B|B-A] */
		EG[i] = BLEND32(_BG,TZ[i],0x0F);		/* [E|G] */
		HF[i] = SUB(_BG,AC);					/* [H|F] = [-B|G] - [A|C] */
		EG[i] = ADD(EG[i],_2_to_35P[i]);
		HF[i] = ADD(HF[i],_2_to_35P[i]);
	}
	compress2_Element_2w_h0h5(HF,EG);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
	mul2_Element_2w_h0h5(TZ,XY,HF,EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
	Element_2w_Fp25519 FH;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	mul_Element_2w_h0h5(XY,FH,EG);
	mul_Element_2w_h0h5(TZ,HF,EG);
#else
	#error "Define the PROCESSOR variable using one of these: {HASWELL,SKYLAKE}"
#endif
	compress2_Element_2w_h0h5(XY,TZ);
}
/**
 * Full addition from Hisil page 6.
 * @param Q
 * @param P
 */
void _1way_mixadd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_precompute_2w_H0H5 *P)
{
	int i;
    argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P;
	argElement_2w XY = Q->XY;
	argElement_2w TZ = Q->TZ;
	argElement_2w subYXaddYX2 = P->subaddYX;
	argElement_2w _2dT_2Z = P->_2dT_2Z;

	Element_2w_Fp25519 _AB,CD,EG,HF;
	argElement_2w _subYXaddYX1 = EG;

	subadd_Element_2w_h0h5(_subYXaddYX1,XY,0); 			/* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
	mul_Element_2w_h0h5(_AB,_subYXaddYX1,subYXaddYX2);	/* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
	mul_Element_2w_h0h5(CD,TZ,_2dT_2Z); 				/* [C|D] = [ 2dT1T2 | 2Z1Z2 ] */
	/*[TODO: Precompute param_d to help to multiplication]*/

	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i _AC = _mm256_permute2x128_si256(_AB[i],CD[i],0x20);
		__m256i BD  = _mm256_permute2x128_si256(_AB[i],CD[i],0x31);
		EG[i] = ADD(BD,_AC);					/* [E|G] = [ B+(-A) | D+C ] */
		HF[i] = ADD(SUB(BD,_AC),_2_to_35P[i]);	/* [H|F] = [ B-(-A) | D-C ] */
	}
	compress2_Element_2w_h0h5(EG,HF);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
	mul2_Element_2w_h0h5(TZ,XY,HF,EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
	Element_2w_Fp25519 FH;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	mul_Element_2w_h0h5(XY,FH,EG);
	mul_Element_2w_h0h5(TZ,HF,EG);
#else
	#error "Define the PROCESSOR variable using one of these: {HASWELL,SKYLAKE}"
#endif
	compress2_Element_2w_h0h5(XY,TZ);
}

/**
 * Full addition from Hisil page 6.
 * @param Q
 * @param P
 */
void _1way_fulladd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_2w_H0H5 *P)
{
	int i;
	argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P;
	argElement_2w XY1 = Q->XY;
	argElement_2w TZ1 = Q->TZ;
	argElement_2w XY2 = P->XY;
	argElement_2w TZ2 = P->TZ;

	const uint64_t param_curve_2d_02[20] = {
			0x2b2f159,0x0f3d130,0x2,0x0,
			0x1a6e509,0x3407977,0x0,0x0,
			0x22add7a,0x19ce331,0x0,0x0,
			0x0d4141d,0x1c56dff,0x0,0x0,
			0x0038052,0x0901b67,0x0,0x0};

	Element_2w_Fp25519 AB,CD,EG,HF;
	argElement_2w subYXaddYX1 = EG;
	argElement_2w subYXaddYX2 = HF;

	subadd_Element_2w_h0h5(subYXaddYX1,XY1,0); 		 /* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
	subadd_Element_2w_h0h5(subYXaddYX2,XY2,1); 		 /* [ sub2|add2] = [  Y2-X2 | Y2+X2 ] */
	mul_Element_2w_h0h5(AB,subYXaddYX1,subYXaddYX2); /* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
	mul_Element_2w_h0h5(CD,TZ1,TZ2); 			     /* [C|D] = [ T1*T2 | Z1*Z2 ] */
	compress_Element_2w_h0h5(CD);
	mul_Element_2w_h0h5(CD,CD,(argElement_2w)param_curve_2d_02); /* [C|D] = [  2d*C | 2*D   ] */

	compressfast_Element_2w_h0h5(AB);
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i AC = _mm256_permute2x128_si256(AB[i],CD[i],0x20);
		__m256i BD = _mm256_permute2x128_si256(AB[i],CD[i],0x31);
		EG[i] = ADD(BD,AC);						/* [E|G] = [ B+(-A) | D+C ] */
		HF[i] = ADD(SUB(BD,AC),_2_to_35P[i]);	/* [H|F] = [ B-(-A) | D-C ] */
	}
	compress2_Element_2w_h0h5(EG,HF);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
	mul2_Element_2w_h0h5(TZ1,XY1,HF,EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
	Element_2w_Fp25519 FH;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	mul_Element_2w_h0h5(XY1,FH,EG);
	mul_Element_2w_h0h5(TZ1,HF,EG);
#else
	#error "Define the PROCESSOR variable using one of these: {HASWELL,SKYLAKE}"
#endif
	compress2_Element_2w_h0h5(XY1,TZ1);
}
/**
 *
 * @param P
 * @param index
 */
static void read_point_ed25519(PointXYZT_precompute_2w_H0H5 * P, int8_t index)
{
	const Element_1w_Fp25519 two = {0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	int8_t abs_index_r = index > 0 ? index: -index;
	abs_index_r >>= 1;

	const uint64_t * ptr_point = &TableVerif_static_w7_Ed25519[12*abs_index_r];
	Element_1w_Fp25519 add,sub,_2dT;

	str_bytes_To_Element_1w_h0h5( add,(uint8_t*)(ptr_point+0));
	str_bytes_To_Element_1w_h0h5( sub,(uint8_t*)(ptr_point+4));
	str_bytes_To_Element_1w_h0h5(_2dT,(uint8_t*)(ptr_point+8));

	if(index<0)/*negative*/
	{
		neg_Element_1w_h0h5(_2dT);
		compressfast_Element_1w_h0h5(_2dT);
		interleave_2w_h0h5(P->subaddYX,add,sub);
	}
	else
	{
		interleave_2w_h0h5(P->subaddYX,sub,add);
	}
	interleave_2w_h0h5(P->_2dT_2Z,_2dT,(argElement_1w)two);
}
/**
 *
 * @param sB_hA
 * @param s
 * @param h
 * @param A
 */
static void double_point_multiplication_ed25519(uint8_t *sB_hA, const uint8_t *s, uint8_t *h, PointXYZT_2w_H0H5 * A)
{
	int i;
	int8_t wnaf_r[256]={0};
	int8_t wnaf_h[256]={0};

	PointXYZT_2w_H0H5 Q;
	PointXYZT_precompute_2w_H0H5 tableA[1<<(OMEGA_DYNAMIC-2)];
	Element_1w_Fp25519 XX,YY,ZZ,TT;
	Element_1w_x64 X,Y,Z,invZ;

	int l_r = wnaf_256bits(wnaf_r,s, OMEGA_STATIC);
	int l_h = wnaf_256bits(wnaf_h,h, OMEGA_DYNAMIC);
	int l = l_r > l_h ? l_r : l_h;

	precompute_points_ed25519(tableA,A);

	for(i=1;i<(NUM_DIGITS_FP25519/2);i++)
	{
		Q.XY[i] = ZERO;
		Q.TZ[i] = ZERO;
	}
	Q.XY[0] = _mm256_set_epi64x(0,1,0,0);
	Q.TZ[0] = _mm256_set_epi64x(0,1,0,0);

	for(i=l-1;i>=0;i--)
	{
		_1way_doubling_2w_H0H5(&Q);
		/* known point addition */
		if(wnaf_r[i] != 0)
		{
			PointXYZT_precompute_2w_H0H5 P;
			read_point_ed25519(&P,wnaf_r[i]);
			_1way_mixadd_2w_H0H5(&Q, &P);
		}
		/* unknown point addition */
		if(wnaf_h[i] != 0)
		{
			PointXYZT_precompute_2w_H0H5 * P;
			uint8_t abs_index_h = wnaf_h[i]> 0 ? wnaf_h[i]: -wnaf_h[i];
			abs_index_h >>= 1;
			P = &tableA[abs_index_h];
			if(wnaf_h[i] < 0)
			{
				int ii=0;
				PointXYZT_precompute_2w_H0H5 _P;
				for(ii=0;ii<(NUM_DIGITS_FP25519/2);ii++)
				{
					_P.subaddYX[ii] = _mm256_permute4x64_epi64(P->subaddYX[ii], 0x4E);
				}
				negZZ_Element_2w_h0h5(_P._2dT_2Z,P->_2dT_2Z);
				compressfast_Element_2w_h0h5(_P._2dT_2Z);
				_1way_mixadd_2w_H0H5(&Q, &_P);
			}
			else
			{
				_1way_mixadd_2w_H0H5(&Q, P);
			}
		}
	}
	deinterleave_2w_h0h5(XX,YY,Q.XY);
	deinterleave_2w_h0h5(TT,ZZ,Q.TZ);

	singleH0H5_To_str_bytes((uint8_t*)X,XX);
	singleH0H5_To_str_bytes((uint8_t*)Y,YY);
	singleH0H5_To_str_bytes((uint8_t*)Z,ZZ);

	inv_Element_1w_x64(invZ,Z);
	mul_Element_1w_x64(X,X,invZ);
	mul_Element_1w_x64((uint64_t*)sB_hA,Y,invZ);
	fred_Element_1w_x64(X);
	fred_Element_1w_x64((uint64_t*)sB_hA);
	sB_hA[31] = (sB_hA[31]&0x7F) | ((X[0]&0x1)<<7);
}


