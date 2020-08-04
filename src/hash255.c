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

#include "faz_hash_avx2.h"
#include "multi_hash.h"

typedef ALIGN uint8_t Digest[512 / 8];

void hash_to_field(argElement_1w u, uint8_t prefix, uint8_t *msg,
                                 size_t mlen) {
  Digest h0_msg;
  sph_sha512_context h0;
  sph_sha512_init(&h0);
  sph_sha512(&h0, &prefix, 1);
  sph_sha512(&h0, msg, mlen);
  sph_sha512_close(&h0, h0_msg);
  modular_reduction_ed25519(h0_msg);
  memcpy(u, h0_msg, SIZE_FP25519);
}

void hash_to_field_2w(argElement_1w u0, argElement_1w u1,
                                    uint8_t *msg, size_t mlen) {
  uint8_t *message[2];
  uint8_t *digest[2];
  Digest h0_msg, h1_msg;
  message[0] = (uint8_t *)_mm_malloc(mlen + 1, ALIGN_BYTES);
  message[1] = (uint8_t *)_mm_malloc(mlen + 1, ALIGN_BYTES);
  digest[0] = h0_msg;
  digest[1] = h1_msg;
  memcpy(message[0] + 1, msg, mlen);
  memcpy(message[1] + 1, msg, mlen);
  message[0][0] = 0;
  message[1][0] = 1;

  sha512_2w_avx(message, mlen + 1, digest);

  modular_reduction_ed25519(h0_msg);
  modular_reduction_ed25519(h1_msg);
  memcpy(u0, h0_msg, SIZE_FP25519);
  memcpy(u1, h1_msg, SIZE_FP25519);

  _mm_free(message[0]);
  _mm_free(message[1]);
}

static inline void expoC4(argElement_1w x_p58, argElement_1w x) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix x2, x9;
  argElement_1w Tab[4];
  int counter;

#define sqrn_Fp255_1w_fullradix(A, times) \
  counter = times;                        \
  while (counter-- > 0) {                 \
    sqr_Fp255_1w_fullradix(A);            \
  }

  Tab[0] = x2;
  Tab[1] = x9;
  Tab[2] = x_p58;
  Tab[3] = x;

  fp.misc.copy(Tab[0], x);
  fp.sqr(Tab[0]); /* x^2 */
  /* 0 */
  fp.misc.copy(Tab[1], Tab[0]);
  sqrn_Fp255_1w_fullradix(Tab[1], 2);
  fp.mul(Tab[1], Tab[1], Tab[3]);
  /* 1 */
  fp.mul(Tab[0], Tab[0], Tab[1]);
  /* 2 */
  sqrn_Fp255_1w_fullradix(Tab[0], 1);
  fp.mul(Tab[0], Tab[0], Tab[1]);
  /* 3 */
  fp.misc.copy(Tab[1], Tab[0]);
  sqrn_Fp255_1w_fullradix(Tab[1], 5);
  fp.mul(Tab[1], Tab[1], Tab[0]);
  /* 4 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 5);
  fp.mul(Tab[2], Tab[2], Tab[0]);
  /* 5 */
  fp.misc.copy(Tab[1], Tab[2]);
  sqrn_Fp255_1w_fullradix(Tab[1], 15);
  fp.mul(Tab[1], Tab[1], Tab[2]);
  /* 6 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 30);
  fp.mul(Tab[2], Tab[2], Tab[1]);
  /* 7 */
  fp.misc.copy(Tab[1], Tab[2]);
  sqrn_Fp255_1w_fullradix(Tab[1], 60);
  fp.mul(Tab[1], Tab[1], Tab[2]);
  /* 8 */
  sqrn_Fp255_1w_fullradix(Tab[1], 5);
  fp.mul(Tab[1], Tab[1], Tab[0]);
  /* 9 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 125);
  fp.mul(Tab[2], Tab[2], Tab[1]);
  /* 10 */
  sqrn_Fp255_1w_fullradix(Tab[2], 2);
  fp.mul(Tab[2], Tab[2], Tab[3]);

#undef sqrn_Fp255_1w_fullradix
}

static inline void expoC4_2w(argElement_2w x_p58, argElement_2w x) {
  const Arith_2w fp = Fp25519._2w_red.arith;
  EltFp25519_2w_redradix x2, x9;
  argElement_2w Tab[4];
  int counter;

#define sqrn_Fp255_2w_redradix(A, times) \
  counter = times;                       \
  while (counter-- > 0) {                \
    sqr_Fp255_2w_redradix(A);            \
  }

  Tab[0] = x2;
  Tab[1] = x9;
  Tab[2] = x_p58;
  Tab[3] = x;

  fp.misc.copy(Tab[0], x);
  fp.sqr(Tab[0]); /* x^2 */
  /* 0 */
  fp.misc.copy(Tab[1], Tab[0]);
  sqrn_Fp255_2w_redradix(Tab[1], 2);
  fp.mul(Tab[1], Tab[1], Tab[3]);
  /* 1 */
  fp.mul(Tab[0], Tab[0], Tab[1]);
  /* 2 */
  sqrn_Fp255_2w_redradix(Tab[0], 1);
  fp.mul(Tab[0], Tab[0], Tab[1]);
  /* 3 */
  fp.misc.copy(Tab[1], Tab[0]);
  sqrn_Fp255_2w_redradix(Tab[1], 5);
  fp.mul(Tab[1], Tab[1], Tab[0]);
  /* 4 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_2w_redradix(Tab[2], 5);
  fp.mul(Tab[2], Tab[2], Tab[0]);
  /* 5 */
  fp.misc.copy(Tab[1], Tab[2]);
  sqrn_Fp255_2w_redradix(Tab[1], 15);
  fp.mul(Tab[1], Tab[1], Tab[2]);
  /* 6 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_2w_redradix(Tab[2], 30);
  fp.mul(Tab[2], Tab[2], Tab[1]);
  /* 7 */
  fp.misc.copy(Tab[1], Tab[2]);
  sqrn_Fp255_2w_redradix(Tab[1], 60);
  fp.mul(Tab[1], Tab[1], Tab[2]);
  /* 8 */
  sqrn_Fp255_2w_redradix(Tab[1], 5);
  fp.mul(Tab[1], Tab[1], Tab[0]);
  /* 9 */
  fp.misc.copy(Tab[2], Tab[1]);
  sqrn_Fp255_2w_redradix(Tab[2], 125);
  fp.mul(Tab[2], Tab[2], Tab[1]);
  /* 10 */
  sqrn_Fp255_2w_redradix(Tab[2], 2);
  fp.mul(Tab[2], Tab[2], Tab[3]);

#undef sqrn_Fp255_2w_redradix
}

static inline void elligator2_curve25519(argElement_1w xn, argElement_1w xd,
                                         argElement_1w yn, argElement_1w yd,
                                         argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
  EltFp25519_1w_fullradix one = {1};
  EltFp25519_1w_fullradix A = {486662};
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
  EltFp25519_1w_fullradix tv1, tv2, tv3, gx1, x1n, gxd, x2n, y1, y12, y21, y11,
      gx2, y22, y2, y3, y;

  fp.misc.copy(tv1, u);        //
  fp.sqr(tv1);                 // 1.  tv1 = u^2
  fp.add(tv1, tv1, tv1);       // 2.  tv1 = 2 * tv1
  fp.add(xd, tv1, one);        // 3.   xd = tv1 + 1
  fp.misc.copy(x1n, A);        //
  fp.neg(x1n);                 // 4.  x1n = -J
  fp.misc.copy(tv2, xd);       //
  fp.sqr(tv2);                 // 5.  tv2 = xd^2
  fp.mul(gxd, tv2, xd);        // 6.  gxd = tv2 * xd
  fp.mul(gx1, A, tv1);         // 7.  gx1 = J * tv1
  fp.mul(gx1, gx1, x1n);       // 8.  gx1 = gx1 * x1n
  fp.add(gx1, gx1, tv2);       // 9.  gx1 = gx1 + tv2
  fp.mul(gx1, gx1, x1n);       // 10. gx1 = gx1 * x1n
  fp.misc.copy(tv3, gxd);      //
  fp.sqr(tv3);                 // 11. tv3 = gxd^2
  fp.misc.copy(tv2, tv3);      //
  fp.sqr(tv2);                 // 12. tv2 = tv3^2
  fp.mul(tv3, tv3, gxd);       // 13. tv3 = tv3 * gxd
  fp.mul(tv3, tv3, gx1);       // 14. tv3 = tv3 * gx1
  fp.mul(tv2, tv2, tv3);       // 15. tv2 = tv2 * tv3
  expoC4(y11, tv2);            // 16. y11 = tv2^c4
  fp.mul(y11, y11, tv3);       // 17. y11 = y11 * tv3
  fp.mul(y12, y11, c3);        // 18. y12 = y11 * c3
  fp.misc.copy(tv2, y11);      //
  fp.sqr(tv2);                 // 19. tv2 = y11^2
  fp.mul(tv2, tv2, gxd);       // 20. tv2 = tv2 * gxd
  e1 = fp.cmp(tv2, gx1) == 0;  // 21.  e1 = tv2 == gx1
  fp.cmv(e1, y1, y12, y11);    // 22.  y1 = CMOV(y12, y11, e1)
  fp.mul(x2n, x1n, tv1);       // 23. x2n = x1n * tv1
  fp.mul(y21, y11, u);         // 24. y21 = y11 * u
  fp.mul(y21, y21, c2);        // 25. y21 = y21 * c2
  fp.mul(y22, y21, c3);        // 26. y22 = y21 * c3
  fp.mul(gx2, gx1, tv1);       // 27. gx2 = gx1 * tv1
  fp.misc.copy(tv2, y21);      //
  fp.sqr(tv2);                 // 28. tv2 = y21^2
  fp.mul(tv2, tv2, gxd);       // 29. tv2 = tv2 * gxd
  e2 = fp.cmp(tv2, gx2) == 0;  // 30.  e2 = tv2 == gx2
  fp.cmv(e2, y2, y22, y21);    // 31.  y2 = CMOV(y22, y21, e2)
  fp.misc.copy(tv2, y1);       //
  fp.sqr(tv2);                 // 32. tv2 = y1^2
  fp.mul(tv2, tv2, gxd);       // 33. tv2 = tv2 * gxd
  e3 = fp.cmp(tv2, gx1) == 0;  // 34.  e3 = tv2 == gx1
  fp.cmv(e3, xn, x2n, x1n);    // 35.  xn = CMOV(x2n, x1n, e3)
  fp.cmv(e3, y, y2, y1);       // 36.   y = CMOV(y2, y1, e3)
  e4 = fp.sgn(y);              // 37.  e4 = sgn0(y) == 1
  fp.misc.copy(y3, y);         //
  fp.neg(y3);                  //
  fp.cmv(e3 ^ e4, yn, y, y3);  // 38.   y = CMOV(y, -y, e3 XOR e4)
  fp.misc.copy(yd, one);
}

static inline void elligator2_edwards25519(argElement_1w xn, argElement_1w xd,
                                           argElement_1w yn, argElement_1w yd,
                                           argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  int e = 0;
  EltFp25519_1w_fullradix zero = {0};
  EltFp25519_1w_fullradix one = {1};
  EltFp25519_1w_fullradix c1 = /*c1 = sqrt(-486664), st sgn(c1)==0. */ {
      0xcc6e04aaff457e06,
      0xc5a1d3d14b7d1a82,
      0xd27b08dc03fc4f7e,
      0xf26edf460a006bb,
  };
  EltFp25519_1w_fullradix xMn, xMd, yMn, yMd, tv1;

  elligator2_curve25519(xMn, xMd, yMn, yMd, u);
  // 1.  (xMn, xMd, yMn, yMd) = elligator2_curve25519(u)
  fp.mul(xn, xMn, yMd);        // 2.  xn = xMn * yMd
  fp.mul(xn, xn, c1);          // 3.  xn = xn * c1
  fp.mul(xd, xMd, yMn);        // 4.  xd = xMd * yMn
  fp.sub(yn, xMn, xMd);        // 5.  yn = xMn - xMd
  fp.add(yd, xMn, xMd);        // 6.  yd = xMn + xMd
  fp.mul(tv1, xd, yd);         // 7. tv1 = xd * yd
  e = fp.cmp(tv1, zero) == 0;  // 8.   e = tv1 == 0
  fp.cmv(e, xn, xn, zero);     // 9.  xn = CMOV(xn, 0, e)
  fp.cmv(e, xd, xd, one);      // 10. xd = CMOV(xd, 1, e)
  fp.cmv(e, yn, yn, one);      // 11. yn = CMOV(yn, 1, e)
  fp.cmv(e, yd, yd, one);      // 12. yd = CMOV(yd, 1, e)
}

static inline void elligator2_curve25519_2w(argElement_2w xn, argElement_2w xd,
                                            argElement_2w yn, argElement_2w yd,
                                            argElement_2w u) {
  const Arith_2w fp = Fp25519._2w_red.arith;
  const __m256i ONE = SET64(0, 1, 0, 1);
  __m256i e1, e2, e3, e4;
  EltFp25519_2w_redradix one = {ONE, ZERO, ZERO, ZERO, ZERO};
  EltFp25519_2w_redradix A = {SET64(0, 486662, 0, 486662), ZERO, ZERO, ZERO,
                              ZERO};
  EltFp25519_2w_redradix c2 = /* 2^((p+3)/8) */ {
      SET64(0x1fbd7a7, 0x20ea0b1, 0x1fbd7a7, 0x20ea0b1),
      SET64(0x2804c9e, 0x186c9d2, 0x2804c9e, 0x186c9d2),
      SET64(0x1e16569, 0x08f189d, 0x1e16569, 0x08f189d),
      SET64(0x004fc1d, 0x035697f, 0x004fc1d, 0x035697f),
      SET64(0x0ae0c92, 0x0bd0c60, 0x0ae0c92, 0x0bd0c60),
  };
  EltFp25519_2w_redradix c3 = /* sqrt_minus_one */ {
      SET64(0x1fbd7a7, 0x20ea0b0, 0x1fbd7a7, 0x20ea0b0),
      SET64(0x2804c9e, 0x186c9d2, 0x2804c9e, 0x186c9d2),
      SET64(0x1e16569, 0x08f189d, 0x1e16569, 0x08f189d),
      SET64(0x004fc1d, 0x035697f, 0x004fc1d, 0x035697f),
      SET64(0x0ae0c92, 0x0bd0c60, 0x0ae0c92, 0x0bd0c60),
  };
  EltFp25519_2w_redradix tv1, tv2, tv3, gx1, x1n, gxd, x2n, y1, y12, y21, y11,
      gx2, y22, y2, y3, y;

  fp.misc.copy(tv1, u);            //
  fp.sqr(tv1);                     // 1.  tv1 = u^2
  fp.add(tv1, tv1, tv1);           // 2.  tv1 = 2 * tv1
  fp.add(xd, tv1, one);            // 3.   xd = tv1 + 1
  fp.misc.copy(x1n, A);            //
  fp.neg(x1n);                     // 4.  x1n = -J
  fp.misc.copy(tv2, xd);           //
  fp.sqr(tv2);                     // 5.  tv2 = xd^2
  fp.mul(gxd, tv2, xd);            // 6.  gxd = tv2 * xd
  fp.mul(gx1, A, tv1);             // 7.  gx1 = J * tv1
  fp.mul(gx1, gx1, x1n);           // 8.  gx1 = gx1 * x1n
  fp.add(gx1, gx1, tv2);           // 9.  gx1 = gx1 + tv2
  fp.mul(gx1, gx1, x1n);           // 10. gx1 = gx1 * x1n
  fp.misc.copy(tv3, gxd);          //
  fp.sqr(tv3);                     // 11. tv3 = gxd^2
  fp.misc.copy(tv2, tv3);          //
  fp.sqr(tv2);                     // 12. tv2 = tv3^2
  fp.mul(tv3, tv3, gxd);           // 13. tv3 = tv3 * gxd
  fp.mul(tv3, tv3, gx1);           // 14. tv3 = tv3 * gx1
  fp.mul(tv2, tv2, tv3);           // 15. tv2 = tv2 * tv3
  expoC4_2w(y11, tv2);             // 16. y11 = tv2^c4
  fp.mul(y11, y11, tv3);           // 17. y11 = y11 * tv3
  fp.mul(y12, y11, c3);            // 18. y12 = y11 * c3
  fp.misc.copy(tv2, y11);          //
  fp.sqr(tv2);                     // 19. tv2 = y11^2
  fp.mul(tv2, tv2, gxd);           // 20. tv2 = tv2 * gxd
  e1 = fp.cmp(tv2, gx1) == ZERO;   // 21.  e1 = tv2 == gx1
  fp.cmv(e1, y1, y12, y11);        // 22.  y1 = CMOV(y12, y11, e1)
  fp.mul(x2n, x1n, tv1);           // 23. x2n = x1n * tv1
  fp.mul(y21, y11, u);             // 24. y21 = y11 * u
  fp.mul(y21, y21, c2);            // 25. y21 = y21 * c2
  fp.mul(y22, y21, c3);            // 26. y22 = y21 * c3
  fp.mul(gx2, gx1, tv1);           // 27. gx2 = gx1 * tv1
  fp.misc.copy(tv2, y21);          //
  fp.sqr(tv2);                     // 28. tv2 = y21^2
  fp.mul(tv2, tv2, gxd);           // 29. tv2 = tv2 * gxd
  e2 = fp.cmp(tv2, gx2) == ZERO;   // 30.  e2 = tv2 == gx2
  fp.cmv(e2, y2, y22, y21);        // 31.  y2 = CMOV(y22, y21, e2)
  fp.misc.copy(tv2, y1);           //
  fp.sqr(tv2);                     // 32. tv2 = y1^2
  fp.mul(tv2, tv2, gxd);           // 33. tv2 = tv2 * gxd
  e3 = fp.cmp(tv2, gx1) == ZERO;   // 34.  e3 = tv2 == gx1
  fp.cmv(e3, xn, x2n, x1n);        // 35.  xn = CMOV(x2n, x1n, e3)
  fp.cmv(e3, y, y2, y1);           // 36.   y = CMOV(y2, y1, e3)
  e4 = fp.sgn(y) == ONE;           // 37.  e4 = sgn0(y) == 1
  fp.misc.copy(y3, y);             //
  fp.neg(y3);                      //
  fp.cmv(XOR(e3, e4), yn, y, y3);  // 38.   y = CMOV(y, -y, e3 XOR e4)
  fp.misc.copy(yd, one);
  Fp25519._2w_red.arithex.compress(xn);
  Fp25519._2w_red.arithex.compress(xd);
  Fp25519._2w_red.arithex.compress(yn);
  Fp25519._2w_red.arithex.compress(yd);
}

static inline void elligator2_edwards25519_2w(argElement_2w xn,
                                              argElement_2w xd,
                                              argElement_2w yn,
                                              argElement_2w yd,
                                              argElement_2w u) {
  const Arith_2w fp = Fp25519._2w_red.arith;
  __m256i e;
  EltFp25519_2w_redradix zero = {ZERO};
  EltFp25519_2w_redradix one = {SET64(0, 1, 0, 1), ZERO, ZERO, ZERO, ZERO};
  EltFp25519_2w_redradix c1 = /*c1 = sqrt(-486664), st sgn(c1)==0. */ {
      SET64(0x1fc4f7e, 0x3457e06, 0x1fc4f7e, 0x3457e06),
      SET64(0x1846e01, 0x1812abf, 0x1846e01, 0x1812abf),
      SET64(0x0d77a4f, 0x350598d, 0x0d77a4f, 0x350598d),
      SET64(0x3460a00, 0x08a5be8, 0x3460a00, 0x08a5be8),
      SET64(0x03c9bb7, 0x316874f, 0x03c9bb7, 0x316874f),
  };
  EltFp25519_2w_redradix xMn, xMd, yMn, yMd, tv1;

  elligator2_curve25519_2w(xMn, xMd, yMn, yMd, u);
  // 1.  (xMn, xMd, yMn, yMd) = elligator2_curve25519(u)
  fp.mul(xn, xMn, yMd);           // 2.  xn = xMn * yMd
  fp.mul(xn, xn, c1);             // 3.  xn = xn * c1
  fp.mul(xd, xMd, yMn);           // 4.  xd = xMd * yMn
  fp.sub(yn, xMn, xMd);           // 5.  yn = xMn - xMd
  fp.add(yd, xMn, xMd);           // 6.  yd = xMn + xMd
  fp.mul(tv1, xd, yd);            // 7. tv1 = xd * yd
  e = fp.cmp(tv1, zero) == ZERO;  // 8.   e = tv1 == 0
  fp.cmv(e, xn, xn, zero);        // 9.  xn = CMOV(xn, 0, e)
  fp.cmv(e, xd, xd, one);         // 10. xd = CMOV(xd, 1, e)
  fp.cmv(e, yn, yn, one);         // 11. yn = CMOV(yn, 1, e)
  fp.cmv(e, yd, yd, one);         // 12. yd = CMOV(yd, 1, e)
  Fp25519._2w_red.arithex.compress(xn);
  Fp25519._2w_red.arithex.compress(xd);
  Fp25519._2w_red.arithex.compress(yn);
  Fp25519._2w_red.arithex.compress(yd);
}

void map_to_curve(PointXYZT_1way_full *P, argElement_1w u) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix xn, xd, yn, yd;

  elligator2_edwards25519(xn, xd, yn, yd, u);
  fp.mul(P->X, xn, yd);
  fp.mul(P->Y, yn, xd);
  fp.mul(P->T, xn, yn);
  fp.mul(P->Z, xd, yd);
}

void map_to_curve_2w(PointXYZT_2way *P, argElement_2w u0u1) {
  const Arith_2w fp = Fp25519._2w_red.arith;
  EltFp25519_2w_redradix xn, xd, yn, yd;

  elligator2_edwards25519_2w(xn, xd, yn, yd, u0u1);
  fp.mul(P->X, xn, yd);
  fp.mul(P->Y, yn, xd);
  fp.mul(P->T, xn, yn);
  fp.mul(P->Z, xd, yd);
}

static inline void clear_cofactor(PointXYZT_1way_full *P) {
  _1way_doubling_1w_full(P);
  _1way_doubling_1w_full(P);
  _1way_doubling_1w_full(P);
}

static inline void clear_cofactor_2w(PointXYZT_2way *P) {
  _2way_doubling(P, 0);
  _2way_doubling(P, 0);
  _2way_doubling(P, 1);
}

static inline void toAffine(Point *P, PointXYZT_1way_full *P_proy) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix invZ;

  fp.inv(invZ, P_proy->Z);
  fp.mul(P->X, P_proy->X, invZ);
  fp.mul(P->Y, P_proy->Y, invZ);
}

static inline int isOnCurveProy(PointXYZT_1way_full *P) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix x2, y2, z2, t2, tz, dt2, e0, e1;
  EltFp25519_1w_fullradix zero = {0};
  EltFp25519_1w_fullradix paramD = {0x75eb4dca135978a3, 0x00700a4d4141d8ab,
                                    0x8cc740797779e898, 0x52036cee2b6ffe73};

  // ax^2+y^2=z^2+dT, XY=ZT
  fp.misc.copy(x2, P->X);
  fp.misc.copy(y2, P->Y);
  fp.misc.copy(t2, P->T);
  fp.misc.copy(z2, P->Z);
  fp.sqr(x2);
  fp.sqr(y2);
  fp.sqr(t2);
  fp.sqr(z2);
  fp.mul(dt2, t2, paramD);
  fp.sub(e0, y2, x2);  // a = -1
  fp.sub(e0, e0, z2);
  fp.sub(e0, e0, dt2);

  fp.mul(e1, P->X, P->Y);
  fp.mul(tz, P->T, P->Z);
  fp.sub(e1, e1, tz);
  return fp.cmp(e0, zero) == 0 && fp.cmp(e1, zero) == 0;
}

int isOnCurve(Point *P) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  EltFp25519_1w_fullradix x2, y2, dx2y2, e;
  EltFp25519_1w_fullradix zero = {0};
  EltFp25519_1w_fullradix one = {1};
  EltFp25519_1w_fullradix paramD = {0x75eb4dca135978a3, 0x00700a4d4141d8ab,
                                    0x8cc740797779e898, 0x52036cee2b6ffe73};

  // ax^2+y^2=z^2+dT, XY=ZT
  fp.misc.copy(x2, P->X);
  fp.misc.copy(y2, P->Y);
  fp.sqr(x2);
  fp.sqr(y2);
  fp.mul(dx2y2, x2, y2);
  fp.mul(dx2y2, dx2y2, paramD);
  fp.sub(e, y2, x2);  // a = -1
  fp.sub(e, e, one);
  fp.sub(e, e, dx2y2);
  return fp.cmp(e, zero) == 0;
}

int areEqual(Point *P, Point *Q) {
  const Arith_1w fp = Fp25519._1w_full.arith;
  return fp.cmp(P->X, Q->X) == 0 && fp.cmp(P->Y, Q->Y) == 0;
}

void h2c25519_x64(Point *P, uint8_t *msg, size_t mlen) {
  EltFp25519_1w_fullradix u0, u1;
  PointXYZT_1way_full Q0, Q1;
  PointXYZT_1way_full Q;

  hash_to_field(u0, 0, msg, mlen);
  hash_to_field(u1, 1, msg, mlen);
  map_to_curve(&Q0, u0);
  map_to_curve(&Q1, u1);

  clear_cofactor(&Q0);
  clear_cofactor(&Q1);
  _1way_fulladd_1w_full(&Q, &Q0, &Q1);
  toAffine(P, &Q);
}

void h2c25519_avx2(Point *P, uint8_t *msg, size_t mlen) {
  EltFp25519_1w_fullradix uu0, uu1;
  EltFp25519_1w_redradix u0, u1;
  PointXYZT_2way Q0Q1;
  EltFp25519_2w_redradix u0u1;
  PointXYZT_1way Q0, Q1;
  PointXYZT_1way_full P0, P1;
  PointXYZT_1way_full Q;

  hash_to_field_2w(uu0, uu1, msg, mlen);
  Fp25519._1w_red.arith.misc.unser(u0, (uint8_t *)uu0);
  Fp25519._1w_red.arith.misc.unser(u1, (uint8_t *)uu1);
  Fp25519._2w_red.arithex.inter(u0u1, u0, u1);

  map_to_curve_2w(&Q0Q1, u0u1);
  clear_cofactor_2w(&Q0Q1);

  Fp25519._2w_red.arithex.deinter(Q0.X, Q1.X, Q0Q1.X);
  Fp25519._2w_red.arithex.deinter(Q0.Y, Q1.Y, Q0Q1.Y);
  Fp25519._2w_red.arithex.deinter(Q0.T, Q1.T, Q0Q1.T);
  Fp25519._2w_red.arithex.deinter(Q0.Z, Q1.Z, Q0Q1.Z);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P0.X, Q0.X);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P0.Y, Q0.Y);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P0.T, Q0.T);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P0.Z, Q0.Z);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P1.X, Q1.X);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P1.Y, Q1.Y);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P1.T, Q1.T);
  Fp25519._1w_red.arith.misc.ser((uint8_t *)P1.Z, Q1.Z);
  _1way_fulladd_1w_full(&Q, &P0, &P1);
  toAffine(P, &Q);
}

static inline void print_point_proy(FILE *file, PointXYZT_1way_full *P) {
  fprintf(file, "X: ");
  Fp25519._1w_full.arith.misc.print(file, P->X);
  fprintf(file, "Y: ");
  Fp25519._1w_full.arith.misc.print(file, P->Y);
  fprintf(file, "T: ");
  Fp25519._1w_full.arith.misc.print(file, P->T);
  fprintf(file, "Z: ");
  Fp25519._1w_full.arith.misc.print(file, P->Z);
}

void print_point(FILE *file, Point *P) {
  fprintf(file, "X: ");
  Fp25519._1w_full.arith.misc.print(file, P->X);
  fprintf(file, "Y: ");
  Fp25519._1w_full.arith.misc.print(file, P->Y);
}

#else
extern int version;
#endif /* HASH255 */
