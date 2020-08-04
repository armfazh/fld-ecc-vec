/**
 * Copyright 2020 Armando Faz Hernández
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

#ifndef FAZ_HASH_AVX2_H
#define FAZ_HASH_AVX2_H

#include <faz_fp_avx2.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
namespace faz {
namespace hash {
#endif /* __cplusplus */

#ifdef __cplusplus
using namespace faz::fp;
#endif

typedef ALIGN struct point_affine { EltFp25519_1w_fullradix X, Y; } Point;

void h2c25519_x64(Point *P, uint8_t *msg, size_t mlen);
void h2c25519_avx2(Point *P, uint8_t *msg, size_t mlen);
int isOnCurve(Point *P);
int areEqual(Point *P, Point *Q);
void print_point(FILE *file, Point *P);

// tmp ->
#include "edwards255.h"
void map_to_curve(PointXYZT_1way_full *P, argElement_1w u);
void map_to_curve_2w(PointXYZT_2way *P, argElement_2w u0u1);
void _1way_fulladd_1w_full(PointXYZT_1way_full *R, PointXYZT_1way_full *Q,
                           PointXYZT_1way_full *P);

void hash_to_field(argElement_1w u, uint8_t prefix, uint8_t *msg,
size_t mlen);
void hash_to_field_2w(argElement_1w u0, argElement_1w u1,
               uint8_t *msg, size_t mlen);

#ifdef __cplusplus
} /* namespace fp */
} /* namespace faz */
} /* extern C */
#endif /* __cplusplus */

#endif /* FAZ_HASH_AVX2_H */
