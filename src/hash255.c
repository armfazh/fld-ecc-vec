/**
 * Copyright 2029 Armando Faz Hernández
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

#if defined(HASH255)

#include <string.h>

#include "edwards255.h"
#include "faz_fp_avx2.h"
#include "faz_hash_avx2.h"
#include "sha512.h"
#include "simd_avx2.h"

typedef ALIGN uint8_t Digest[512 / 64];

static void hash_to_field(argElement_1w u, uint8_t *msg, size_t mlen) {
  Digest h0_msg;
  sph_sha512_context h0;
  sph_sha512_init(&h0);
  sph_sha512(&h0, msg, mlen);
  sph_sha512_close(&h0, h0_msg);
  Fp25519._1w_full.arith.misc.zero(u);
}

static void map_to_curve_elligator2_curve25519(argElement_1w xn,
                                               argElement_1w xd,
                                               argElement_1w yn,
                                               argElement_1w yd,
                                               argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  int e2 = 0, e3 = 0, e4 = 0;
  EltFp25519_1w_fullradix c2 = /* 2^((p+3)/8) */ {
      0xc4ee1b274a0ea0b1,
      0x2f431806ad2fe478,
      0x2b4d00993dfbd7a7,
      0x2b8324804fc1df0b,
  };
  EltFp25519_1w_fullradix c3 = /* sqrt_minus_one */ {
      0xc4ee1b274a0ea0b0,
      0x2f431806ad2fe478,
      0x2b4d00993dfbd7a7,
      0x2b8324804fc1df0b,
  };
  EltFp25519_1w_fullradix one = {1};
  EltFp25519_1w_fullradix A = {486662};
  EltFp25519_1w_fullradix tv1, tv2, gx1, x1n, gxd, x2n, y1, y21, y11, gx2, y22,
      y2, y3, y;

  fp.misc.copy(tv1, u);              //
  fp.sqr(tv1);                       // 1.  tv1 = u^2
  fp.add(tv1, tv1, tv1);             // 2.  tv1 = 2 * tv1
  fp.add(xd, tv1, one);              // 3.   xd = tv1 + 1
  fp.misc.copy(x1n, A);              //
  fp.neg(x1n);                       // 4.  x1n = -J
  fp.misc.copy(tv2, xd);             //
  fp.sqr(tv2);                       // 5.  tv2 = xd^2
  fp.mul(gxd, tv2, xd);              // 6.  gxd = tv2 * xd
  fp.mul(gx1, A, tv1);               // 7.  gx1 = J * tv1
  fp.mul(gx1, gx1, x1n);             // 8.  gx1 = gx1 * x1n
  fp.add(gx1, gx1, tv2);             // 9.  gx1 = gx1 + tv2
  fp.mul(gx1, gx1, x1n);             // 10. gx1 = gx1 * x1n
  fp.misc.copy(y1, gx1);             //
  fp.srt(y1, gxd);                   // 11. tv3 = gxd^2
                                     // 12. tv2 = tv3^2
                                     // 13. tv3 = tv3 * gxd
                                     // 14. tv3 = tv3 * gx1
                                     // 15. tv2 = tv2 * tv3
                                     // 16. y11 = tv2^c4
                                     // 17. y11 = y11 * tv3
                                     // 18. y12 = y11 * c3
                                     // 19. tv2 = y11^2
                                     // 20. tv2 = tv2 * gxd
                                     // 21.  e1 = tv2 == gx1
                                     // 22.  y1 = CMOV(y12, y11, e1)
  fp.mul(x2n, x1n, tv1);             // 23. x2n = x1n * tv1
  fp.mul(y21, y11, u);               // 24. y21 = y11 * u
  fp.mul(y21, y21, c2);              // 25. y21 = y21 * c2
  fp.mul(y22, y21, c3);              // 26. y22 = y21 * c3
  fp.mul(gx2, gx1, tv1);             // 27. gx2 = gx1 * tv1
  fp.misc.copy(tv2, y21);            //
  fp.sqr(tv2);                       // 28. tv2 = y21^2
  fp.mul(tv2, tv2, gxd);             // 29. tv2 = tv2 * gxd
  e2 = fp.misc.cmp(tv2, gx2);        // 30.  e2 = tv2 == gx2
  fp.misc.cmov(e2, y2, y22, y21);    // 31.  y2 = CMOV(y22, y21, e2)
  fp.misc.copy(tv2, y1);             //
  fp.sqr(tv2);                       // 32. tv2 = y1^2
  fp.mul(tv2, tv2, gxd);             // 33. tv2 = tv2 * gxd
  e3 = fp.misc.cmp(tv2, gx1);        // 34.  e3 = tv2 == gx1
  fp.misc.cmov(e3, xn, x2n, x1n);    // 35.  xn = CMOV(x2n, x1n, e3)
  fp.misc.cmov(e3, y, y2, y1);       // 36.   y = CMOV(y2, y1, e3)
  e4 = fp.sgn(y);                    // 37.  e4 = sgn0(y) == 1
  fp.misc.copy(y3, y);               //
  fp.neg(y3);                        //
  fp.misc.cmov(e3 ^ e4, yn, y, y3);  // 38.   y = CMOV(y, -y, e3 XOR e4)
  fp.misc.copy(yd, one);
}

static void map_to_curve_elligator2_edwards25519(argElement_1w xn,
                                                 argElement_1w xd,
                                                 argElement_1w yn,
                                                 argElement_1w yd,
                                                 argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix zero = {0};
  EltFp25519_1w_fullradix one = {1};
  EltFp25519_1w_fullradix c1 = /*c1 = sqrt(-486664), st sgn(c1)==0. */ {
      0xcc6e04aaff457e06,
      0xc5a1d3d14b7d1a82,
      0xd27b08dc03fc4f7e,
      0xf26edf460a006bb,
  };

  EltFp25519_1w_fullradix xMn, xMd, yMn, yMd, tv1;
  int e = 0;
  // 1.  (xMn, xMd, yMn, yMd) = map_to_curve_elligator2_curve25519(u)
  map_to_curve_elligator2_curve25519(xMn, xMd, yMn, yMd, u);
  fp.mul(xn, xMn, yMd);           // 2.  xn = xMn * yMd
  fp.mul(xn, xn, c1);             // 3.  xn = xn * c1
  fp.mul(xd, xMd, yMn);           // 4.  xd = xMd * yMn
  fp.sub(yn, xMn, xMd);           // 5.  yn = xMn - xMd
  fp.add(yd, xMn, xMd);           // 6.  yd = xMn + xMd
  fp.mul(tv1, xd, yd);            // 7. tv1 = xd * yd
  e = fp.misc.cmp(tv1, zero);     // 8.   e = tv1 == 0
  fp.misc.cmov(e, xn, xn, zero);  // 9.  xn = CMOV(xn, 0, e)
  fp.misc.cmov(e, xd, xd, one);   // 10. xd = CMOV(xd, 1, e)
  fp.misc.cmov(e, yn, yn, one);   // 11. yn = CMOV(yn, 1, e)
  fp.misc.cmov(e, yd, yd, one);   // 12. yd = CMOV(yd, 1, e)
}

static void map_to_curve(Point255 *P, argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix xn, xd, yn, yd;
  map_to_curve_elligator2_edwards25519(xn, xd, yn, yd, u);
  fp.mul(P->X, xn, yd);
  fp.mul(P->Y, yn, xd);
  fp.mul(P->T, xn, yn);
  fp.mul(P->Z, xd, yd);
}

static inline void clear_cofactor(Point255 *P, Point255 *Q) {
  int i = 0;
  PointXYTZ_4w_H0H5 Q0;
  EltFp25519_1w_redradix X, Y, T, Z;

  Fp25519._1w_red.arith.misc.unser(X, (uint8_t *)Q->X);
  Fp25519._1w_red.arith.misc.unser(Y, (uint8_t *)Q->Y);
  Fp25519._1w_red.arith.misc.unser(T, (uint8_t *)Q->T);
  Fp25519._1w_red.arith.misc.unser(Z, (uint8_t *)Q->Z);
  zip_Fp255_4w_redradix(Q0.XYTZ, Q->X, Q->Y, Q->T, Q->Z);

  for (i = 0; i < 3; i++) {
    _1way_doubling_4w_H0H5(&Q0);
  }

  unzip_Fp255_4w_redradix(X, Y, T, Z, Q0.XYTZ);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P->X, X);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P->Y, Y);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P->T, T);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P->Z, Z);
}

static inline void clear_cofactor_2w(PointXYTZ_2way *Q) {
  int i = 0;

  for (i = 0; i < 3; i++) {
    _2way_doubling(&Q);
  }
}

void h2c25519_x64(Point255 *P, uint8_t *msg, size_t mlen) {
  EltFp25519_1w_fullradix u0;
  Point255 Q0;

  hash_to_field(u0, msg, mlen);
  map_to_curve(&Q0, u0);
  clear_cofactor(P, &Q0);
}

void h2c25519_avx2(Point255 *P, uint8_t *msg, size_t mlen) {
  EltFp25519_1w_fullradix u0;
  Point255 Q0;

  hash_to_field(u0, msg, mlen);
  map_to_curve_2w(&Q0, u0);
  clear_cofactor_2w(&Q0);
}

void print_point(FILE *file, Point255 *P) {
  fprintf(file, "X: ");
  Fp25519._1w_full.arith.misc.print(file, P->X);
  fprintf(file, "Y: ");
  Fp25519._1w_full.arith.misc.print(file, P->Y);
  fprintf(file, "T: ");
  Fp25519._1w_full.arith.misc.print(file, P->T);
  fprintf(file, "Z: ");
  Fp25519._1w_full.arith.misc.print(file, P->Z);
}

#else
extern int version;
#endif /* HASH255 */
