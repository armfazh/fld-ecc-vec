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

#if defined(ED255)

#include "edwards255.h"
#include "table_verf_w7_ed25519.h"

#if   LOOKUP_TABLE_SIZE == LUT_12KB
#include "table_sign_w4_12k_ed25519.h"
#elif LOOKUP_TABLE_SIZE == LUT_24KB
#include "table_sign_w4_24k_ed25519.h"
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif

/**
 * Doubling for signing
 * Hisil Section 3.3 page 7
 */
void _1way_doubling_1w_full(PointXYZT_1way_full *P) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  argElement_1w const X1 = P->X;
  argElement_1w const Y1 = P->Y;
  argElement_1w const T1 = P->T;
  argElement_1w const Z1 = P->Z;

  EltFp25519_1w_fullradix A, B, C, E, F, G, H;

  fp.misc.copy(A, X1);
  fp.misc.copy(B, Y1);
  fp.misc.copy(C, Z1);
  fp.sqr(A);
  fp.sqr(B);
  fp.sqr(C);

  fp.add(C, C, C);
  fp.add(E, X1, Y1);
  fp.sqr(E);
  fp.sub(E, E, A);
  fp.sub(E, E, B);

  fp.sub(G, B, A);
  fp.sub(F, G, C);
  fp.add(H, A, B);
  fp.neg(H);

  fp.mul(X1, E, F);
  fp.mul(Y1, G, H);
  fp.mul(T1, E, H);
  fp.mul(Z1, F, G);
}

/**
 * Addition law for twisted Edwards curves
 * Hisil Section 3.1 page 6
 */
void _1way_fulladd_1w_full(
    PointXYZT_1way_full *R,
    PointXYZT_1way_full *Q,
    PointXYZT_1way_full *P) {
	EltFp25519_1w_fullradix A,B,C,D,E,F,G,H;
	uint64_t * const X1 = Q->X; uint64_t * const X2 = P->X; uint64_t * const X3 = R->X;
	uint64_t * const Y1 = Q->Y; uint64_t * const Y2 = P->Y; uint64_t * const Y3 = R->Y;
	uint64_t * const Z1 = Q->Z; uint64_t * const Z2 = P->Z; uint64_t * const Z3 = R->Z;
	uint64_t * const T1 = Q->T; uint64_t * const T2 = P->T; uint64_t * const T3 = R->T;
	uint64_t * const addY1X1 = E;
	uint64_t * const subY1X1 = F;
	uint64_t * const addY2X2 = G;
	uint64_t * const subY2X2 = H;

	EltFp25519_1w_fullradix param_2d = {
			0xebd69b9426b2f159,
			0x00e0149a8283b156,
			0x198e80f2eef3d130,
			0x2406d9dc56dffce7
	};

    const Arith_1w fp = Fp25519._1w_full.arith;

	fp.add(addY1X1,Y1,X1);     /*  addY1X1 <- Y1+X1 */
	fp.sub(subY1X1,Y1,X1);     /*  subY1X1 <- Y1-X1 */
	fp.add(addY2X2,Y2,X2);     /*  addY2X2 <- Y2+X2 */
	fp.sub(subY2X2,Y2,X2);     /*  subY2X2 <- Y2-X2 */

	fp.mul(A,subY1X1,subY2X2); /*  A <- subY1X1 * subY2X2  */
	fp.mul(B,addY1X1,addY2X2); /*  B <- addY1X1 * addY2X2  */
	fp.mul(C,T1,T2);           /*  C <-      T1 * T2       */
	fp.mul(C,C,param_2d);      /*  C <-      2d * C        */
	fp.mul(D,Z1,Z2);           /*  D <-      Z1 * Z2       */
	fp.add(D,D,D);             /*  D <-       2 * D        */

	fp.sub(E,B,A);   /* E <- B - A  */
	fp.sub(F,D,C);   /* F <- D - C  */
	fp.add(G,D,C);   /* G <- D + C  */
	fp.add(H,B,A);   /* H <- B + A  */

	fp.mul(X3,E,F);  /* X <- E * F  */
	fp.mul(Y3,G,H);  /* Y <- G * H  */
	fp.mul(Z3,F,G);  /* Z <- F * G  */
	fp.mul(T3,E,H);  /* T <- E * H  */
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
static void _4way_mixadd_25519(PointXYZT_4way *Q, Point_precmp_4way *P) {
  argElement_4w const X1 = Q->X;
  argElement_4w const Y1 = Q->Y;
  argElement_4w const T1 = Q->T;
  argElement_4w const Z1 = Q->Z;
  argElement_4w const addY2X2 = P->addYX;
  argElement_4w const subY2X2 = P->subYX;
  argElement_4w const dT2 = P->_2dYX;

  EltFp25519_4w_redradix A, B, C, D;
  EltFp25519_4w_redradix addY1X1, subY1X1;

  Fp25519._4w_red.arithex.addsub(addY1X1, subY1X1, Y1, X1);
  Fp25519._4w_red.arithex.intmul(A, subY1X1, subY2X2);
  Fp25519._4w_red.arithex.intmul(B, addY1X1, addY2X2);
  Fp25519._4w_red.arithex.intmul(C, dT2, T1);
  Fp25519._4w_red.arith.add(D, Z1, Z1);
  Fp25519._4w_red.arithex.compress2(C, D);
  Fp25519._4w_red.arithex.addsublar(B, A);
  Fp25519._4w_red.arithex.addsublar(D, C);
  Fp25519._4w_red.arithex.compress2(B, A);
  Fp25519._4w_red.arithex.compress2(D, C);
  Fp25519._4w_red.arithex.intmul(X1, A, C);
  Fp25519._4w_red.arithex.intmul(T1, A, B);
  Fp25519._4w_red.arithex.intmul(Z1, D, C);
  Fp25519._4w_red.arithex.intmul(Y1, D, B);
  Fp25519._4w_red.arithex.compress2(X1, T1);
  Fp25519._4w_red.arithex.compress2(Y1, Z1);
}


/*
 * TODO  Incomplete
 */
static inline void _4way_doubling_25519(PointXYZT_4way *Q) {
  argElement_4w const X1 = Q->X;
  argElement_4w const Y1 = Q->Y;
  argElement_4w const T1 = Q->T;
  argElement_4w const Z1 = Q->Z;

  EltFp25519_4w_redradix A,B,C,D;
  argElement_4w addY1X1 = A;
  argElement_4w subY1X1 = B;

  Fp25519._4w_red.arith.sub(subY1X1,Y1,X1);
  Fp25519._4w_red.arithex.intsqr(subY1X1);
  Fp25519._4w_red.arith.add(addY1X1,Y1,X1);
  Fp25519._4w_red.arithex.intsqr(addY1X1);

  Fp25519._4w_red.arith.misc.copy(C,T1);
  Fp25519._4w_red.arith.misc.copy(D,Z1);
  Fp25519._4w_red.arithex.intsqr(C);
  Fp25519._4w_red.arithex.intsqr(D);
  Fp25519._4w_red.arithex.addsublar(B,A);
  Fp25519._4w_red.arithex.addsublar(D,C);

  Fp25519._4w_red.arithex.compress2(A,B);
  Fp25519._4w_red.arithex.compress2(C,D);

  Fp25519._4w_red.arithex.intmul(X1,A,C);
  Fp25519._4w_red.arithex.intmul(Y1,D,B);
  Fp25519._4w_red.arithex.intmul(Z1,D,C);
  Fp25519._4w_red.arithex.intmul(T1,A,B);

  Fp25519._4w_red.arithex.compress2(Z1,T1);
  Fp25519._4w_red.arithex.compress2(X1,Y1);
}
/**
 * Addition law for twisted Edwards curves
 * Hisil Section 3.1 page 6
 */
static inline void _1way_fulladd_1w_H0H5(PointXYZT_1way *Q, PointXYZT_1way *P) {
  argElement_1w X1 = Q->X;
  argElement_1w X2 = P->X;
  argElement_1w Y1 = Q->Y;
  argElement_1w Y2 = P->Y;
  argElement_1w Z1 = Q->Z;
  argElement_1w Z2 = P->Z;
  argElement_1w T1 = Q->T;
  argElement_1w T2 = P->T;

  EltFp25519_1w_redradix A, B, C, D, tmp_X2, tmp_Y2;
  const EltFp25519_1w_redradix param_2d_H0H5 = {
      0x2b2f159, 0x0f3d130,
      0x1a6e509, 0x3407977,
      0x22add7a, 0x19ce331,
      0x0d4141d, 0x1c56dff,
      0x0038052, 0x0901b67};

  Fp25519._1w_red.arith.misc.copy(tmp_X2, X2);
  Fp25519._1w_red.arith.misc.copy(tmp_Y2, Y2);

  Fp25519._1w_red.arith.add(Z1, Z1, Z1);
  Fp25519._1w_red.arithex.addsub(Y1, X1);
  Fp25519._1w_red.arithex.addsub(tmp_Y2, tmp_X2);
  Fp25519._1w_red.arithex.compressfast(tmp_X2);
  Fp25519._1w_red.arithex.compressfast(tmp_Y2);

  Fp25519._1w_red.arithex.intmul(A, X1, tmp_X2);
  Fp25519._1w_red.arithex.compress(A);
  Fp25519._1w_red.arithex.intmul(B, Y1, tmp_Y2);
  Fp25519._1w_red.arithex.compress(B);
  Fp25519._1w_red.arithex.intmul(C, T1, T2);
  Fp25519._1w_red.arithex.compress(C);
  Fp25519._1w_red.arithex.intmul(C, C, (argElement_1w) param_2d_H0H5);
  Fp25519._1w_red.arithex.compress(C);
  Fp25519._1w_red.arithex.intmul(D, Z1, Z2);
  Fp25519._1w_red.arithex.compress(D);

  Fp25519._1w_red.arithex.addsub(B, A);
  Fp25519._1w_red.arithex.addsub(D, C);
  Fp25519._1w_red.arithex.compressfast(C);
  Fp25519._1w_red.arithex.compressfast(B);

  Fp25519._1w_red.arithex.intmul(X1, A, C);
  Fp25519._1w_red.arithex.intmul(T1, A, B);
  Fp25519._1w_red.arithex.intmul(Z1, D, C);
  Fp25519._1w_red.arithex.intmul(Y1, D, B);

  Fp25519._1w_red.arithex.compress(X1);
  Fp25519._1w_red.arithex.compress(T1);
  Fp25519._1w_red.arithex.compress(Z1);
  Fp25519._1w_red.arithex.compress(Y1);
}

/**
 * Addition law for twisted Edwards curves
 * Hisil Section 3.1 page 6
 */
static inline void _2way_fulladd(PointXYZT_2way *Q, PointXYZT_2way *P) {
	__m256i * const X1 = Q->X; __m256i * const X2 = P->X;
	__m256i * const Y1 = Q->Y; __m256i * const Y2 = P->Y;
	__m256i * const Z1 = Q->Z; __m256i * const Z2 = P->Z;
	__m256i * const T1 = Q->T; __m256i * const T2 = P->T;

	EltFp25519_2w_redradix A,B,C,D;
	const EltFp25519_2w_redradix param_2d_H0H5 = {
			SET64(0x0f3d130,0x2b2f159,0x0f3d130,0x2b2f159),
			SET64(0x3407977,0x1a6e509,0x3407977,0x1a6e509),
			SET64(0x19ce331,0x22add7a,0x19ce331,0x22add7a),
			SET64(0x1c56dff,0x0d4141d,0x1c56dff,0x0d4141d),
			SET64(0x0901b67,0x0038052,0x0901b67,0x0038052)};

	Fp25519._2w_red.arith.add(Z1,Z1,Z1);
	Fp25519._2w_red.arithex.addsub(Y1,X1);
	Fp25519._2w_red.arithex.addsub(Y2,X2);
	Fp25519._2w_red.arithex.compressfast(X2);
	Fp25519._2w_red.arithex.compressfast(Y2);

	Fp25519._2w_red.arithex.intmul(A,X1,X2);		    Fp25519._2w_red.arithex.compress(A);
	Fp25519._2w_red.arithex.intmul(B,Y1,Y2);		    Fp25519._2w_red.arithex.compress(B);
	Fp25519._2w_red.arithex.intmul(C,T1,T2);           Fp25519._2w_red.arithex.compress(C);
	Fp25519._2w_red.arithex.intmul(C,C,(argElement_2w)param_2d_H0H5);	Fp25519._2w_red.arithex.compress(C);
	Fp25519._2w_red.arithex.intmul(D,Z1,Z2);			Fp25519._2w_red.arithex.compress(D);

	Fp25519._2w_red.arithex.addsub(B,A);
	Fp25519._2w_red.arithex.addsub(D,C);/* [TODO] put this additions before reduction */
	Fp25519._2w_red.arithex.compressfast(C);
	Fp25519._2w_red.arithex.compressfast(B);

	Fp25519._2w_red.arithex.intmul(X1,A,C);
	Fp25519._2w_red.arithex.intmul(T1,A,B);
	Fp25519._2w_red.arithex.intmul(Z1,D,C);
	Fp25519._2w_red.arithex.intmul(Y1,D,B);

	Fp25519._2w_red.arithex.compress(X1);
	Fp25519._2w_red.arithex.compress(T1);
	Fp25519._2w_red.arithex.compress(Z1);
	Fp25519._2w_red.arithex.compress(Y1);
}

void _2way_doubling(PointXYZT_2way *P, const int compute_T) {
  const Arith_2w fp = Fp25519._2w_red.arith;
  __m256i *const X1 = P->X;
  __m256i *const Y1 = P->Y;
  __m256i *const T1 = P->T;
  __m256i *const Z1 = P->Z;
  EltFp25519_2w_redradix E, F, G, H;

  fp.add(T1, X1, Y1);
  Fp25519._2w_red.arithex.compressfast(T1);
  fp.sqr(T1);          // (X1+Y1)^2
  fp.sqr(X1);          // A = X^2
  fp.sqr(Y1);          // B = Y^2
  fp.sqr(Z1);          // C = Z^2
  fp.add(Z1, Z1, Z1);  //   = 2Z^2
  Fp25519._2w_red.arithex.compressfast(Z1);

  naddsub_Fp255_2w_redradix(H, G, X1, Y1);
  Fp25519._2w_red.arithex.compressfast(H);
  Fp25519._2w_red.arithex.compressfast(G);
  fp.add(E, T1, H);  // E = T1 + H
  fp.sub(F, G, Z1);  // F = G  - Z1
  Fp25519._2w_red.arithex.compressfast(E);
  Fp25519._2w_red.arithex.compressfast(F);

  fp.mul(X1, E, F);  // X = E * F
  fp.mul(Y1, G, H);  // Y = G * H
  fp.mul(Z1, F, G);  // Z = F * G
  if (compute_T) {
    fp.mul(T1, E, H);  // T = E * H
  }
}

static const ALIGN uint64_t CONST_2_to_35P_2w[2*NUM_DIGITS_FP25519] = {
    0x1fffff6800000000, 0x0ffffff800000000, 0x1fffff6800000000, 0x0ffffff800000000,
    0x0ffffff800000000, 0x1ffffff800000000, 0x0ffffff800000000, 0x1ffffff800000000,
    0x1ffffff800000000, 0x0ffffff800000000, 0x1ffffff800000000, 0x0ffffff800000000,
    0x0ffffff800000000, 0x1ffffff800000000, 0x0ffffff800000000, 0x1ffffff800000000,
    0x1ffffff800000000, 0x0ffffff800000000, 0x1ffffff800000000, 0x0ffffff800000000
};

/**
 * Computes the doubling of ONE point
 * stored in P = {XY,ZT}
 * @param P
 */
static inline void _1way_doubling_2w_H0H5(PointXYZT_2w_H0H5 *P) {
  int i;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_2w;
  argElement_2w XY = P->XY;
  argElement_2w TZ = P->TZ;
  EltFp25519_2w_redradix EG, HF;

  for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
    EG[i] = PERM64(TZ[i], 0xEE);
  }
  Fp25519._2w_red.arithex.intsqr(XY);       /* [A|B] = [X^2 | Y^2 ] */
  Fp25519._2w_red.arithex.intmul(TZ, TZ, EG); /* [T|Z] = [ TZ | Z^2 ] = [ XY | Z^2 ] */
  Fp25519._2w_red.arith.add(TZ, TZ, TZ);      /* [E|C] = [2XY | 2Z^2] = [ 2(X+Y)^2-A-B | 2Z^2 ] */

  for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
    __m256i _BG, AC, _0B, BA;
    BA = PERM64(XY[i], 0x4E);         /* [B|A] */
    _0B = BLEND32(XY[i], ZERO, 0x0F); /* [0|B] */
    AC = BLEND32(XY[i], TZ[i], 0xF0); /* [A|C] */
    _BG = SUB(_0B, BA);                 /* [-B|G] = [0|B] - [B|A] = [-B|B-A] */
    EG[i] = BLEND32(_BG, TZ[i], 0x0F);     /* [E|G] */
    HF[i] = SUB(_BG, AC);                 /* [H|F] = [-B|G] - [A|C] */
    EG[i] = ADD(EG[i], _2_to_35P[i]);
    HF[i] = ADD(HF[i], _2_to_35P[i]);
  }
  Fp25519._2w_red.arithex.compress2(HF, EG);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
  intmul2_Fp255_2w_redradix(TZ, XY, HF, EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
  EltFp25519_2w_redradix FH;
    for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
    {
        FH[i] = PERM64(HF[i],0x4E);
    }
    Fp25519._2w_red.arithex.intmul(XY,FH,EG);
    Fp25519._2w_red.arithex.intmul(TZ,HF,EG);
#endif
  Fp25519._2w_red.arithex.compress2(XY, TZ);
}

/**
 * Full addition from Hisil page 6.
 * @param Q
 * @param P
 */
static inline void _1way_mixadd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_precompute_2w_H0H5 *P) {
  int i;
  argElement_2w _2_to_35P = (argElement_2w) CONST_2_to_35P_2w;
  argElement_2w XY = Q->XY;
  argElement_2w TZ = Q->TZ;
  argElement_2w subYXaddYX2 = P->subaddYX;
  argElement_2w _2dT_2Z = P->_2dT_2Z;

  EltFp25519_2w_redradix _AB, CD, EG, HF;
  argElement_2w _subYXaddYX1 = EG;

  subadd_Fp255_2w_redradix(_subYXaddYX1, XY, 0);            /* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
  Fp25519._2w_red.arithex.intmul(_AB, _subYXaddYX1, subYXaddYX2);    /* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
  Fp25519._2w_red.arithex.intmul(CD, TZ, _2dT_2Z);                /* [C|D] = [ 2dT1T2 | 2Z1Z2 ] */
  /*[TODO: Precompute param_d to help to multiplication]*/

  for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
    __m256i _AC = PERM128(_AB[i], CD[i], 0x20);
    __m256i BD = PERM128(_AB[i], CD[i], 0x31);
    EG[i] = ADD(BD, _AC);                    /* [E|G] = [ B+(-A) | D+C ] */
    HF[i] = ADD(SUB(BD, _AC), _2_to_35P[i]);    /* [H|F] = [ B-(-A) | D-C ] */
  }
  Fp25519._2w_red.arithex.compress2(EG, HF);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
  intmul2_Fp255_2w_redradix(TZ, XY, HF, EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
  EltFp25519_2w_redradix FH;
    for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
    {
        FH[i] = PERM64(HF[i],0x4E);
    }
    Fp25519._2w_red.arithex.intmul(XY,FH,EG);
    Fp25519._2w_red.arithex.intmul(TZ,HF,EG);
#endif
  Fp25519._2w_red.arithex.compress2(XY, TZ);
}

/**
 * Full addition from Hisil page 6.
 * @param Q
 * @param P
 */
static inline void _1way_fulladd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_2w_H0H5 *P)
{
	int i;
	argElement_2w _2_to_35P = (argElement_2w)CONST_2_to_35P_2w;
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

	EltFp25519_2w_redradix AB,CD,EG,HF;
	argElement_2w subYXaddYX1 = EG;
	argElement_2w subYXaddYX2 = HF;

	subadd_Fp255_2w_redradix(subYXaddYX1,XY1,0); 		 /* [-sub1|add1] = [ -Y1+X1 | Y1+X1 ] */
	subadd_Fp255_2w_redradix(subYXaddYX2,XY2,1); 		 /* [ sub2|add2] = [  Y2-X2 | Y2+X2 ] */
	Fp25519._2w_red.arithex.intmul(AB,subYXaddYX1,subYXaddYX2); /* [-A|B] = [ (-Y1+X1)(Y2-X2) | (Y1+X1)(Y2+X2) ] */
	Fp25519._2w_red.arithex.intmul(CD,TZ1,TZ2); 			     /* [C|D] = [ T1*T2 | Z1*Z2 ] */
	Fp25519._2w_red.arithex.compress(CD);
	Fp25519._2w_red.arithex.intmul(CD,CD,(argElement_2w)param_curve_2d_02); /* [C|D] = [  2d*C | 2*D   ] */

	Fp25519._2w_red.arithex.compressfast(AB);
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		__m256i AC = PERM128(AB[i],CD[i],0x20);
		__m256i BD = PERM128(AB[i],CD[i],0x31);
		EG[i] = ADD(BD,AC);						/* [E|G] = [ B+(-A) | D+C ] */
		HF[i] = ADD(SUB(BD,AC),_2_to_35P[i]);	/* [H|F] = [ B-(-A) | D-C ] */
	}
	Fp25519._2w_red.arithex.compress2(EG,HF);
#if defined PROCESSOR && (PROCESSOR == SKYLAKE)
	intmul2_Fp255_2w_redradix(TZ1,XY1,HF,EG);
#elif defined PROCESSOR && (PROCESSOR == HASWELL)
	EltFp25519_2w_redradix FH;
	for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
	{
		FH[i] = PERM64(HF[i],0x4E);
	}
	Fp25519._2w_red.arithex.intmul(XY1,FH,EG);
	Fp25519._2w_red.arithex.intmul(TZ1,HF,EG);
#else
	#error "Define the PROCESSOR variable using one of these: {HASWELL,SKYLAKE}"
#endif
	Fp25519._2w_red.arithex.compress2(XY1,TZ1);
}

static inline void point_encoding_ed25519(uint8_t*encP, PointXYZT_2w_H0H5* P) {
  EltFp25519_1w_fullradix X, Y, Z, invZ;
  EltFp25519_1w_redradix tX, tY, tZ, tT;
  ALIGN uint8_t encX[SIZE_FP25519];

  Fp25519._2w_red.arithex.deinter(tX, tY, P->XY);
  Fp25519._2w_red.arithex.deinter(tT, tZ, P->TZ);

  Fp25519._1w_red.arith.misc.ser((uint8_t *) X, tX);
  Fp25519._1w_red.arith.misc.ser((uint8_t *) Y, tY);
  Fp25519._1w_red.arith.misc.ser((uint8_t *) Z, tZ);

  Fp25519._1w_full.arith.inv(invZ, Z);
  Fp25519._1w_full.arith.mul(X, X, invZ);
  Fp25519._1w_full.arith.mul(Y, Y, invZ);

  Fp25519._1w_full.arith.misc.ser(encX, X);
  Fp25519._1w_full.arith.misc.ser(encP, Y);

  encP[ED25519_KEY_SIZE_BYTES_PARAM - 1] =
      (encP[ED25519_KEY_SIZE_BYTES_PARAM - 1] & 0x7F)
    | ((encX[0] & 0x1) << 7);
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
static inline void recoding_signed_scalar_fold4w4(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r)
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
static inline void query_table_fold4w4(Point_precmp_4way *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits)
{
	const __m256i _P[10] = {
			SET164(0x3ffffed),	SET164(0x1ffffff),
			SET164(0x1ffffff),	SET164(0x3ffffff),
			SET164(0x3ffffff),	SET164(0x1ffffff),
			SET164(0x1ffffff),	SET164(0x3ffffff),
			SET164(0x3ffffff),	SET164(0x1ffffff)
	};
	const __m256i one = SET164(1);
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
static inline void point_multiplication_fold4w4(uint8_t *rB, uint8_t *r)
{
	int i;
	PointXYZT_4way Q;
	Point_precmp_4way P;
	PointXYZT_2w_H0H5 Q0,Q1,Q2,Q3;
	EltFp25519_1w_redradix XX, YY, ZZ, invZZ;
	Element_1w_Buffer_x64 buffer_1w;
	EltFp25519_1w_fullradix X, Y, Z, invZ;
	ALIGN uint64_t K[64];
	ALIGN uint64_t S[64];
	const EltFp25519_4w_redradix one_half = {
			SET164(0x3fffff7),SET164(0x1ffffff),
			SET164(0x1ffffff),SET164(0x3ffffff),
			SET164(0x3ffffff),SET164(0x1ffffff),
			SET164(0x1ffffff),SET164(0x3ffffff),
			SET164(0x3ffffff),SET164(0x0ffffff)
	};

	recoding_signed_scalar_fold4w4(S,K,r);

	Q.Z[0] = SET164(2);
	for(i=1;i<10;i++)
	{
		Q.Z[i] = ZERO;
	}

	query_table_fold4w4(&P, ((uint8_t*)TableSign_w4_12k),S,K);
	Fp25519._4w_red.arith.sub(Q.X,P.addYX,P.subYX);
	Fp25519._4w_red.arith.add(Q.Y,P.addYX,P.subYX);
	new_compressfast2_Element_4w_h0h5(Q.X,Q.Y);
	Fp25519._4w_red.arithex.intmul(Q.T,Q.X,Q.Y);						Fp25519._4w_red.arithex.compress(Q.T);
	Fp25519._4w_red.arithex.intmul(Q.T,Q.T,(argEltFp25519_4w_redradix)one_half);	Fp25519._4w_red.arithex.compress(Q.T);

	for(i=1;i<16;i++)
	{
		query_table_fold4w4(&P, ((uint8_t*)TableSign_w4_12k)+768*i,S+4*i,K+4*i);
		_4way_mixadd_25519(&Q, &P);
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

	Fp25519._2w_red.arithex.deinter(XX,YY,Q0.XY);
	Fp25519._2w_red.arithex.deinter(invZZ,ZZ,Q0.TZ);
	reduced2full_radix(X,XX);
	reduced2full_radix(Y,YY);
	reduced2full_radix(Z,ZZ);

	Fp25519._1w_full.arith.inv(invZ,Z);
	Fp25519._1w_full.arith.mul(X,X,invZ);
	Fp25519._1w_full.arith.mul((uint64_t*)rB,Y,invZ);
	fred_EltFp25519_1w_x64(X);
	fred_EltFp25519_1w_x64((uint64_t*)rB);

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
static inline void recoding_signed_scalar_fold2w4_ed25519(
    uint64_t *list_signs,
    uint64_t *list_digits,
    uint8_t *r) {
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
	list_digits[64] = carry; /*This is always equal to 0 iff r < 2**252 */
}

/**
 * Query table
 * @param P
 * @param table
 * @param secret_signs
 * @param secret_digits
 */
static inline void query_table_fold2w4_ed25519(
	Point_precmp_4way *P,
	uint64_t * secret_signs,
	uint64_t * secret_digits,
	uint64_t index_table) {
	const __m256i _P[NUM_DIGITS_FP25519] = {
			SET164(0x3ffffed),	SET164(0x1ffffff),
			SET164(0x1ffffff),	SET164(0x3ffffff),
			SET164(0x3ffffff),	SET164(0x1ffffff),
			SET164(0x1ffffff),	SET164(0x3ffffff),
			SET164(0x3ffffff),	SET164(0x1ffffff)
	};
	const __m256i one = SET164(1);
	int i,j;
	__m256i mask[8+1];
	__m256i P_addYX[4], P_subYX[4], P__2dYX[4];
	__m256i digits = LOAD(secret_digits);
	__m256i signs  = LOAD(secret_signs);
	__m256i iiii = ZERO;
	uint64_t * p64Table = (uint64_t*) (((uint8_t*)TableSign_w4_24k)+1536*index_table);

	_mm_prefetch( (const char *) p64Table,_MM_HINT_T0);

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
			__m256i addYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)p64Table + 24*j + 3*i + 0));
			__m256i subYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)p64Table + 24*j + 3*i + 1));
			__m256i _2dYX = _mm256_broadcastsi128_si256(_mm_load_si128((__m128i*)p64Table + 24*j + 3*i + 2));

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
static inline void unpack_2w_Element_4w_h0h5(argElement_2w pA, argElement_2w pB, argElement_4w A) {
  pA[0] = UPKL64(A[0], A[1]);
  pB[0] = UPKH64(A[0], A[1]);
  pA[1] = UPKL64(A[2], A[3]);
  pB[1] = UPKH64(A[2], A[3]);
  pA[2] = UPKL64(A[4], A[5]);
  pB[2] = UPKH64(A[4], A[5]);
  pA[3] = UPKL64(A[6], A[7]);
  pB[3] = UPKH64(A[6], A[7]);
  pA[4] = UPKL64(A[8], A[9]);
  pB[4] = UPKH64(A[8], A[9]);
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
static inline void join_points(PointXYZT_2w_H0H5 *T0, PointXYZT_4way *Q) {
  int i;
  PointXYZT_2way R0, R1;
  PointXYZT_2w_H0H5 T1;

  unpack_2w_Element_4w_h0h5(R0.X, R1.X, Q->X);
  unpack_2w_Element_4w_h0h5(R0.Y, R1.Y, Q->Y);
  unpack_2w_Element_4w_h0h5(R0.Z, R1.Z, Q->Z);
  unpack_2w_Element_4w_h0h5(R0.T, R1.T, Q->T);

  _2way_fulladd(&R0, &R1);

  for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
    T0->XY[i] = PERM128(R0.X[i], R0.Y[i], 0x20);
    T1.XY[i] = PERM128(R0.X[i], R0.Y[i], 0x31);
    T0->TZ[i] = PERM128(R0.T[i], R0.Z[i], 0x20);
    T1.TZ[i] = PERM128(R0.T[i], R0.Z[i], 0x31);
  }

  _1way_doubling_2w_H0H5(&T1);
  _1way_doubling_2w_H0H5(&T1);
  _1way_doubling_2w_H0H5(&T1);
  _1way_doubling_2w_H0H5(&T1);

  _1way_fulladd_2w_H0H5(T0, &T1);
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
static inline void point_multiplication_fold2w4_ed25519(PointXYZT_2w_H0H5 *rB, uint8_t *r) {
  int i=0;
  PointXYZT_4way Q;
  Point_precmp_4way P;
  ALIGN uint64_t K[64 + 3];
  ALIGN uint64_t S[64 + 3];
  const EltFp25519_4w_redradix one_half = {
      SET164(0x3fffff7), SET164(0x1ffffff),
      SET164(0x1ffffff), SET164(0x3ffffff),
      SET164(0x3ffffff), SET164(0x1ffffff),
      SET164(0x1ffffff), SET164(0x3ffffff),
      SET164(0x3ffffff), SET164(0x0ffffff)
  };

  recoding_signed_scalar_fold2w4_ed25519(S, K, r);

  Fp25519._4w_red.arith.misc.zero(Q.Z);
  Q.Z[0] = SET164(2);

  query_table_fold2w4_ed25519(&P, S, K, 0);

  Fp25519._4w_red.arith.sub(Q.X, P.addYX, P.subYX);
  Fp25519._4w_red.arith.add(Q.Y, P.addYX, P.subYX);
  Fp25519._4w_red.arithex.new_compressfast2(Q.X, Q.Y);
  Fp25519._4w_red.arith.mul(Q.T, Q.X, Q.Y);
  Fp25519._4w_red.arith.mul(Q.T, Q.T, (argElement_4w) one_half);

  for (i = 1; i < 16; i++) {
    query_table_fold2w4_ed25519(&P, S + 4 * i, K + 4 * i, i);
    _4way_mixadd_25519(&Q, &P);
  }

  join_points(rB, &Q);
}
#else
#error Define symbol LOOKUP_TABLE_SIZE with LUT_12KB or LUT_24KB.
#endif

/**
 * Given:
 *      STR_BYTES r
 * Computes
 *      rB = [r]B
 * Ensuring that B is the generator of Ed25519.
 * @param rB
 * @param r
 */
static inline void point_multiplication_ed25519(PointXYZT_2w_H0H5 *rB, uint8_t *r) {
#if LOOKUP_TABLE_SIZE == LUT_12KB
  point_multiplication_fold4w4_ed25519(rB,r);
#elif LOOKUP_TABLE_SIZE == LUT_24KB
  point_multiplication_fold2w4_ed25519(rB, r);
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
static inline int point_decoding_ed25519(
    PointXYTZ_4w_H0H5 *P_4w,
    PointXYZT_2w_H0H5 *P_2w,
    uint8_t *A) {
  EltFp25519_1w_fullradix param_curve_d = {
      0x75eb4dca135978a3,
      0x00700a4d4141d8ab,
      0x8cc740797779e898,
      0x52036cee2b6ffe73
  };
  EltFp25519_1w_fullradix X, Y, T, u, v;
  EltFp25519_1w_fullradix one = {0};
  uint8_t bit_X;

  one[0] = 1;
  bit_X = A[31] >> 7;

  Fp25519._1w_full.arith.misc.copy(u, (argElement_1w) A);
  u[3] &= ((uint64_t) 1 << 63) - 1;
  Fp25519._1w_full.arith.misc.copy(Y, u);
  /*  u = y^2    */
  Fp25519._1w_full.arith.sqr(u);
  /*  v = dy^2   */
  Fp25519._1w_full.arith.mul(v, u, param_curve_d);
  /*  u =  y^2-1 */
  Fp25519._1w_full.arith.sub(u, u, one);
  /*  v = dy^2+1 */
  Fp25519._1w_full.arith.add(v, v, one);
  /*  X = sqrt(u/v) */
  invsqrt_Fp255_1w_fullradix(X, u, v);
  /*  Checking whether
   *  X is positive or negative
   */
  modp_Fp255_1w_fullradix(X);
  if (bit_X == (X[0] & 0x1)) {
    Fp25519._1w_full.arith.neg(X);
  }
  /* Point represented in extended coordinates */
  Fp25519._1w_full.arith.mul(T, X, Y);

  EltFp25519_1w_redradix XX, YY, TT, ONE;
  Fp25519._1w_red.arith.misc.unser(XX, (uint8_t *) X);
  Fp25519._1w_red.arith.misc.unser(YY, (uint8_t *) Y);
  Fp25519._1w_red.arith.misc.unser(TT, (uint8_t *) T);
  Fp25519._1w_red.arith.misc.unser(ONE, (uint8_t *) one);

  if (P_2w != NULL) {
    Fp25519._2w_red.arithex.inter(P_2w->XY, XX, YY);
    Fp25519._2w_red.arithex.inter(P_2w->TZ, TT, ONE);
  }
  if (P_4w != NULL) {
    zip_Fp255_4w_redradix(P_4w->XYTZ, XX, YY, TT, ONE);
  }
  return 0;
}

#define is_greater_equal_one(r) ((r[3]!=0)||(r[2]!=0)||(r[1]!=0)||(r[0]>=(uint64_t)1))
#define is_odd(r) ((r[0]&0x1)==1)
#define div_2(r)                \
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
static inline int wnaf_256bits(int8_t *K, const uint8_t *p8_r, int w) {
	int i = 0;
	int64_t value;
	ALIGN uint64_t r[4];
	r[0] = ((uint64_t *)p8_r)[0];
	r[1] = ((uint64_t *)p8_r)[1];
	r[2] = ((uint64_t *)p8_r)[2];
	r[3] = ((uint64_t *)p8_r)[3];

	while(is_greater_equal_one(r))
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
		div_2(r);
		i++;
	}
	return i;
}

static const ALIGN uint64_t CONST_2_to_35P_4w[4*NUM_DIGITS_FP25519] = {
		0xfffffb400000000,0xfffffb400000000,0xfffffb400000000,0xfffffb400000000,
		0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,
		0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,
		0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,
		0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,
		0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,
		0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,
		0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,
		0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,0xffffffc00000000,
		0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000,0x7fffffc00000000
};
/**
 * Computes the doubling of ONE point
 * stored in P = <X,Y,T,Z>
 *
 * @param P
 */
static inline void _1way_mixadd_4w_H0H5(PointXYTZ_4w_H0H5 * Q, PointXYTZ_precompute_4w_H0H5 * P) {
  const __m256i CONST_2P_00_00_00[NUM_DIGITS_FP25519] = {
      SET64(0, 0, 0, 0x7ffffda), SET64(0, 0, 0, 0x3fffffe),
      SET64(0, 0, 0, 0x3fffffe), SET64(0, 0, 0, 0x7fffffe),
      SET64(0, 0, 0, 0x7fffffe), SET64(0, 0, 0, 0x3fffffe),
      SET64(0, 0, 0, 0x3fffffe), SET64(0, 0, 0, 0x7fffffe),
      SET64(0, 0, 0, 0x7fffffe), SET64(0, 0, 0, 0x3fffffe)
  };
  int i;
  EltFp25519_4w_redradix ABCD, subYX_addYX_T_Z;
  argElement_4w _2_to_34P = (argElement_4w) CONST_2_to_35P_4w;
  argElement_4w _2P_00_00_00 = (argElement_4w) CONST_2P_00_00_00;
  argElement_4w XYTZ = Q->XYTZ;
  argElement_4w EGHF = subYX_addYX_T_Z;
  argElement_4w FHEG = ABCD;

  const __m256i mask_subadd1 = SET64(0, 0, 0, -1);
  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    __m256i M = SHUF32(XYTZ[i], 0x4E);
    __m256i N = XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    subYX_addYX_T_Z[i] = BLEND32(subYX_addYX, XYTZ[i], 0xF0);
  }
  /* [A|B|C|D] = [ (y1-x1)*(y2-x2) | (y1+x1)*(y2+x2) | T*2dT | Z*2Z ] */
  Fp25519._4w_red.arithex.intmul(ABCD, subYX_addYX_T_Z, P->subYX_addYX_2dT_2Z);

  const __m256i mask_subadd2 = SET64(-1, 0, 0, -1);
  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    __m256i BDBD = PERM64(ABCD[i], 0xDD);
    __m256i ACAC = PERM64(ABCD[i], 0x88);
    EGHF[i] = ADD(ADD(BDBD, _2_to_34P[i]), SUB(XOR(ACAC, mask_subadd2), mask_subadd2));
  }
  Fp25519._4w_red.arithex.compress(EGHF);

  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    FHEG[i] = PERM64(EGHF[i], 0x4B);
  }
  Fp25519._4w_red.arithex.intmul(XYTZ, EGHF, FHEG);
  Fp25519._4w_red.arithex.compress(XYTZ);
}

/**
 * Computes the doubling of ONE point
 * stored in P = <X,Y,T,Z>
 *
 * @param P
 */
static inline void _1way_doubling_4w_H0H5(PointXYTZ_4w_H0H5 * P) {
  int i;
  EltFp25519_4w_redradix ABEC, XYZZ;
  argElement_4w _2_to_34P = (argElement_4w) CONST_2_to_35P_4w;
  argElement_4w XYTZ = P->XYTZ;
  argElement_4w FHEG = ABEC;
  argElement_4w EGHF = XYZZ;

  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    XYZZ[i] = PERM64(XYTZ[i], 0xF4);
  }
  /* [A|B|E/2|C/2] = [X^2 | Y^2 | TZ | Z^2 ] */
  Fp25519._4w_red.arithex.intmul(ABEC, XYTZ, XYZZ);

  /* [-F|-H|E|G] = [ -G+C | A+B | 2XY | -A+B ] */
  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    __m256i CBEB = PERM64(ABEC[i], 0x67);
    __m256i CAE0 = BLEND32(PERM64(ABEC[i], 0xE3), ZERO, 0xC0);
    __m256i A000 = BLEND32(ABEC[i], ZERO, 0xFC);
    __m256i B00A = BLEND32(PERM64(ABEC[i], 0x25), ZERO, 0x3C);
    FHEG[i] = ADD(CBEB, ADD(CAE0, ADD(A000, SUB(_2_to_34P[i], B00A))));
  }
  Fp25519._4w_red.arithex.compress(FHEG);

  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    EGHF[i] = PERM64(FHEG[i], 0x1E);
  }
  Fp25519._4w_red.arithex.intmul(XYTZ, EGHF, FHEG);
  Fp25519._4w_red.arithex.compress(XYTZ);
}

/**
 *
 * @param table
 * @param P
 */
static inline void precompute_points_2w_ed25519(PointXYZT_precompute_2w_H0H5 *table, PointXYZT_2w_H0H5 *P) {
  const ALIGN uint64_t param_curve_2d_02[2*NUM_DIGITS_FP25519] = {
      0x2b2f159, 0x0f3d130, 0x2, 0x0,
      0x1a6e509, 0x3407977, 0x0, 0x0,
      0x22add7a, 0x19ce331, 0x0, 0x0,
      0x0d4141d, 0x1c56dff, 0x0, 0x0,
      0x0038052, 0x0901b67, 0x0, 0x0
  };
  const int num = (1 << (OMEGA_DYNAMIC - 2));

  int i = 0;
  PointXYZT_2w_H0H5 iP;
  PointXYZT_precompute_2w_H0H5 _2P_precmp;

  Fp25519._2w_red.arith.misc.copy(iP.XY, P->XY);
  Fp25519._2w_red.arith.misc.copy(iP.TZ, P->TZ);
  _1way_doubling_2w_H0H5(P);

  subadd_Fp255_2w_redradix(_2P_precmp.subaddYX, P->XY, 1);
  Fp25519._2w_red.arithex.intmul(_2P_precmp._2dT_2Z, P->TZ, (argElement_2w) param_curve_2d_02);
  Fp25519._2w_red.arithex.compress2(_2P_precmp.subaddYX, _2P_precmp._2dT_2Z);

  subadd_Fp255_2w_redradix(table[i].subaddYX, iP.XY, 1);
  Fp25519._2w_red.arithex.intmul(table[i]._2dT_2Z, iP.TZ, (argElement_2w) param_curve_2d_02);
  Fp25519._2w_red.arithex.compress2(table[i].subaddYX, table[i]._2dT_2Z);
  for (i = 1; i < num; i++) {
    _1way_mixadd_2w_H0H5(&iP, &_2P_precmp);
    subadd_Fp255_2w_redradix(table[i].subaddYX, iP.XY, 1);
    Fp25519._2w_red.arithex.intmul(table[i]._2dT_2Z, iP.TZ, (argElement_2w) param_curve_2d_02);
    Fp25519._2w_red.arithex.compress2(table[i].subaddYX, table[i]._2dT_2Z);
  }
}

/**
 *
 * @param P
 * @param index
 */
static inline void read_point_2w_ed25519(PointXYZT_precompute_2w_H0H5 *P, int8_t index) {
  const EltFp25519_1w_redradix two = {
      0x2, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0
  };
  int8_t abs_index_r = index > 0 ? index : -index;
  abs_index_r >>= 1;

  const uint64_t *ptr_point = &TableVerif_static_w7_Ed25519[3 * NUM_DIGITS_FP25519_X64 * abs_index_r];
  EltFp25519_1w_redradix add, sub, _2dT;

  Fp25519._1w_red.arith.misc.unser(add, (uint8_t *) (ptr_point + 0 * NUM_DIGITS_FP25519_X64));
  Fp25519._1w_red.arith.misc.unser(sub, (uint8_t *) (ptr_point + 1 * NUM_DIGITS_FP25519_X64));
  Fp25519._1w_red.arith.misc.unser(_2dT, (uint8_t *) (ptr_point + 2 * NUM_DIGITS_FP25519_X64));

  if (index < 0)/*negative*/
  {
    Fp25519._1w_red.arith.neg(_2dT);
    Fp25519._1w_red.arithex.compressfast(_2dT);
    Fp25519._2w_red.arithex.inter(P->subaddYX, add, sub);
  } else {
    Fp25519._2w_red.arithex.inter(P->subaddYX, sub, add);
  }
  Fp25519._2w_red.arithex.inter(P->_2dT_2Z, _2dT, (argElement_1w) two);
}

/**
 *
 * @param sB_hA
 * @param s
 * @param h
 * @param A
 */
static inline void double_point_mult_2w_ed25519(
    uint8_t *sB_hA,
    const uint8_t *s,
    uint8_t *h,
    PointXYZT_2w_H0H5 *A) {
  int i;
  int8_t wnaf_r[256] = {0};
  int8_t wnaf_h[256] = {0};

  PointXYZT_2w_H0H5 Q;
  PointXYZT_precompute_2w_H0H5 tableA[1 << (OMEGA_DYNAMIC - 2)];

  int l_r = wnaf_256bits(wnaf_r, s, OMEGA_STATIC);
  int l_h = wnaf_256bits(wnaf_h, h, OMEGA_DYNAMIC);
  int l = l_r > l_h ? l_r : l_h;

  precompute_points_2w_ed25519(tableA, A);
  Fp25519._2w_red.arith.misc.zero(Q.XY);
  Fp25519._2w_red.arith.misc.zero(Q.TZ);
  Q.XY[0] = SET64(0, 1, 0, 0);
  Q.TZ[0] = SET64(0, 1, 0, 0);

  for (i = l - 1; i >= 0; i--) {
    _1way_doubling_2w_H0H5(&Q);
    /* known point addition */
    if (wnaf_r[i] != 0) {
      PointXYZT_precompute_2w_H0H5 P;
      read_point_2w_ed25519(&P, wnaf_r[i]);
      _1way_mixadd_2w_H0H5(&Q, &P);
    }
    /* unknown point addition */
    if (wnaf_h[i] != 0) {
      PointXYZT_precompute_2w_H0H5 *P = NULL;
      uint8_t abs_index_h = wnaf_h[i] > 0 ? wnaf_h[i] : -wnaf_h[i];
      abs_index_h >>= 1;
      P = &tableA[abs_index_h];
      if (wnaf_h[i] < 0) {
        int ii = 0;
        PointXYZT_precompute_2w_H0H5 _P;
        for (ii = 0; ii < (NUM_DIGITS_FP25519 / 2); ii++) {
          _P.subaddYX[ii] = PERM64(P->subaddYX[ii], 0x4E);
        }
        Fp25519._2w_red.arith.ngz(_P._2dT_2Z, P->_2dT_2Z);
        Fp25519._2w_red.arithex.compressfast(_P._2dT_2Z);
        _1way_mixadd_2w_H0H5(&Q, &_P);
      } else {
        _1way_mixadd_2w_H0H5(&Q, P);
      }
    }
  }

  point_encoding_ed25519(sB_hA, &Q);
}

static inline void precompute_points_4w_ed25519(PointXYTZ_precompute_4w_H0H5 *table, PointXYTZ_4w_H0H5 *P) {
  const __m256i CONST_param_curve_1_1_2d_02[NUM_DIGITS_FP25519] = {
      SET64(2, 0x2b2f159, 1, 1), SET64(0, 0x0f3d130, 0, 0),
      SET64(0, 0x1a6e509, 0, 0), SET64(0, 0x3407977, 0, 0),
      SET64(0, 0x22add7a, 0, 0), SET64(0, 0x19ce331, 0, 0),
      SET64(0, 0x0d4141d, 0, 0), SET64(0, 0x1c56dff, 0, 0),
      SET64(0, 0x0038052, 0, 0), SET64(0, 0x0901b67, 0, 0)
  };

  const __m256i CONST_2P_00_00_00_H0H5[NUM_DIGITS_FP25519] = {
      SET64(0, 0, 0, 0x7ffffda), SET64(0, 0, 0, 0x3fffffe),
      SET64(0, 0, 0, 0x3fffffe), SET64(0, 0, 0, 0x7fffffe),
      SET64(0, 0, 0, 0x7fffffe), SET64(0, 0, 0, 0x3fffffe),
      SET64(0, 0, 0, 0x3fffffe), SET64(0, 0, 0, 0x7fffffe),
      SET64(0, 0, 0, 0x7fffffe), SET64(0, 0, 0, 0x3fffffe)
  };

  argElement_4w _2P_00_00_00 = (argElement_4w) CONST_2P_00_00_00_H0H5;
  PointXYTZ_4w_H0H5 iP;
  const int num = (1 << (OMEGA_DYNAMIC - 2));
  int i = 0, j = 0;
  const __m256i mask_subadd1 = SET64(0, 0, 0, -1);

  Fp25519._4w_red.arith.misc.copy(iP.XYTZ, P->XYTZ);

  _1way_doubling_4w_H0H5(P);

  PointXYTZ_precompute_4w_H0H5 _2P_precmp;
  Fp25519._4w_red.arithex.intmul(_2P_precmp.subYX_addYX_2dT_2Z, P->XYTZ, (argElement_4w) CONST_param_curve_1_1_2d_02);
  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    __m256i M = SHUF32(P->XYTZ[i], 0x4E);
    __m256i N = P->XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    _2P_precmp.subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, _2P_precmp.subYX_addYX_2dT_2Z[i], 0xF0);
  }
  Fp25519._4w_red.arithex.compress(_2P_precmp.subYX_addYX_2dT_2Z);

  Fp25519._4w_red.arithex.intmul(table[0].subYX_addYX_2dT_2Z, iP.XYTZ, (argElement_4w) CONST_param_curve_1_1_2d_02);
  for (i = 0; i < NUM_DIGITS_FP25519; i++) {
    __m256i M = SHUF32(iP.XYTZ[i], 0x4E);
    __m256i N = iP.XYTZ[i];
    __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
    table[0].subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, table[0].subYX_addYX_2dT_2Z[i], 0xF0);
  }
  Fp25519._4w_red.arithex.compress(table[0].subYX_addYX_2dT_2Z);

  for (j = 1; j < num; j++) {
    _1way_mixadd_4w_H0H5(&iP, &_2P_precmp);
    Fp25519._4w_red.arithex.intmul(table[j].subYX_addYX_2dT_2Z, iP.XYTZ, (argElement_4w) CONST_param_curve_1_1_2d_02);
    for (i = 0; i < NUM_DIGITS_FP25519; i++) {
      __m256i M = SHUF32(iP.XYTZ[i], 0x4E);
      __m256i N = iP.XYTZ[i];
      __m256i subYX_addYX = ADD(ADD(M, _2P_00_00_00[i]), SUB(XOR(N, mask_subadd1), mask_subadd1));
      table[j].subYX_addYX_2dT_2Z[i] = BLEND32(subYX_addYX, table[j].subYX_addYX_2dT_2Z[i], 0xF0);
    }
    Fp25519._4w_red.arithex.compress(table[j].subYX_addYX_2dT_2Z);
  }
}

static inline void read_point_4w_ed25519(PointXYTZ_precompute_4w_H0H5 *P, int8_t index) {
  const EltFp25519_1w_redradix two = {
      0x2, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0
  };
  EltFp25519_1w_redradix add, sub, _2dT;
  uint8_t abs_index_r = index > 0 ? index : -index;
  abs_index_r >>= 1;

  const uint64_t *ptr_point = &TableVerif_static_w7_Ed25519[3 * NUM_DIGITS_FP25519_X64 * abs_index_r];

  Fp25519._1w_red.arith.misc.unser(add, (uint8_t *) (ptr_point + 0 * NUM_DIGITS_FP25519_X64));
  Fp25519._1w_red.arith.misc.unser(sub, (uint8_t *) (ptr_point + 1 * NUM_DIGITS_FP25519_X64));
  Fp25519._1w_red.arith.misc.unser(_2dT,(uint8_t *) (ptr_point + 2 * NUM_DIGITS_FP25519_X64));

  if (index < 0) {
    Fp25519._1w_red.arith.neg(_2dT);
    Fp25519._1w_red.arithex.compress(_2dT);
    zip_Fp255_4w_redradix(P->subYX_addYX_2dT_2Z, add, sub, _2dT, (argElement_1w) two);
  } else {
    zip_Fp255_4w_redradix(P->subYX_addYX_2dT_2Z, sub, add, _2dT, (argElement_1w) two);
  }
}

static inline void unzip_2w_Element_4w_h0h5(argElement_2w pA,argElement_2w pB,argElement_4w A) {
  int i = 0;
  for (i = 0; i < (NUM_DIGITS_FP25519/2); i++) {
    __m256i pA0 = UPKL64(A[2 * i], A[2 * i + 1]);
    __m256i pB0 = UPKH64(A[2 * i], A[2 * i + 1]);
    pA[i] = PERM128(pA0, pB0, 0x20);
    pB[i] = PERM128(pA0, pB0, 0x31);
  }
}

static inline void double_point_mult_4w_ed25519(
    uint8_t *sB_hA,
    const uint8_t *s,
    uint8_t *h,
    PointXYTZ_4w_H0H5 *A) {
  int i;
  int8_t wnaf_r[260] = {0};
  int8_t wnaf_h[260] = {0};

  PointXYTZ_4w_H0H5 Q;
  PointXYZT_2w_H0H5 QQ;
  PointXYTZ_precompute_4w_H0H5 tableA[1 << (OMEGA_DYNAMIC - 2)];

  int l_r = wnaf_256bits(wnaf_r, s, OMEGA_STATIC);
  int l_h = wnaf_256bits(wnaf_h, h, OMEGA_DYNAMIC);
  int l = l_r > l_h ? l_r : l_h;

  precompute_points_4w_ed25519(tableA, A);

  Fp25519._4w_red.arith.misc.zero(Q.XYTZ);
  Q.XYTZ[0] = SET64(1, 0, 1, 0);

  for (i = l - 1; i >= 0; i--) {
    _1way_doubling_4w_H0H5(&Q);
    /* known point addition */
    if (wnaf_r[i] != 0) {
      PointXYTZ_precompute_4w_H0H5 P;
      read_point_4w_ed25519(&P, wnaf_r[i]);
      _1way_mixadd_4w_H0H5(&Q, &P);
    }
    /* unknown point addition */
    if (wnaf_h[i] != 0) {
      PointXYTZ_precompute_4w_H0H5 *P = NULL;
      uint8_t abs_index_h = wnaf_h[i] > 0 ? wnaf_h[i] : -wnaf_h[i];
      abs_index_h >>= 1;
      P = &tableA[abs_index_h];
      if (wnaf_h[i] < 0) {
        PointXYTZ_precompute_4w_H0H5 _P;
        Fp25519._4w_red.arith.ngz(_P.subYX_addYX_2dT_2Z, P->subYX_addYX_2dT_2Z);
        Fp25519._4w_red.arithex.compressfast(_P.subYX_addYX_2dT_2Z);
        _1way_mixadd_4w_H0H5(&Q, &_P);
      } else {
        _1way_mixadd_4w_H0H5(&Q, P);
      }
    }
  }
  unzip_2w_Element_4w_h0h5(QQ.XY, QQ.TZ, Q.XYTZ);

  point_encoding_ed25519(sB_hA, &QQ);
}

#else
extern int version;
#endif /* ED255 */
