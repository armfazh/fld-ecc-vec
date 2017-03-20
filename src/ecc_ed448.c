#include "ecc_ed448.h"

#include "table_sign_w4_3675k_ed448.h"
#include "table_verf_w7_ed448.h"
#include <string.h>

/**
 * This function performs a modular reduction
 * (mod \ell) where:
 * 		order = 2^446 - ell
 * 		      = 2^246 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d
 * 		ell = 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d
 * this is a prime number which denotes the
 * size of the main subgroup of the elliptic curve.
 *
 * @param a
 */
static void modular_reduction_ed448(uint8_t *a)
{
	int i;
	const uint64_t ellx4[4] = {0x721cf5b5529eec34,0x7a4cf635c8e9c2ab,0xeec492d944a725bf,0x20cd77058};
	const uint64_t ell[4]   = {0xdc873d6d54a7bb0d,0xde933d8d723a70aa,0x3bb124b65129c96f,0x08335dc16};
	uint64_t *pA = (uint64_t *)a;

	ALIGN uint64_t product[15];
	ALIGN uint64_t high[8];

	high[7]=0;
	for(i=0;i<58;i++)
	{
		((uint8_t *)high)[i] = a[i+56];
	}
	for(i=0;i<7;i++)
	{
		product[i] = pA[i];
	}
	for(i=7;i<13;i++)
	{
		product[i] = 0;
	}

	word64_multiplier(product,ellx4,4,high,8);
	for(i=0;i<58;i++)
	{
		((uint8_t *)high)[i] = ((uint8_t*)product)[i+56];
		((uint8_t*)product)[i+56]=0;
	}
	word64_multiplier(product,ellx4,4,high,4);
	for(i=0;i<58;i++)
	{
		((uint8_t *)high)[i] = ((uint8_t*)product)[i+56];
		((uint8_t*)product)[i+56]=0;
	}
	word64_multiplier(product,ellx4,4,high,1);

	high[0] = product[6]>>62;
	product[6] &= (((uint64_t)1)<<62)-1;
	word64_multiplier(product,ell,4,high,1);
	for(i=0;i<56;i++)
	{
		a[i] = ((uint8_t*)product)[i];
	}
	for(i=56;i<114;i++)
	{
		a[i]=0;
	}
}

/**
 * Computes:
 *  number <-- number/4 mod curve.order
 *
 * @param number_div_4
 * @param number
 */
static void div4(uint8_t * number_div_4,const uint8_t * number)
{
	const uint64_t ifour[7] = {
			0x48de30a4aad6113d,	0x085b309ca37163d5,	0x7113b6d26bb58da4, 0xffffffffdf3288fa,
			0xffffffffffffffff,	0xffffffffffffffff,	0x0fffffffffffffff
	};
	uint64_t a[7];

	int i;
	uint64_t product[16];
	for(i=0;i<16;i++)
	{
		product[i] = 0;
	}
	for(i=0;i<56;i++)
	{
		((uint8_t*)a)[i] = number[i];
	}
	word64_multiplier(product,ifour,7,a,7);
	modular_reduction_ed448((uint8_t*)product);
	for(i=0;i<56;i++)
	{
		number_div_4[i] = ((uint8_t*)product)[i];
	}
	number_div_4[ED448_KEY_SIZE_BYTES_PARAM-1] = 0;
}

static void point_encoding_ed448(uint8_t*enc, PointXYZT_2w_H0H8* P)
{
	/* convert to affine coordinates */
	Ed448_PublicKey bufX;
	Element_1w_Fp448 Z,X,Y,invZ;

	deinterleave_2w_h0h8(X,Y,P->XY);
	deinterleave_2w_h0h8(invZ,Z,P->TZ);
	invsqrt_Element_1w_h0h8(invZ,Z,1);
	mul_Element_1w_h0h8(X,X,invZ);compress_Element_1w_h0h8(X);
	mul_Element_1w_h0h8(Y,Y,invZ);compress_Element_1w_h0h8(Y);

	/* encoding */
	singleH0H8_To_str_bytes(bufX,X);
	singleH0H8_To_str_bytes(enc,Y);
	enc[ED448_KEY_SIZE_BYTES_PARAM-1] = (bufX[0]&0x1)<<7;
}

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
void recoding_signed_scalar_fold4w4_448(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r)
{
	const int OMEGA = 4;
	int i,j;
	int8_t carry=0;
	uint8_t nibble_low, nibble_hig;
	int8_t value,digit;

	for(i=0;i<2;i++)
	{
		for(j=0;j<28;j++)
		{
			nibble_low = r[28*i+j] & 0xF;
			value = nibble_low + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[56*i+2*j+0] = (int64_t) digit;
			list_signs [56*i+2*j+0] = (int64_t) -carry;

			nibble_hig = (r[28*i+j] >> 4) & 0xF;
			value = nibble_hig + carry;
			carry = value >= (1 << (OMEGA - 1));
			digit = ((value^(-carry))+carry)&0xF;

			list_digits[56*i+2*j+1] = (int64_t) digit;
			list_signs [56*i+2*j+1] = (int64_t) -carry;
		}
	}
//	printf("Carry: %d\n",carry);
	list_digits[112] = carry; /* This is always equal to 0 iff r < 2**447 */
}

/**
 *
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
static void query_table_fold4w4_ed448(Point_precmp_4way_Fp448 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
	const __m256i _P[16] = {
			SET1_64(0xfffffff),	SET1_64(0xffffffe),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff),
			SET1_64(0xfffffff),	SET1_64(0xfffffff)
	};
	const __m256i one = SET1_64(1);
	int i,j;
	__m256i mask[9];
	__m256i P_addYX[7], P_subYX[7], P__2dYX[7];
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
		mask[i] = SHUF(_mm256_srai_epi32(mask[i],32),0xE5);
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
	for(j=0;j<7;j++)/* num of 64-bit words */
	{
		P_addYX[j] = ZERO;
		P_subYX[j] = ZERO;
		P__2dYX[j] = ZERO;
		for(i=0;i<8;i++)/* num of multiples {1B,2B,3B,...,8B} */
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
	for(j=0;j<7;j++)
	{
		__m256i swap = AND(XOR(P_addYX[j], P_subYX[j]), signs);
		P_addYX[j] = XOR(P_addYX[j], swap);
		P_subYX[j] = XOR(P_subYX[j], swap);
	}

	str_bytes_to_Element_4w_h0h8(P->addYX,P_addYX);
	str_bytes_to_Element_4w_h0h8(P->subYX,P_subYX);
	str_bytes_to_Element_4w_h0h8(P->_2dYX,P__2dYX);

	/**
	 * Point Sign Verification (Part 2/2)
	 * if signs == 111111111
	 *    _2dYX <-- -_2dYX + P
	 * otherwise
	 *    _2dYX <--  _2dYX + P
	 */
	for(i=0;i<NUM_DIGITS_FP448;i++)
	{
		P->_2dYX[i] = ADD( XOR(P->_2dYX[i],signs), SUB(_P[i],signs));
	}
}

/**
 *
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
void query_table_ed448(Point_precmp_4way_Fp448 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
	query_table_fold4w4_ed448(P,table,secret_signs,secret_digits);
}

/**
 * This version is intended to compute four independent addition points
 * on four pair of points.
 * Taken from Hisil, Wong,Carter,Dawson
 * Section 3.1 page 6
 *
 * @param Q
 * @param P
 */
void _4way_mixadd_448(PointXYZT_4way_Fp448 *Q, Point_precmp_4way_Fp448 *P)
{
	__m256i * const X1 = Q->X;
	__m256i * const Y1 = Q->Y;
	__m256i * const T1 = Q->T;
	__m256i * const Z1 = Q->Z;
	__m256i * const addY2X2 = P->addYX;
	__m256i * const subY2X2 = P->subYX;
	__m256i * const dT2 = P->_2dYX;
	Element_4w_Fp448 A,B,C,D;
	argElement_4w addY1X1 = C;
	argElement_4w subY1X1 = D;

	sub_Element_4w_h0h8(subY1X1,Y1,X1);
	mul_Element_4w_h0h8(A,subY1X1,subY2X2);
	add_Element_4w_h0h8(addY1X1,Y1,X1);
	mul_Element_4w_h0h8(B,addY1X1,addY2X2);

	mul_Element_4w_h0h8(C,dT2,T1);
	add_Element_4w_h0h8(D,Z1,Z1);
	addsub_large_Element_4w_h0h8(B,A);
	addsub_large_Element_4w_h0h8(D,C);

	compress2_Element_4w_h0h8(B,A);
	compress2_Element_4w_h0h8(D,C);

	mul_Element_4w_h0h8(X1,A,C);
	mul_Element_4w_h0h8(Y1,D,B);
	mul_Element_4w_h0h8(Z1,D,C);
	mul_Element_4w_h0h8(T1,A,B);

	compress2_Element_4w_h0h8(X1,Y1);
	compress2_Element_4w_h0h8(Z1,T1);
}

/**
 *
 * @param R0
 * @param Q
 */
static void join_points_2w_H0H8(PointXYZT_2w_H0H8 *R0, PointXYZT_4way_Fp448 *Q)
{
	PointXYZT_1way_Fp448 Q1,Q2,Q3,Q0;
	PointXYZT_2w_H0H8 R1,R2,R3;

    unzip_Element_4w_h0h8(Q0.X,Q1.X,Q2.X,Q3.X,Q->X);
    unzip_Element_4w_h0h8(Q0.Y,Q1.Y,Q2.Y,Q3.Y,Q->Y);
    unzip_Element_4w_h0h8(Q0.Z,Q1.Z,Q2.Z,Q3.Z,Q->Z);
    unzip_Element_4w_h0h8(Q0.T,Q1.T,Q2.T,Q3.T,Q->T);

    interleave_2w_h0h8(R0->XY,Q0.X,Q0.Y);   interleave_2w_h0h8(R0->TZ,Q0.T,Q0.Z);
    interleave_2w_h0h8( R1.XY,Q1.X,Q1.Y);   interleave_2w_h0h8( R1.TZ,Q1.T,Q1.Z);
    interleave_2w_h0h8( R2.XY,Q2.X,Q2.Y);   interleave_2w_h0h8( R2.TZ,Q2.T,Q2.Z);
    interleave_2w_h0h8( R3.XY,Q3.X,Q3.Y);   interleave_2w_h0h8( R3.TZ,Q3.T,Q3.Z);

    _1way_doubling_2w_H0H8(&R3);    _1way_doubling_2w_H0H8(&R3);
    _1way_doubling_2w_H0H8(&R3);    _1way_doubling_2w_H0H8(&R3);
	_1way_fulladd_2w_H0H8(&R2,&R3);
    _1way_doubling_2w_H0H8(&R2);    _1way_doubling_2w_H0H8(&R2);
    _1way_doubling_2w_H0H8(&R2);    _1way_doubling_2w_H0H8(&R2);
	_1way_fulladd_2w_H0H8(&R1,&R2);
    _1way_doubling_2w_H0H8(&R1);    _1way_doubling_2w_H0H8(&R1);
    _1way_doubling_2w_H0H8(&R1);    _1way_doubling_2w_H0H8(&R1);
	_1way_fulladd_2w_H0H8(R0,&R1);
}

static const uint64_t CONST_2_to_35P_ED448[32] = {
		0x7ffffff800000000, 0x7ffffff000000000, 0x7ffffff800000000, 0x7ffffff000000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000,
		0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000, 0x7ffffff800000000
};

/**
 * Inverse isogeny_1w_H0H8 defined in "Ted with isogenies."
 * (Hx,Hy) = \phi_a(Qx,Qy,Qz,Qt)
 * such that:
 * a  = -1
 * Hx = (2*Qx*Qy)/(Qy**2-a*Qx**2)
 * Hy = (Qy**2+a*Qx**2)/(2*Qz**2-Qy**2-a*Qx**2)
 *
 * @param P
 */
static void isogeny_2w_H0H8(PointXYZT_2w_H0H8 *P)
{
	int i;
	argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P_ED448;
	argElement_2w XY = P->XY;
	argElement_2w TZ = P->TZ;
	Element_2w_Fp448 HF,EC,EG,_FH;

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		EC[i] = PERM64(TZ[i],0xEE);
	}
	sqr_Element_2w_h0h8(XY);            /* [A|B] = [X^2 | Y^2 ] */
	mul_Element_2w_h0h8(TZ,TZ,EC);		/* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
	add_Element_2w_h0h8(EC,TZ,TZ);		/* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		__m256i CB,BA,AA,_H,_FG,_B;
		BA = PERM64(XY[i], 0x4E);			/* [  B  |  A ] */
		AA = PERM64(XY[i], 0x44);			/* [  A  |  A ] */
		CB = PERM128(EC[i],XY[i],0x31); 	/* [  C  |  B ] */
		_H = ADD(CB,AA);                	/* [ C+A |  H ] = [ C+A   | B+A ] */
		_B = BLEND32(XY[i],_H,0x0F);		/* [ C+A |  B ] */
		_FG = SUB(BA,_B);            		/* [ -F  | -G ] = [ B-C-A | B-A ] */
		_FG = ADD(_FG,_2_to_35P[i]);
		_FH[i] = BLEND32(_H,_FG,0x0F);		/* [ -F  |  H ] */
		EG[i]  = BLEND32(_FG,EC[i],0x0F);	/* [  E  |  G ]  */
	}
	compress2_Element_2w_h0h8(_FH,EG);

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		HF[i] = PERM64(_FH[i],0x4E);
	}
	mul_Element_2w_h0h8(XY,_FH,EG);
	mul_Element_2w_h0h8(TZ,HF,_FH);
	compress2_Element_2w_h0h8(XY,TZ);
}

/**
 * This function will use a pre-computed table of 36.75KB.
 * Folding 4 means four queries at the same time.
 * Using w=4, means that scalar will be partitioned in
 * 4-bit signed digits.
 * This produces to compute 28 iterations in the main loop.
 * as follows:
 *		S_0 = r_0 + 16^4 r_4 + 16^8 r_8  + ... + 16^108 r_108
 *		S_1 = r_1 + 16^4 r_5 + 16^8 r_9  + ... + 16^108 r_109
 *		S_2 = r_2 + 16^4 r_6 + 16^8 r_10 + ... + 16^108 r_110
 *		S_3 = r_3 + 16^4 r_7 + 16^8 r_11 + ... + 16^108 r_111
 * Then recombining partial sums with:
 *		S = S_0 + 16^1 S_1 + 16^2 S_2 + 16^3 S_3.
 *
 * @param rB
 * @param r
 */
static void point_multiplication_fold4w4(PointXYZT_2w_H0H8 *rB, uint8_t *r)
{
	unsigned int i;
	PointXYZT_4way_Fp448 Q;
	Point_precmp_4way_Fp448 P;
	ALIGN uint64_t K[112+3];
	ALIGN uint64_t S[112+3];
	const Element_4w_Fp448 one_half = {
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x0000000),SET1_64(0xfffffff),
			SET1_64(0x8000000),SET1_64(0x7ffffff)
	};
 	recoding_signed_scalar_fold4w4_448(S,K,r);

	Q.Z[0] = SET1_64(2);
	for(i=1;i<NUM_DIGITS_FP448;i++)
	{
		Q.Z[i] = ZERO;
	}

	query_table_fold4w4_ed448(&P, ((uint8_t*)TableSign_w4_3675k),S,K);
	sub_Element_4w_h0h8(Q.X,P.addYX,P.subYX);
	add_Element_4w_h0h8(Q.Y,P.addYX,P.subYX);
	compress_Element_4w_h0h8(Q.X);
	compress_Element_4w_h0h8(Q.Y);
	mul_Element_4w_h0h8(Q.T,Q.X,Q.Y);						compress_Element_4w_h0h8(Q.T);
	mul_Element_4w_h0h8(Q.T,Q.T,(argElement_4w)one_half);	compress_Element_4w_h0h8(Q.T);

	for(i=1;i<NUM_LUT_ED448;i++)
	{
        _mm_prefetch( (const char *)  (((uint8_t*)TableSign_w4_3675k)+768*(i+1) ),_MM_HINT_T0);
		query_table_fold4w4_ed448(&P, ((uint8_t*)TableSign_w4_3675k)+SIZE_ONE_LUT_ED448*i,S+4*i,K+4*i);
		_4way_mixadd_448(&Q, &P);
	}
	join_points_2w_H0H8(rB, &Q);

	/* inverse isogeny */
	isogeny_2w_H0H8(rB);
}

/**
 * Given:
 *      STR_BYTES r
 * Computes
 *      rB = [r]B
 * Ensuring that B is the generator of Ed25519.
 *
 * @param rB
 * @param r
 */
static void fixed_point_multiplication_ed448(PointXYZT_2w_H0H8 *rB, uint8_t *r)
{
	/**
	 * Compute a division by 4 because the point multiplication
	 * is processed using the twisted Edwards curve, which is
	 * the isogeny_1w_H0H8 of the Edwards curve.
	 **/
	Ed448_PublicKey r_div_4;
	div4(r_div_4,r);
//	printf("div4> ");print_bytes(r_div_4,ED448_KEY_SIZE_BYTES_PARAM);
	point_multiplication_fold4w4(rB,r_div_4);
}

/**
 * Given a compressed point xy,
 * this function returns:
 * 		P = (-x,y)
 * @param P
 * @param A
 * @return
 */
static int point_decoding_ed448(PointXYZT_2w_H0H8 * P, const uint8_t * A)
{
	/**
	 * Using the d parameter using the
	 * goldilocks curve.
	 *     d = -39081
	 */
	const Element_1w_Fp448 param_curve_d = {
			0xfff6756,0xffffffe,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff,
			0xfffffff,0xfffffff
	};
	Element_1w_Fp448 X,Y,Z,T;
	Element_1w_Fp448 a,b,c,d;
	Element_1w_Fp448 u,v,v2;
	Element_1w_Fp448 uv,u2,u3v,u5v,u5v3,u5v3_exp;
	Ed448_PublicKey buf;

	memcpy(buf,A,ED448_KEY_SIZE_BYTES_PARAM);
	int8_t bit_X = buf[ED448_KEY_SIZE_BYTES_PARAM-1]>>7;

	buf[ED448_KEY_SIZE_BYTES_PARAM-1] &= 0x7F;
	str_bytes_To_Element_1w_h0h8(Y,buf);
	buf[ED448_KEY_SIZE_BYTES_PARAM-1] |= bit_X<<7;

	copy_Element_1w_h0h8(u,Y);
	sqr_Element_1w_h0h8(u); /* y^2 */
	mul_Element_1w_h0h8(v,u,(argElement_1w)param_curve_d);	compress_Element_1w_h0h8(v);/* dy^2 */
	u[0] = u[0]-1;/* u=y^2-1 */
	v[0] = v[0]-1;/* v=dy^2-1 */

	mul_Element_1w_h0h8(uv,u,v);		compress_Element_1w_h0h8(uv);/* uv */
	copy_Element_1w_h0h8(u2,u);
	copy_Element_1w_h0h8(v2,v);
	sqr_Element_1w_h0h8(u2);			/* u^2 */
	sqr_Element_1w_h0h8(v2);			/* v^2 */
	mul_Element_1w_h0h8(u3v,uv,u2);		compress_Element_1w_h0h8(u3v); /* u^3v */
	mul_Element_1w_h0h8(u5v,u3v,u2);	compress_Element_1w_h0h8(u5v); /* u^5v */
	mul_Element_1w_h0h8(u5v3,u5v,v2);	compress_Element_1w_h0h8(u5v3);/* u^5v^3 */

	invsqrt_Element_1w_h0h8(u5v3_exp,u5v3,0);
	mul_Element_1w_h0h8(X,u3v,u5v3_exp);    compress_Element_1w_h0h8(X);

	/**
	 * Checking whether
	 *      v*x^2 == u
	 **/
	copy_Element_1w_h0h8(uv,X);
	sqr_Element_1w_h0h8(uv);   	/*   x^2 */
	mul_Element_1w_h0h8(uv,uv,v);	compress_Element_1w_h0h8(uv); /* v*x^2 */

	if(compare_Element_1w_h0h8(uv,u) != 0)
	{
		/* no square root exists */
		return -1;
	}

	/**
	 * Checking whether
	 *      X is positive or negative
	 **/
	if((unsigned)bit_X != (X[0]&0x1))
	{
		neg_Element_1w_h0h8(X);
		compressfast_Element_1w_h0h8(X);
	}

	/**
	 * Convert point P to \phi(P)
	 * according to the isogeny_1w_H0H8
	 * AND
	 * Point represented in extended coordinates
	 ***/
	add_Element_1w_h0h8(a,X,X);
	mul_Element_1w_h0h8(a,a,Y);compress_Element_1w_h0h8(a);

	/* Variable 'a' must be negative to produce -A = (-X,Y,Z,-T) */
	neg_Element_1w_h0h8(a);
	compressfast_Element_1w_h0h8(a);
	sqr_Element_1w_h0h8(X);
	sqr_Element_1w_h0h8(Y);
	add_Element_1w_h0h8(b,Y,X);new_compressfast_Element_1w_h0h8(b);
	sub_Element_1w_h0h8(c,Y,X);new_compressfast_Element_1w_h0h8(c);
	copy_Element_1w_h0h8(d,b);
	neg_Element_1w_h0h8(d);
	d[0] += 2;
	compressfast_Element_1w_h0h8(d);

	mul_Element_1w_h0h8(X,a,d);compress_Element_1w_h0h8(X);
	mul_Element_1w_h0h8(Y,b,c);compress_Element_1w_h0h8(Y);
	mul_Element_1w_h0h8(Z,c,d);compress_Element_1w_h0h8(Z);
	mul_Element_1w_h0h8(T,a,b);compress_Element_1w_h0h8(T);

	interleave_2w_h0h8(P->XY,X,Y);
	interleave_2w_h0h8(P->TZ,T,Z);
	return 0;
}

#define is_greater_equal_one_512(r)  \
(       (r[7]!=0)                \
	||	(r[6]!=0)                \
	||	(r[5]!=0)                \
	||	(r[4]!=0)                \
	||	(r[3]!=0)                \
	||	(r[2]!=0)                \
	||	(r[1]!=0)                \
	||	(r[0]>=(uint64_t)1)      \
)
#define is_odd(r) ((r[0]&0x1)==1)
#define div_2_512(r) \
{\
    uint64_t bit7 = r[7]<<63;\
    uint64_t bit6 = r[6]<<63;\
    uint64_t bit5 = r[5]<<63;\
    uint64_t bit4 = r[4]<<63;\
    uint64_t bit3 = r[3]<<63;\
    uint64_t bit2 = r[2]<<63;\
    uint64_t bit1 = r[1]<<63;\
    r[7] = (r[7]>>1);\
    r[6] = (r[6]>>1) | bit7;\
    r[5] = (r[5]>>1) | bit6;\
    r[4] = (r[4]>>1) | bit5;\
    r[3] = (r[3]>>1) | bit4;\
    r[2] = (r[2]>>1) | bit3;\
    r[1] = (r[1]>>1) | bit2;\
    r[0] = (r[0]>>1) | bit1;\
}
#define SUB512(b,sign,value)        \
	__asm__ __volatile__ (          \
		" movq   0(%0), %%rax \n\t subq	%2, %%rax \n\t movq  %%rax,  0(%0) \n\t"   \
		" movq   8(%0), %%rcx \n\t sbbq	%1, %%rcx \n\t movq  %%rcx,  8(%0) \n\t"   \
		" movq  16(%0), %%rdx \n\t sbbq	%1, %%rdx \n\t movq  %%rdx, 16(%0) \n\t"   \
		" movq  24(%0),  %%r8 \n\t sbbq	%1,  %%r8 \n\t movq   %%r8, 24(%0) \n\t"   \
		" movq  32(%0),  %%r9 \n\t sbbq	%1,  %%r9 \n\t movq   %%r9, 32(%0) \n\t"   \
		" movq  40(%0), %%rax \n\t sbbq	%1, %%rax \n\t movq  %%rax, 40(%0) \n\t"   \
		" movq  48(%0), %%rcx \n\t sbbq	%1, %%rcx \n\t movq  %%rcx, 48(%0) \n\t"   \
		" movq  56(%0), %%rdx \n\t sbbq	%1, %%rdx \n\t movq  %%rdx, 56(%0) \n\t"   \
		:                                                                          \
		: "r"(b), "r"(sign), "r"(value)                                            \
        : "cc", "memory", "%rax", "%rcx", "%rdx", "%r8", "%r9")
/**
 *
 * @param K
 * @param p8_r
 * @param w
 * @return
 */
int wnaf_448bits(int8_t *K, const uint8_t *p8_r, int w)
{
	int i = 0;
	int64_t value;
	ALIGN uint64_t r[8] = {0};

	r[7] = 0;
	for(i=0;i<57;i++)
	{
		((uint8_t*)r)[i] = p8_r[i];
	}

	i=0;
	while(is_greater_equal_one_512(r))
	{
		if(is_odd(r))
		{
			value = r[0] & (((uint64_t)1<<w)-1);
			if(value>(1<<(w-1)))
			{
				value = value - (1<<w);
			}
			int64_t sign=value>>63;
			SUB512(r,sign,value);
		}
		else
		{
			value = 0;
		}
		K[i] = (int8_t)(value&0xFF);
		div_2_512(r);
		i++;
	}
	return i;
}

/**
 * Using the d parameter from the isogeny
 *  d = -39082
 */
static const uint64_t CONST_param_curve_2d_02 [32] = {
		0xffeceab,0xffffffe,0x2,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0,
		0xfffffff,0xfffffff,0x0,0x0
};

/**
 * Computes the doubling of ONE point
 * stored in P = {XY,ZT}
 *
 * @param P
 */
void _1way_doubling_2w_H0H8(PointXYZT_2w_H0H8 * P)
{
	int i;
    argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P_ED448;
	argElement_2w XY = P->XY;
	argElement_2w TZ = P->TZ;
    Element_2w_Fp448 EG,HF,FH;

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		EG[i] = PERM64(TZ[i],0xEE);
	}
	sqr_Element_2w_h0h8(XY);            /* [A|B] = [X^2 | Y^2 ] */
	mul_Element_2w_h0h8(TZ,TZ,EG);		/* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
	add_Element_2w_h0h8(TZ,TZ,TZ);		/* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
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
	compress2_Element_2w_h0h8(HF,EG);

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	mul_Element_2w_h0h8(XY,FH,EG);
	mul_Element_2w_h0h8(TZ,HF,EG);
	compress2_Element_2w_h0h8(XY,TZ);
}

/**
 * 
 * @param Q 
 * @param P 
 */
void _1way_fulladd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_2w_H0H8 *P)
{
    int i;
	argElement_2w param_curve_2d_02 = (argElement_2w)CONST_param_curve_2d_02;
    argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P_ED448;
    argElement_2w XY1 = Q->XY;		argElement_2w XY2 = P->XY;
    argElement_2w TZ1 = Q->TZ;		argElement_2w TZ2 = P->TZ;
	Element_2w_Fp448 _AB,CD,EG,HF;
	Element_2w_Fp448 FH;
	argElement_4w _subYXaddYX1 = EG;
	argElement_4w _subYXaddYX2 = HF;

    subadd_Element_2w_h0h8(_subYXaddYX1,XY1,0); 			/* [-sub1|add1] = [ -(Y1-X1) | Y1+X1 ] */
    subadd_Element_2w_h0h8(_subYXaddYX2,XY2,1); 			/* [ sub2|add2] = [   Y2-X2  | Y2+X2 ] */
	compressfast_Element_2w_h0h8(_subYXaddYX1);
	compressfast_Element_2w_h0h8(_subYXaddYX2);
    mul_Element_2w_h0h8(_AB,_subYXaddYX1,_subYXaddYX2);		/* [-A|B] = [ -(Y1-X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
    mul_Element_2w_h0h8(CD,TZ1,TZ2); 						/* [C|D] = [ T1T2 | Z1Z2 ] */
	compress_Element_2w_h0h8(CD);
	mul_Element_2w_h0h8(CD,CD,param_curve_2d_02);			/* [C|D] = [ 2dT1T2 | 2Z1Z2 ] */

    for(i=0;i<(NUM_DIGITS_FP448/2);i++)
    {
        __m256i _AC = _mm256_permute2x128_si256(_AB[i],CD[i],0x20);
        __m256i BD  = _mm256_permute2x128_si256(_AB[i],CD[i],0x31);
        EG[i] = ADD(BD,_AC);							/* [E|G] = [ B+(-A) | D+C ] */
        HF[i] = ADD(SUB(BD,_AC),_2_to_35P[i]);			/* [H|F] = [ B-(-A) | D-C ] */
    }
    compress2_Element_2w_h0h8(EG,HF);

    for(i=0;i<(NUM_DIGITS_FP448/2);i++)
    {
        FH[i] = PERM64(HF[i],0x4E);
    }
    mul_Element_2w_h0h8(XY1,FH,EG);
    mul_Element_2w_h0h8(TZ1,HF,EG);
    compress2_Element_2w_h0h8(XY1,TZ1);
}

/**
 * Full addition from Hisil page 6.
 * 
 * @param Q 
 * @param P 
 */
void _1way_mixadd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_precompute_2w_H0H8 *P)
{
	int i;
    argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P_ED448;
	argElement_2w XY = Q->XY;
	argElement_2w TZ = Q->TZ;
	argElement_2w subYXaddYX2 = P->subaddYX;
	argElement_2w _2dT_2Z = P->_2dT_2Z;
	Element_2w_Fp448 _AB,CD,EG,HF,FH;
	argElement_4w _subYXaddYX1 = EG;

	subadd_Element_2w_h0h8(_subYXaddYX1,XY,0); 			/* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
	mul_Element_2w_h0h8(_AB,_subYXaddYX1,subYXaddYX2);	/* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
	mul_Element_2w_h0h8(CD,TZ,_2dT_2Z); 				/* [C|D] = [ 2dT1T2 | 2Z1Z2 ] */

	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		__m256i _AC = _mm256_permute2x128_si256(_AB[i],CD[i],0x20);
		__m256i BD  = _mm256_permute2x128_si256(_AB[i],CD[i],0x31);
		EG[i] = ADD(BD,_AC);							/* [E|G] = [ B-(-A) | D+C ] */
		HF[i] = ADD(SUB(BD,_AC),_2_to_35P[i]);			/* [H|F] = [ B+(-A) | D-C ] */
	}
	compress2_Element_2w_h0h8(EG,HF);
	
	for(i=0;i<(NUM_DIGITS_FP448/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	mul_Element_2w_h0h8(XY,FH,EG);
	mul_Element_2w_h0h8(TZ,HF,EG);
	compress2_Element_2w_h0h8(XY,TZ);
}

/**
 * 
 * @param table 
 * @param P 
 */
static void precompute_points_ed448(PointXYZT_precompute_2w_H0H8 * table, PointXYZT_2w_H0H8* P)
{
	const int num = (1<<(OMEGA_DYNAMIC-2));
	int i=0;

	argElement_2w param_curve_2d_02 = (argElement_2w)CONST_param_curve_2d_02;
	PointXYZT_2w_H0H8 iP;
	PointXYZT_precompute_2w_H0H8 _2P_precmp;

	copy_Element_2w_h0h8(iP.XY,P->XY);
	copy_Element_2w_h0h8(iP.TZ,P->TZ);
	_1way_doubling_2w_H0H8(P);

	subadd_Element_2w_h0h8(_2P_precmp.subaddYX,P->XY,1);
	mul_Element_2w_h0h8(_2P_precmp._2dT_2Z,P->TZ,(argElement_2w)param_curve_2d_02);
	compress2_Element_2w_h0h8(_2P_precmp.subaddYX,_2P_precmp._2dT_2Z);

	subadd_Element_2w_h0h8(table[0].subaddYX,iP.XY,1);
	mul_Element_2w_h0h8(table[0]._2dT_2Z,iP.TZ,(argElement_2w)param_curve_2d_02);
	compress2_Element_2w_h0h8(table[0].subaddYX,table[0]._2dT_2Z);

	for(i=1;i<num;i++)
	{
		_1way_mixadd_2w_H0H8(&iP,&_2P_precmp);
		subadd_Element_2w_h0h8(table[i].subaddYX,iP.XY,1);
		mul_Element_2w_h0h8(table[i]._2dT_2Z,iP.TZ,(argElement_2w)param_curve_2d_02);
		compress2_Element_2w_h0h8(table[i].subaddYX,table[i]._2dT_2Z);
	}
}

/**
 * 
 * @param P 
 * @param index 
 */
static void read_point(PointXYZT_precompute_2w_H0H8 * P, int8_t index)
{
	const Element_1w_Fp448 two = {
			0x2,0x0,0x0,0x0,
			0x0,0x0,0x0,0x0,
			0x0,0x0,0x0,0x0,
			0x0,0x0,0x0,0x0};
	uint8_t abs_index_r = index > 0 ? index: -index;
	abs_index_r >>= 1;

	const uint64_t * ptr_point = &TableVerif_static_w7_Ed448[3*7*abs_index_r];
	Element_1w_Fp448 add,sub,_2dT;

	str_bytes_To_Element_1w_h0h8( add,(uint8_t*)(ptr_point+0));
	str_bytes_To_Element_1w_h0h8( sub,(uint8_t*)(ptr_point+7));
	str_bytes_To_Element_1w_h0h8(_2dT,(uint8_t*)(ptr_point+14));

	if(index<0)/*negative*/
	{
		neg_Element_1w_h0h8(_2dT);
		compress_Element_1w_h0h8(_2dT);
		interleave_2w_h0h8(P->subaddYX,add,sub);
	}
	else
	{
		interleave_2w_h0h8(P->subaddYX,sub,add);
	}
	interleave_2w_h0h8(P->_2dT_2Z,_2dT,(argElement_1w)two);
}

/**
 * 
 * @param sB_hA 
 * @param s 
 * @param h 
 * @param A 
 */
static void double_point_multiplication_ed448(uint8_t *sB_hA, const uint8_t *s, uint8_t *h, PointXYZT_2w_H0H8 * A)
{
	int i;
	int8_t wnaf_r[460]={0};
	int8_t wnaf_h[460]={0};
	ALIGN uint8_t s_div_4[ED448_KEY_SIZE_BYTES_PARAM];
	ALIGN uint8_t h_div_4[ED448_KEY_SIZE_BYTES_PARAM];

	PointXYZT_2w_H0H8 Q;
	PointXYZT_precompute_2w_H0H8 tableA[1<<(OMEGA_DYNAMIC-2)];

	/**
	 * To use Isogeny
	 * compute h' = h/4 mod q
	 * compute s' = s/4 mod q
	 **/
	div4(s_div_4,s);
	div4(h_div_4,h);

	int l_r = wnaf_448bits(wnaf_r,s_div_4, OMEGA_STATIC);
	int l_h = wnaf_448bits(wnaf_h,h_div_4, OMEGA_DYNAMIC);
	int l = l_r > l_h ? l_r : l_h;

	precompute_points_ed448(tableA,A);

	for(i=1;i<(NUM_DIGITS_FP448/2);i++)
	{
		Q.XY[i] = ZERO;
		Q.TZ[i] = ZERO;
	}
	Q.XY[0] = _mm256_set_epi64x(0,1,0,0);
	Q.TZ[0] = _mm256_set_epi64x(0,1,0,0);
	for(i=l-1;i>=0;i--)
	{
		_1way_doubling_2w_H0H8(&Q);
		/* known point addition */
		if(wnaf_r[i] != 0)
		{
			PointXYZT_precompute_2w_H0H8 P;
			read_point(&P,wnaf_r[i]);
			_1way_mixadd_2w_H0H8(&Q,&P);
		}

		/* unknown point addition */
		if(wnaf_h[i] != 0)
		{
			uint8_t abs_index_h = wnaf_h[i]> 0 ? wnaf_h[i]: -wnaf_h[i];
			abs_index_h >>= 1;
			PointXYZT_precompute_2w_H0H8 * P = &tableA[abs_index_h];
			if(wnaf_h[i] < 0)
			{
				int ii=0;
				PointXYZT_precompute_2w_H0H8 _P;
				for(ii=0;ii<(NUM_DIGITS_FP448/2);ii++)
				{
					_P.subaddYX[ii] = _mm256_permute4x64_epi64(P->subaddYX[ii], 0x4E);
				}
				negZZ_Element_2w_h0h8(_P._2dT_2Z,P->_2dT_2Z);
				compressfast_Element_2w_h0h8(_P._2dT_2Z);
				_1way_mixadd_2w_H0H8(&Q,&_P);
			}
			else
			{
				_1way_mixadd_2w_H0H8(&Q,P);
			}
		}
	}

	/* inverse isogeny */
	isogeny_2w_H0H8(&Q);

	/* convert to affine coordinates */
	Element_1w_Fp448 invZ,Z,X,Y;
	deinterleave_2w_h0h8(X,Y,Q.XY);
	deinterleave_2w_h0h8(invZ,Z,Q.TZ);
	invsqrt_Element_1w_h0h8(invZ,Z,1);
	mul_Element_1w_h0h8(X,X,invZ);compress_Element_1w_h0h8(X);
	mul_Element_1w_h0h8(Y,Y,invZ);compress_Element_1w_h0h8(Y);

	/* encoding */
	Ed448_PublicKey bufX;
	singleH0H8_To_str_bytes(bufX,X);
	singleH0H8_To_str_bytes(sB_hA,Y);
	sB_hA[ED448_KEY_SIZE_BYTES_PARAM-1] = (bufX[0]&0x1)<<7;
}
