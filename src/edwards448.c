/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined(ED448)

#include "edwards448.h"
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
static inline void modular_reduction_ed448(uint8_t *a) {
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
static inline void div4(uint8_t * number_div_4,const uint8_t * number) {
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

/**
 *
 * @param encP
 * @param P
 */
static inline void point_encoding_ed448(uint8_t*encP, PointXYZT_2w_H0H8* P) {
  EltFp448_1w_redradix Z, X, Y, invZ;
  ALIGN uint8_t encX[SIZE_FP448];

  Fp448._2w_red.arithex.deinter(X, Y, P->XY);
  Fp448._2w_red.arithex.deinter(invZ, Z, P->TZ);
  Fp448._1w_red.arith.inv(invZ, Z);
  Fp448._1w_red.arith.mul(X, X, invZ);
  Fp448._1w_red.arith.mul(Y, Y, invZ);
  Fp448._1w_red.arith.misc.ser(encX, X);
  Fp448._1w_red.arith.misc.ser(encP, Y);
  encP[ED448_KEY_SIZE_BYTES_PARAM - 1] = (encX[0] & 0x1) << 7;
}

/**
 * Recoding to a signed radix 2^OMEGA (fold4w4)
 * Given:
 *      r   A 256-bit integer
 * Returns:
 *      (-1)^(S[i])*K[1] for i in [0,64]
 * such that
 *      K[i] in {0, 1, 2, ..., 2^OMEGA}
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
static inline void recoding_signed_scalar_fold4w4_448(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r) {
  const int OMEGA = 4;
  int i, j;
  int8_t carry = 0;
  uint8_t nibble_low, nibble_hig;
  int8_t value, digit;

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 28; j++) {
      nibble_low = r[28 * i + j] & 0xF;
      value = nibble_low + carry;
      carry = value >= (1 << (OMEGA - 1));
      digit = ((value ^ (-carry)) + carry) & 0xF;

      list_digits[56 * i + 2 * j + 0] = (int64_t) digit;
      list_signs[56 * i + 2 * j + 0] = (int64_t) -carry;

      nibble_hig = (r[28 * i + j] >> 4) & 0xF;
      value = nibble_hig + carry;
      carry = value >= (1 << (OMEGA - 1));
      digit = ((value ^ (-carry)) + carry) & 0xF;

      list_digits[56 * i + 2 * j + 1] = (int64_t) digit;
      list_signs[56 * i + 2 * j + 1] = (int64_t) -carry;
    }
  }
  list_digits[112] = carry; /* This is always equal to 0 iff r < 2**447 */
}

/**
 *
 * @param P
 * @param secret_signs
 * @param secret_digits
 * @param index_table
 */
static inline void query_table_fold4w4_ed448(
	Point_precmp_4way_Fp448 *P,
	uint64_t * secret_signs,
	uint64_t * secret_digits,
	uint64_t index_table) {
	const __m256i _P[NUM_DIGITS_FP448] = {
			SET164(0xfffffff),	SET164(0xffffffe),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff),
			SET164(0xfffffff),	SET164(0xfffffff)
	};
	const __m256i one = SET164(1);
	int i,j;
	__m256i mask[9];
	__m256i P_addYX[7], P_subYX[7], P__2dYX[7];
	__m256i digits = LOAD(secret_digits);
	__m256i signs  = LOAD(secret_signs);
	__m256i iiii = ZERO;
	uint64_t * p64Table = (uint64_t*) (((uint8_t*)TableSign_w4_3675k)+SIZE_ONE_LUT_ED448*index_table);

	_mm_prefetch( (const char *) p64Table,_MM_HINT_T0);

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
		mask[i] = SHUF32(_mm256_srai_epi32(mask[i],32),0xE5);
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
 * This version is intended to compute four independent addition points
 * on four pair of points.
 * Taken from Hisil, Wong,Carter,Dawson
 * Section 3.1 page 6
 *
 * @param Q
 * @param P
 */
static inline void _4way_mixadd_ed448(PointXYZT_4way_Fp448 *Q, Point_precmp_4way_Fp448 *P) {
  __m256i *const X1 = Q->X;
  __m256i *const Y1 = Q->Y;
  __m256i *const T1 = Q->T;
  __m256i *const Z1 = Q->Z;
  __m256i *const addY2X2 = P->addYX;
  __m256i *const subY2X2 = P->subYX;
  __m256i *const dT2 = P->_2dYX;
  EltFp448_4w_redradix A, B, C, D;
  argElement_4w addY1X1 = C;
  argElement_4w subY1X1 = D;

  Fp448._4w_red.arith.sub(subY1X1, Y1, X1);
  Fp448._4w_red.arith.add(addY1X1, Y1, X1);
  Fp448._4w_red.arithex.intmul(A, subY1X1, subY2X2);
  Fp448._4w_red.arithex.intmul(B, addY1X1, addY2X2);
  Fp448._4w_red.arithex.intmul(C, dT2, T1);
  Fp448._4w_red.arith.add(D, Z1, Z1);
  Fp448._4w_red.arithex.compress2(C,D);/* [TODO] <- review*/

  Fp448._4w_red.arithex.addsublar(B, A);
  Fp448._4w_red.arithex.addsublar(D, C);
  Fp448._4w_red.arithex.compress2(B, A);
  Fp448._4w_red.arithex.compress2(D, C);

  Fp448._4w_red.arithex.intmul(X1, A, C);
  Fp448._4w_red.arithex.intmul(Y1, D, B);
  Fp448._4w_red.arithex.intmul(Z1, D, C);
  Fp448._4w_red.arithex.intmul(T1, A, B);

  Fp448._4w_red.arithex.compress2(X1, Y1);
  Fp448._4w_red.arithex.compress2(Z1, T1);
}

static const ALIGN uint64_t CONST_2_to_35P_ED448[2*NUM_DIGITS_FP448] = {
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
 * Using the d parameter from the isogeny
 *  d = -39082
 */
static const ALIGN uint64_t CONST_param_curve_2d_02 [2*NUM_DIGITS_FP448] = {
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
 *
 * @param Q
 * @param P
 */
static inline void _1way_fulladd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_2w_H0H8 *P) {
  int i;
  argElement_2w param_curve_2d_02 = (argElement_2w) CONST_param_curve_2d_02;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_ED448;
  argElement_2w XY1 = Q->XY;
  argElement_2w XY2 = P->XY;
  argElement_2w TZ1 = Q->TZ;
  argElement_2w TZ2 = P->TZ;
  EltFp448_2w_redradix _AB, CD, EG, HF;
  EltFp448_2w_redradix FH;
  argElement_4w _subYXaddYX1 = EG;
  argElement_4w _subYXaddYX2 = HF;

  subadd_Fp448_2w_redradix(_subYXaddYX1, XY1, 0);     /* [-sub1|add1] = [ -(Y1-X1) | Y1+X1 ] */
  subadd_Fp448_2w_redradix(_subYXaddYX2, XY2, 1);     /* [ sub2|add2] = [   Y2-X2  | Y2+X2 ] */
  Fp448._2w_red.arithex.compressfast(_subYXaddYX1);
  Fp448._2w_red.arithex.compressfast(_subYXaddYX2);
  Fp448._2w_red.arithex.intmul(_AB, _subYXaddYX1, _subYXaddYX2);    /* [-A|B] = [ -(Y1-X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
  Fp448._2w_red.arithex.intmul(CD, TZ1, TZ2);                    /* [ C|D] = [ T1T2 | Z1Z2 ] */
  Fp448._2w_red.arithex.compress(CD);
  Fp448._2w_red.arithex.intmul(CD, CD, param_curve_2d_02);        /* [ C|D] = [ 2dT1T2 | 2Z1Z2 ] */

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    __m256i _AC = PERM128(_AB[i], CD[i], 0x20);
    __m256i BD = PERM128(_AB[i], CD[i], 0x31);
    EG[i] = ADD(BD, _AC);                            /* [E|G] = [ B+(-A) | D+C ] */
    HF[i] = ADD(SUB(BD, _AC), _2_to_35P[i]);            /* [H|F] = [ B-(-A) | D-C ] */
  }
  Fp448._2w_red.arithex.compress2(EG, HF);

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    FH[i] = PERM64(HF[i], 0x4E);
  }
  Fp448._2w_red.arithex.intmul(XY1, FH, EG);
  Fp448._2w_red.arithex.intmul(TZ1, HF, EG);
  Fp448._2w_red.arithex.compress2(XY1, TZ1);
}

/**
 * Full addition from Hisil page 6.
 *
 * @param Q
 * @param P
 */
static inline void _1way_mixadd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_precompute_2w_H0H8 *P) {
  int i;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_ED448;
  argElement_2w XY = Q->XY;
  argElement_2w TZ = Q->TZ;
  argElement_2w subYXaddYX2 = P->subaddYX;
  argElement_2w _2dT_2Z = P->_2dT_2Z;
  EltFp448_2w_redradix _AB, CD, EG, HF, FH;
  argElement_4w _subYXaddYX1 = EG;

  subadd_Fp448_2w_redradix(_subYXaddYX1, XY, 0);            /* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
  Fp448._2w_red.arithex.intmul(_AB, _subYXaddYX1, subYXaddYX2);    /* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
  Fp448._2w_red.arithex.intmul(CD, TZ, _2dT_2Z);                /* [C|D] = [ 2dT1T2 | 2Z1Z2 ] */

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    __m256i _AC = PERM128(_AB[i], CD[i], 0x20);
    __m256i BD = PERM128(_AB[i], CD[i], 0x31);
    EG[i] = ADD(BD, _AC);                            /* [E|G] = [ B-(-A) | D+C ] */
    HF[i] = ADD(SUB(BD, _AC), _2_to_35P[i]);            /* [H|F] = [ B+(-A) | D-C ] */
  }
  Fp448._2w_red.arithex.compress2(EG, HF);

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    FH[i] = PERM64(HF[i], 0x4E);
  }
  Fp448._2w_red.arithex.intmul(XY, FH, EG);
  Fp448._2w_red.arithex.intmul(TZ, HF, EG);
  Fp448._2w_red.arithex.compress2(XY, TZ);
}

/**
 * Computes the doubling of ONE point
 * stored in P = {XY,ZT}
 *
 * @param P
 */
static inline void _1way_doubling_2w_H0H8(PointXYZT_2w_H0H8 * P) {
  int i;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_ED448;
  argElement_2w XY = P->XY;
  argElement_2w TZ = P->TZ;
  EltFp448_2w_redradix EG, HF, FH;

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    EG[i] = PERM64(TZ[i], 0xEE);
  }
  Fp448._2w_red.arithex.intsqr(XY);         /* [A|B] = [X^2 | Y^2 ] */
  Fp448._2w_red.arithex.intmul(TZ, TZ, EG); /* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
  Fp448._2w_red.arith.add(TZ, TZ, TZ);      /* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    __m256i _BG, AC, _0B, BA;
    BA = PERM64(XY[i], 0x4E);           /* [B|A] */
    _0B = BLEND32(XY[i], ZERO, 0x0F);   /* [0|B] */
    AC = BLEND32(XY[i], TZ[i], 0xF0);   /* [A|C] */
    _BG = SUB(_0B, BA);                 /* [-B|G] = [0|B] - [B|A] = [-B|B-A] */
    EG[i] = BLEND32(_BG, TZ[i], 0x0F);  /* [E|G] */
    HF[i] = SUB(_BG, AC);               /* [H|F] = [-B|G] - [A|C] */
    EG[i] = ADD(EG[i], _2_to_35P[i]);
    HF[i] = ADD(HF[i], _2_to_35P[i]);
  }
  Fp448._2w_red.arithex.compress2(HF, EG);

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    FH[i] = PERM64(HF[i], 0x4E);
  }
  Fp448._2w_red.arithex.intmul(XY, FH, EG);
  Fp448._2w_red.arithex.intmul(TZ, HF, EG);
  Fp448._2w_red.arithex.compress2(XY, TZ);
}


/**
 * Computes the doubling of ONE point
 * stored in P = <X,Y,T,Z>
 *
 * @param P
 */
static inline void _1way_mixadd_4w_H0H8(PointXYTZ_4w_H0H8 * Q, PointXYTZ_precompute_4w_H0H8 * P) {
  int i;
  EltFp448_4w_redradix ABCD, subYX_addYX_T_Z;
  const __m256i CONST_2_to_34P[NUM_DIGITS_FP448] = {
      SET164(0x3ffffffc00000000), SET164(0x3ffffff800000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000)
  };

  const __m256i CONST_2P_00_00_00[NUM_DIGITS_FP448] = {
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x3ffffffc),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffc),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe)
  };

  argElement_4w _2_to_34P = (argElement_2w) CONST_2_to_34P;
  argElement_4w _2P_00_00_00 = (argElement_2w) CONST_2P_00_00_00;
  argElement_4w XYTZ = Q->XYTZ;
  argElement_4w EGHF = subYX_addYX_T_Z;
  argElement_4w FHEG = ABCD;

  const __m256i mask_subadd1 = SET64(0, 0, 0, -1);
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    __m256i M = SHUF32(XYTZ[i], 0x4E);
    __m256i N = XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    subYX_addYX_T_Z[i] = BLEND32(subYX_addYX, XYTZ[i], 0xF0);
  }
  /* [A|B|C|D] = [ (y1-x1)*(y2-x2) | (y1+x1)*(y2+x2) | T*2dT | Z*2Z ] */
  Fp448._4w_red.arithex.intmul(ABCD, subYX_addYX_T_Z, P->subYX_addYX_2dT_2Z);

  const __m256i mask_subadd2 = SET64(-1, 0, 0, -1);
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    __m256i BDBD = PERM64(ABCD[i], 0xDD);
    __m256i ACAC = PERM64(ABCD[i], 0x88);
    EGHF[i] = ADD(ADD(BDBD, _2_to_34P[i]), SUB(XOR(ACAC, mask_subadd2), mask_subadd2));
  }
  Fp448._4w_red.arithex.compress(EGHF);

  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    FHEG[i] = PERM64(EGHF[i], 0x4B);
  }
  Fp448._4w_red.arithex.intmul(XYTZ, EGHF, FHEG);
  Fp448._4w_red.arithex.compress(XYTZ);
}

/**
 * Computes the doubling of ONE point
 * stored in P = <X,Y,T,Z>
 *
 * @param P
 */
static inline void _1way_doubling_4w_H0H8(PointXYTZ_4w_H0H8 * P) {
  int i;
  EltFp448_4w_redradix ABEC, XYZZ;
  const __m256i CONST_2_to_34P[NUM_DIGITS_FP448] = {
      SET164(0x3ffffffc00000000), SET164(0x3ffffff800000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000),
      SET164(0x3ffffffc00000000), SET164(0x3ffffffc00000000)
  };
  argElement_4w _2_to_34P = (argElement_4w) CONST_2_to_34P;
  argElement_4w XYTZ = P->XYTZ;
  argElement_4w FHEG = ABEC;
  argElement_4w EGHF = XYZZ;

  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    XYZZ[i] = PERM64(XYTZ[i], 0xF4);
  }
  /* [A|B|E/2|C/2] = [X^2 | Y^2 | TZ | Z^2 ] */
  Fp448._4w_red.arithex.intmul(ABEC, XYTZ, XYZZ);

  /* [-F|-H|E|G] = [ -G+C | A+B | 2XY | -A+B ] */
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    __m256i CBEB = PERM64(ABEC[i], 0x67);
    __m256i CAE0 = BLEND32(PERM64(ABEC[i], 0xE3), ZERO, 0xC0);
    __m256i A000 = BLEND32(ABEC[i], ZERO, 0xFC);
    __m256i B00A = BLEND32(PERM64(ABEC[i], 0x25), ZERO, 0x3C);
    FHEG[i] = ADD(CBEB, ADD(CAE0, ADD(A000, SUB(_2_to_34P[i], B00A))));
  }
  Fp448._4w_red.arithex.compress(FHEG);

  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    EGHF[i] = PERM64(FHEG[i], 0x1E);
  }
  Fp448._4w_red.arithex.intmul(XYTZ, EGHF, FHEG);
  Fp448._4w_red.arithex.compress(XYTZ);
}

/**
 *
 * @param R0
 * @param Q
 */
static inline void join_points_2w_H0H8(PointXYZT_2w_H0H8 *R0, PointXYZT_4way_Fp448 *Q) {
  PointXYZT_1way_Fp448 Q1, Q2, Q3, Q0;
  PointXYZT_2w_H0H8 R1, R2, R3;

  unzip_Fp448_4w_redradix(Q0.X, Q1.X, Q2.X, Q3.X, Q->X);
  unzip_Fp448_4w_redradix(Q0.Y, Q1.Y, Q2.Y, Q3.Y, Q->Y);
  unzip_Fp448_4w_redradix(Q0.Z, Q1.Z, Q2.Z, Q3.Z, Q->Z);
  unzip_Fp448_4w_redradix(Q0.T, Q1.T, Q2.T, Q3.T, Q->T);

  Fp448._2w_red.arithex.inter(R0->XY, Q0.X, Q0.Y);  Fp448._2w_red.arithex.inter(R0->TZ, Q0.T, Q0.Z);
  Fp448._2w_red.arithex.inter(R1.XY , Q1.X, Q1.Y);  Fp448._2w_red.arithex.inter(R1.TZ , Q1.T, Q1.Z);
  Fp448._2w_red.arithex.inter(R2.XY , Q2.X, Q2.Y);  Fp448._2w_red.arithex.inter(R2.TZ , Q2.T, Q2.Z);
  Fp448._2w_red.arithex.inter(R3.XY , Q3.X, Q3.Y);  Fp448._2w_red.arithex.inter(R3.TZ , Q3.T, Q3.Z);

  _1way_doubling_2w_H0H8(&R3);
  _1way_doubling_2w_H0H8(&R3);
  _1way_doubling_2w_H0H8(&R3);
  _1way_doubling_2w_H0H8(&R3);
  _1way_fulladd_2w_H0H8(&R2, &R3);
  _1way_doubling_2w_H0H8(&R2);
  _1way_doubling_2w_H0H8(&R2);
  _1way_doubling_2w_H0H8(&R2);
  _1way_doubling_2w_H0H8(&R2);
  _1way_fulladd_2w_H0H8(&R1, &R2);
  _1way_doubling_2w_H0H8(&R1);
  _1way_doubling_2w_H0H8(&R1);
  _1way_doubling_2w_H0H8(&R1);
  _1way_doubling_2w_H0H8(&R1);
  _1way_fulladd_2w_H0H8(R0, &R1);
}

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
static inline void isogeny_2w_H0H8(PointXYZT_2w_H0H8 *P) {
  int i;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_ED448;
  argElement_2w XY = P->XY;
  argElement_2w TZ = P->TZ;
  EltFp448_2w_redradix HF, EC, EG, _FH;

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    EC[i] = PERM64(TZ[i], 0xEE);
  }

  Fp448._2w_red.arithex.intsqr(XY);         /* [A|B] = [X^2 | Y^2 ] */
  Fp448._2w_red.arithex.intmul(TZ, TZ, EC); /* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
  Fp448._2w_red.arith.add(EC, TZ, TZ);      /* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    __m256i CB, BA, AA, _H, _FG, _B;
    BA = PERM64(XY[i], 0x4E);           /* [  B  |  A ] */
    AA = PERM64(XY[i], 0x44);           /* [  A  |  A ] */
    CB = PERM128(EC[i], XY[i], 0x31);   /* [  C  |  B ] */
    _H = ADD(CB, AA);                   /* [ C+A |  H ] = [ C+A   | B+A ] */
    _B = BLEND32(XY[i], _H, 0x0F);      /* [ C+A |  B ] */
    _FG = SUB(BA, _B);                  /* [ -F  | -G ] = [ B-C-A | B-A ] */
    _FG = ADD(_FG, _2_to_35P[i]);
    _FH[i] = BLEND32(_H, _FG, 0x0F);    /* [ -F  |  H ] */
    EG[i] = BLEND32(_FG, EC[i], 0x0F);  /* [  E  |  G ]  */
  }
  Fp448._2w_red.arithex.compress2(_FH, EG);

  for (i = 0; i < (NUM_DIGITS_FP448 / 2); i++) {
    HF[i] = PERM64(_FH[i], 0x4E);
  }
  Fp448._2w_red.arithex.intmul(XY, _FH, EG);
  Fp448._2w_red.arithex.intmul(TZ, HF, _FH);
  Fp448._2w_red.arithex.compress2(XY, TZ);
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
static void point_multiplication_fold4w4(PointXYZT_2w_H0H8 *rB, uint8_t *r) {
  unsigned int i;
  PointXYZT_4way_Fp448 Q;
  Point_precmp_4way_Fp448 P;
  ALIGN uint64_t K[112 + 3];
  ALIGN uint64_t S[112 + 3];
  const EltFp448_4w_redradix one_half = {
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x0000000), SET164(0xfffffff),
      SET164(0x8000000), SET164(0x7ffffff)
  };
  recoding_signed_scalar_fold4w4_448(S, K, r);

  Fp448._4w_red.arith.misc.zero(Q.Z);
  Q.Z[0] = SET164(2);

  query_table_fold4w4_ed448(&P, S, K, 0);

  Fp448._4w_red.arith.sub(Q.X, P.addYX, P.subYX);
  Fp448._4w_red.arith.add(Q.Y, P.addYX, P.subYX);
  Fp448._4w_red.arithex.compress(Q.X);
  Fp448._4w_red.arithex.compress(Q.Y);
  Fp448._4w_red.arith.mul(Q.T, Q.X, Q.Y);
  Fp448._4w_red.arith.mul(Q.T, Q.T, (argElement_4w) one_half);

  for (i = 1; i < NUM_LUT_ED448; i++) {
    query_table_fold4w4_ed448(&P, S + 4 * i, K + 4 * i, i);
    _4way_mixadd_ed448(&Q, &P);
  }
  join_points_2w_H0H8(rB, &Q);

  /* inverse isogeny */
  isogeny_2w_H0H8(rB);
}

/**
 * Compute a division by 4 because the point multiplication
 * is processed using the twisted Edwards curve, which is
 * the isogeny_1w_H0H8 of the Edwards curve.
 * Given:
 *      STR_BYTES r
 * Computes
 *      rB = [r]B
 * Ensuring that B is the generator of Ed448.
 *
 * @param rB
 * @param r
 */
static inline void fixed_point_multiplication_ed448(PointXYZT_2w_H0H8 * rB, uint8_t *r) {
	Ed448_PublicKey r_div_4;
	div4(r_div_4,r);
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
static int point_decoding_ed448(
    PointXYTZ_4w_H0H8 * P_4w,
    PointXYZT_2w_H0H8 * P_2w,
    const uint8_t * A) {
  /**
   * Using the d parameter using the
   * goldilocks curve.
   *     d = -39081
   */
  const EltFp448_1w_redradix param_curve_d = {
      0xfff6756, 0xffffffe,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff,
      0xfffffff, 0xfffffff
  };
  EltFp448_1w_redradix X, Y, Z, T;
  EltFp448_1w_redradix a, b, c, d;
  EltFp448_1w_redradix u, v, v2;
  EltFp448_1w_redradix uv, u2, u3v, u5v, u5v3, u5v3_exp;
  Ed448_PublicKey buf;

  memcpy(buf, A, ED448_KEY_SIZE_BYTES_PARAM);
  int8_t bit_X = buf[ED448_KEY_SIZE_BYTES_PARAM - 1] >> 7;

  buf[ED448_KEY_SIZE_BYTES_PARAM - 1] &= 0x7F;
  Fp448._1w_red.arith.misc.unser(Y, buf);
  buf[ED448_KEY_SIZE_BYTES_PARAM - 1] |= bit_X << 7;

  Fp448._1w_red.arith.misc.copy(u, Y);
  Fp448._1w_red.arith.sqr(u);  /* y^2 */
  Fp448._1w_red.arith.mul(v, u, (argElement_1w) param_curve_d);/* dy^2 */
  u[0] = u[0] - 1;  /* u=y^2-1 */
  v[0] = v[0] - 1;  /* v=dy^2-1 */

  Fp448._1w_red.arith.mul(uv, u, v);      /* uv */
  Fp448._1w_red.arith.misc.copy(u2, u);
  Fp448._1w_red.arith.misc.copy(v2, v);
  Fp448._1w_red.arith.sqr(u2);            /* u^2 */
  Fp448._1w_red.arith.sqr(v2);            /* v^2 */
  Fp448._1w_red.arith.mul(u3v, uv, u2);   /* u^3v */
  Fp448._1w_red.arith.mul(u5v, u3v, u2);  /* u^5v */
  Fp448._1w_red.arith.mul(u5v3, u5v, v2); /* u^5v^3 */

  Fp448._1w_red.arith.srt(u5v3_exp, u5v3);
  Fp448._1w_red.arith.mul(X, u3v, u5v3_exp);

  /**
   * Checking whether
   *      v*x^2 == u
   **/
  Fp448._1w_red.arith.misc.copy(uv, X);
  Fp448._1w_red.arith.sqr(uv);         /*   x^2 */
  Fp448._1w_red.arith.mul(uv, uv, v);  /* v*x^2 */

  if (Fp448._1w_red.arith.cmp(uv, u) != 0) {
    /* no square root exists */
    return -1;
  }

  /**
   * Checking whether
   *      X is positive or negative
   **/
  if ((unsigned) bit_X != (X[0] & 0x1)) {
    Fp448._1w_red.arith.neg(X);
    Fp448._1w_red.arithex.compressfast(X);
  }

  /**
   * Convert point P to \phi(P)
   * according to the isogeny_1w_H0H8
   * AND
   * Point represented in extended coordinates
   ***/
  Fp448._1w_red.arith.add(a, X, X);
  Fp448._1w_red.arith.mul(a, a, Y);

  /* Variable 'a' must be negative to produce -A = (-X,Y,Z,-T) */
  Fp448._1w_red.arith.neg(a);
  Fp448._1w_red.arithex.compressfast(a);
  Fp448._1w_red.arith.sqr(X);
  Fp448._1w_red.arith.sqr(Y);
  Fp448._1w_red.arith.add(b, Y, X);
  new_compressfast_Fp448_1w_redradix(b);
  Fp448._1w_red.arith.sub(c, Y, X);
  new_compressfast_Fp448_1w_redradix(c);
  Fp448._1w_red.arith.misc.copy(d, b);
  Fp448._1w_red.arith.neg(d);
  d[0] += 2;
  Fp448._1w_red.arithex.compressfast(d);

  Fp448._1w_red.arith.mul(X, a, d);
  Fp448._1w_red.arith.mul(Y, b, c);
  Fp448._1w_red.arith.mul(Z, c, d);
  Fp448._1w_red.arith.mul(T, a, b);

  if (P_2w != NULL) {
    Fp448._2w_red.arithex.inter(P_2w->XY, X, Y);
    Fp448._2w_red.arithex.inter(P_2w->TZ, T, Z);
  }
  if (P_4w != NULL) {
    zip_Fp448_4w_redradix(P_4w->XYTZ, X, Y, T, Z);
  }
  return 0;
}

#define is_greater_equal_one_512(r)  \
(       (r[7]!=0)            \
	||	(r[6]!=0)            \
	||	(r[5]!=0)            \
	||	(r[4]!=0)            \
	||	(r[3]!=0)            \
	||	(r[2]!=0)            \
	||	(r[1]!=0)            \
	||	(r[0]>=(uint64_t)1)  \
)

#define is_odd(r) ((r[0]&0x1)==1)

#define div_2_512(r) \
{  \
  uint64_t bit7 = r[7]<<63; \
  uint64_t bit6 = r[6]<<63; \
  uint64_t bit5 = r[5]<<63; \
  uint64_t bit4 = r[4]<<63; \
  uint64_t bit3 = r[3]<<63; \
  uint64_t bit2 = r[2]<<63; \
  uint64_t bit1 = r[1]<<63; \
  r[7] = (r[7]>>1);         \
  r[6] = (r[6]>>1) | bit7;  \
  r[5] = (r[5]>>1) | bit6;  \
  r[4] = (r[4]>>1) | bit5;  \
  r[3] = (r[3]>>1) | bit4;  \
  r[2] = (r[2]>>1) | bit3;  \
  r[1] = (r[1]>>1) | bit2;  \
  r[0] = (r[0]>>1) | bit1;  \
}

#define SUB512(b,sign,value) \
  __asm__ __volatile__ (     \
    " movq   0(%0), %%rax;  subq %2, %%rax;  movq %%rax,  0(%0);"  \
    " movq   8(%0), %%rcx;  sbbq %1, %%rcx;  movq %%rcx,  8(%0);"  \
    " movq  16(%0), %%rdx;  sbbq %1, %%rdx;  movq %%rdx, 16(%0);"  \
    " movq  24(%0),  %%r8;  sbbq %1,  %%r8;  movq  %%r8, 24(%0);"  \
    " movq  32(%0),  %%r9;  sbbq %1,  %%r9;  movq  %%r9, 32(%0);"  \
    " movq  40(%0), %%rax;  sbbq %1, %%rax;  movq %%rax, 40(%0);"  \
    " movq  48(%0), %%rcx;  sbbq %1, %%rcx;  movq %%rcx, 48(%0);"  \
    " movq  56(%0), %%rdx;  sbbq %1, %%rdx;  movq %%rdx, 56(%0);"  \
    :                                                              \
    : "r"(b), "r"(sign), "r"(value)                                \
    : "cc", "memory", "%rax", "%rcx", "%rdx", "%r8", "%r9")

/**
 *
 * @param K
 * @param p8_r
 * @param w
 * @return
 */
static inline int wnaf_448bits(int8_t *K, const uint8_t *p8_r, int w) {
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
 *
 * @param table
 * @param P
 */
static inline void precompute_points_2w_ed448(PointXYZT_precompute_2w_H0H8 * table, PointXYZT_2w_H0H8* P) {
  const int num = (1 << (OMEGA_DYNAMIC - 2));
  int i = 0;
  argElement_2w param_curve_2d_02 = (argElement_2w) CONST_param_curve_2d_02;
  PointXYZT_2w_H0H8 iP;
  PointXYZT_precompute_2w_H0H8 _2P_precmp;

  Fp448._2w_red.arith.misc.copy(iP.XY, P->XY);
  Fp448._2w_red.arith.misc.copy(iP.TZ, P->TZ);
  _1way_doubling_2w_H0H8(P);

  subadd_Fp448_2w_redradix(_2P_precmp.subaddYX, P->XY, 1);
  Fp448._2w_red.arithex.intmul(_2P_precmp._2dT_2Z, P->TZ, param_curve_2d_02);
  Fp448._2w_red.arithex.compress2(_2P_precmp.subaddYX, _2P_precmp._2dT_2Z);

  subadd_Fp448_2w_redradix(table[0].subaddYX, iP.XY, 1);
  Fp448._2w_red.arithex.intmul(table[0]._2dT_2Z, iP.TZ, param_curve_2d_02);
  Fp448._2w_red.arithex.compress2(table[0].subaddYX, table[0]._2dT_2Z);

  for (i = 1; i < num; i++) {
    _1way_mixadd_2w_H0H8(&iP, &_2P_precmp);
    subadd_Fp448_2w_redradix(table[i].subaddYX, iP.XY, 1);
    Fp448._2w_red.arithex.intmul(table[i]._2dT_2Z, iP.TZ, param_curve_2d_02);
    Fp448._2w_red.arithex.compress2(table[i].subaddYX, table[i]._2dT_2Z);
  }
}

/**
 *
 * @param P
 * @param index
 */
static inline void read_point_2w_ed448(PointXYZT_precompute_2w_H0H8 * P, int8_t index) {
  const EltFp448_1w_redradix two = {
      0x2, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0
  };
  uint8_t abs_index_r = index > 0 ? index : -index;
  abs_index_r >>= 1;

  const uint64_t *ptr_point = &TableVerif_static_w7_Ed448[3 * NUM_DIGITS_FP448_X64 * abs_index_r];
  EltFp448_1w_redradix add, sub, _2dT;

  Fp448._1w_red.arith.misc.unser(add, (uint8_t *) (ptr_point + 0 * NUM_DIGITS_FP448_X64));
  Fp448._1w_red.arith.misc.unser(sub, (uint8_t *) (ptr_point + 1 * NUM_DIGITS_FP448_X64));
  Fp448._1w_red.arith.misc.unser(_2dT, (uint8_t *) (ptr_point + 2 * NUM_DIGITS_FP448_X64));

  if (index < 0) {
    Fp448._1w_red.arith.neg(_2dT);
    Fp448._1w_red.arithex.compress(_2dT);
    Fp448._2w_red.arithex.inter(P->subaddYX, add, sub);
  } else {
    Fp448._2w_red.arithex.inter(P->subaddYX, sub, add);
  }
  Fp448._2w_red.arithex.inter(P->_2dT_2Z, _2dT, (argElement_1w) two);
}

/**
 *
 * @param sB_hA
 * @param s
 * @param h
 * @param A
 */
static inline void double_point_mult_2w_ed448(uint8_t *sB_hA, const uint8_t *s, uint8_t *h, PointXYZT_2w_H0H8 * A) {
  int i;
  int8_t wnaf_r[460] = {0};
  int8_t wnaf_h[460] = {0};
  ALIGN uint8_t s_div_4[ED448_KEY_SIZE_BYTES_PARAM];
  ALIGN uint8_t h_div_4[ED448_KEY_SIZE_BYTES_PARAM];

  PointXYZT_2w_H0H8 Q;
  PointXYZT_precompute_2w_H0H8 tableA[1 << (OMEGA_DYNAMIC - 2)];

  /**
   * To use Isogeny
   * compute h' = h/4 mod q
   * compute s' = s/4 mod q
   **/
  div4(s_div_4, s);
  div4(h_div_4, h);

  int l_r = wnaf_448bits(wnaf_r, s_div_4, OMEGA_STATIC);
  int l_h = wnaf_448bits(wnaf_h, h_div_4, OMEGA_DYNAMIC);
  int l = l_r > l_h ? l_r : l_h;

  precompute_points_2w_ed448(tableA, A);
  Fp448._2w_red.arith.misc.zero(Q.XY);
  Fp448._2w_red.arith.misc.zero(Q.TZ);
  Q.XY[0] = SET64(0, 1, 0, 0);
  Q.TZ[0] = SET64(0, 1, 0, 0);

  for (i = l - 1; i >= 0; i--) {
    _1way_doubling_2w_H0H8(&Q);
    /* known point addition */
    if (wnaf_r[i] != 0) {
      PointXYZT_precompute_2w_H0H8 P;
      read_point_2w_ed448(&P, wnaf_r[i]);
      _1way_mixadd_2w_H0H8(&Q, &P);
    }
    /* unknown point addition */
    if (wnaf_h[i] != 0) {
      PointXYZT_precompute_2w_H0H8 *P = NULL;
      uint8_t abs_index_h = wnaf_h[i] > 0 ? wnaf_h[i] : -wnaf_h[i];
      abs_index_h >>= 1;
      P = &tableA[abs_index_h];
      if (wnaf_h[i] < 0) {
        int ii = 0;
        PointXYZT_precompute_2w_H0H8 _P;
        for (ii = 0; ii < (NUM_DIGITS_FP448 / 2); ii++) {
          _P.subaddYX[ii] = PERM64(P->subaddYX[ii], 0x4E);
        }
        Fp448._2w_red.arith.ngz(_P._2dT_2Z, P->_2dT_2Z);
        Fp448._2w_red.arithex.compressfast(_P._2dT_2Z);
        _1way_mixadd_2w_H0H8(&Q, &_P);
      } else {
        _1way_mixadd_2w_H0H8(&Q, P);
      }
    }
  }
  /* inverse isogeny */
  isogeny_2w_H0H8(&Q);

  point_encoding_ed448(sB_hA, &Q);
}

static inline void precompute_points_4w_ed448(PointXYTZ_precompute_4w_H0H8 * table, PointXYTZ_4w_H0H8* P) {
  const ALIGN uint64_t CONST_param_curve_1_1_2d_02[4*NUM_DIGITS_FP448] = {
      0x1, 0x1, 0xffeceab, 0x2,
      0x0, 0x0, 0xffffffe, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0,
      0x0, 0x0, 0xfffffff, 0x0
  };
  const __m256i CONST_2P_00_00_00_H0H8[NUM_DIGITS_FP448] = {
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x3ffffffc),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffc),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe),
      SET64(0, 0, 0, 0x1ffffffe), SET64(0, 0, 0, 0x1ffffffe)
  };

  argElement_4w _2P_00_00_00 = (argElement_4w) CONST_2P_00_00_00_H0H8;
  PointXYTZ_4w_H0H8 iP;
  const int num = (1 << (OMEGA_DYNAMIC - 2));
  int i = 0, j = 0;
  const __m256i mask_subadd1 = SET64(0, 0, 0, -1);

  Fp448._4w_red.arith.misc.copy(iP.XYTZ, P->XYTZ);

  _1way_doubling_4w_H0H8(P);
  PointXYTZ_precompute_4w_H0H8 _2P_precmp;

  Fp448._4w_red.arithex.intmul(_2P_precmp.subYX_addYX_2dT_2Z, P->XYTZ, (argElement_4w)CONST_param_curve_1_1_2d_02);
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    __m256i M = SHUF32(P->XYTZ[i], 0x4E);
    __m256i N = P->XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    _2P_precmp.subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, _2P_precmp.subYX_addYX_2dT_2Z[i], 0xF0);
  }
  Fp448._4w_red.arithex.compress(_2P_precmp.subYX_addYX_2dT_2Z);

  Fp448._4w_red.arithex.intmul(table[0].subYX_addYX_2dT_2Z, iP.XYTZ, (argElement_4w)CONST_param_curve_1_1_2d_02);
  for (i = 0; i < NUM_DIGITS_FP448; i++) {
    __m256i M = SHUF32(iP.XYTZ[i], 0x4E);
    __m256i N = iP.XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    table[0].subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, table[0].subYX_addYX_2dT_2Z[i], 0xF0);
  }
  Fp448._4w_red.arithex.compress(table[0].subYX_addYX_2dT_2Z);

  for (j = 1; j < num; j++) {
    _1way_mixadd_4w_H0H8(&iP, &_2P_precmp);
    Fp448._4w_red.arithex.intmul(table[j].subYX_addYX_2dT_2Z, iP.XYTZ,(argElement_4w) CONST_param_curve_1_1_2d_02);
    for (i = 0; i < NUM_DIGITS_FP448; i++) {
      __m256i M = SHUF32(iP.XYTZ[i], 0x4E);
      __m256i N = iP.XYTZ[i];
      __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
      table[j].subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, table[j].subYX_addYX_2dT_2Z[i], 0xF0);
    }
    Fp448._4w_red.arithex.compress(table[j].subYX_addYX_2dT_2Z);
  }
}

static inline void read_point_4w_ed448(PointXYTZ_precompute_4w_H0H8 * P, int8_t index) {
  const EltFp448_1w_redradix two = {
      0x2, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0
  };
  EltFp448_1w_redradix add, sub, _2dT;
  uint8_t abs_index_r = index > 0 ? index : -index;
  abs_index_r >>= 1;

  const uint64_t *ptr_point = &TableVerif_static_w7_Ed448[3 * NUM_DIGITS_FP448_X64 * abs_index_r];

  Fp448._1w_red.arith.misc.unser(add, (uint8_t *)(ptr_point + 0 * NUM_DIGITS_FP448_X64));
  Fp448._1w_red.arith.misc.unser(sub, (uint8_t *)(ptr_point + 1 * NUM_DIGITS_FP448_X64));
  Fp448._1w_red.arith.misc.unser(_2dT,(uint8_t *)(ptr_point + 2 * NUM_DIGITS_FP448_X64));

  if (index < 0)
  {
    Fp448._1w_red.arith.neg(_2dT);
    Fp448._1w_red.arithex.compress(_2dT);
    zip_Fp448_4w_redradix(P->subYX_addYX_2dT_2Z, add, sub, _2dT, (argElement_1w) two);
  } else {
    zip_Fp448_4w_redradix(P->subYX_addYX_2dT_2Z, sub, add, _2dT, (argElement_1w) two);
  }
}

static inline void unzip_2w_Element_4w_h0h8(argElement_2w pA,argElement_2w pB,argElement_4w A) {
  int i=0;
  for(i=0;i<(NUM_DIGITS_FP448/2);i++) {
    __m256i pA0 = UPKL64(A[2*i], A[2*i+1]);
    __m256i pB0 = UPKH64(A[2*i], A[2*i+1]);
    pA[i] = PERM128(pA0, pB0, 0x20);
    pB[i] = PERM128(pA0, pB0, 0x31);
  }
}

static inline void double_point_mult_4w_ed448(
    uint8_t *sB_hA,
    const uint8_t *s,
    uint8_t *h,
    PointXYTZ_4w_H0H8 * A) {
  int i;
  int8_t wnaf_r[460] = {0};
  int8_t wnaf_h[460] = {0};
  ALIGN uint8_t s_div_4[57];
  ALIGN uint8_t h_div_4[57];

  PointXYTZ_4w_H0H8 Q;
  PointXYZT_2w_H0H8 QQ;
  PointXYTZ_precompute_4w_H0H8 tableA[1 << (OMEGA_DYNAMIC - 2)];

  /**
   * To use Isogeny
   * compute h' = h/4 mod q
   * compute s' = s/4 mod q
   **/
  div4(s_div_4, s);
  div4(h_div_4, h);

  int l_r = wnaf_448bits(wnaf_r, s_div_4, OMEGA_STATIC);
  int l_h = wnaf_448bits(wnaf_h, h_div_4, OMEGA_DYNAMIC);
  int l = l_r > l_h ? l_r : l_h;

  precompute_points_4w_ed448(tableA, A);

  Fp448._4w_red.arith.misc.zero(Q.XYTZ);
  Q.XYTZ[0] = SET64(1, 0, 1, 0);

  for (i = l - 1; i >= 0; i--) {
    _1way_doubling_4w_H0H8(&Q);
    /* known point addition */
    if (wnaf_r[i] != 0) {
      PointXYTZ_precompute_4w_H0H8 P;
      read_point_4w_ed448(&P, wnaf_r[i]);
      _1way_mixadd_4w_H0H8(&Q, &P);
    }
    /* unknown point addition */
    if (wnaf_h[i] != 0) {
      PointXYTZ_precompute_4w_H0H8 *P = NULL;
      uint8_t abs_index_h = wnaf_h[i] > 0 ? wnaf_h[i] : -wnaf_h[i];
      abs_index_h >>= 1;
      P = &tableA[abs_index_h];
      if (wnaf_h[i] < 0) {
        PointXYTZ_precompute_4w_H0H8 _P;
        Fp448._4w_red.arith.ngz(_P.subYX_addYX_2dT_2Z, P->subYX_addYX_2dT_2Z);
        Fp448._4w_red.arithex.compressfast(_P.subYX_addYX_2dT_2Z);
        _1way_mixadd_4w_H0H8(&Q, &_P);
      } else {
        _1way_mixadd_4w_H0H8(&Q, P);
      }
    }
  }
  unzip_2w_Element_4w_h0h8(QQ.XY, QQ.TZ, Q.XYTZ);

  /* inverse isogeny */
  isogeny_2w_H0H8(&QQ);

  point_encoding_ed448(sB_hA, &QQ);
}

#else
extern int version;
#endif /* ED448 */
