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

#ifndef EDWARDS448_H
#define EDWARDS448_H

#include <faz_fp_avx2.h>

typedef ALIGN struct point_extended_1way_fp448 {
  EltFp448_1w_redradix X, Y, Z, T;
} PointXYZT_1way_Fp448;

typedef ALIGN struct point_extended_2way_fp448 {
  EltFp448_2w_redradix X, Y, Z, T;
} PointXYZT_2way_Fp448;

typedef ALIGN struct point_precompute_2way_fp448 {
  EltFp448_2w_redradix addYX, subYX, _2dYX;
} Point_precmp_2way_Fp448;

typedef ALIGN struct point_extended_4way_fp448 {
  EltFp448_4w_redradix X, Y, Z, T;
} PointXYZT_4way_Fp448;

typedef ALIGN struct point_precompute_4way_fp448 {
  EltFp448_4w_redradix addYX, subYX, _2dYX;
} Point_precmp_4way_Fp448;


/** structures for verification */
#define OMEGA_DYNAMIC 5
typedef ALIGN struct pointxytz_precompute_2w_h0h8_fp448 {
  EltFp448_2w_redradix subaddYX, _2dT_2Z;
} PointXYZT_precompute_2w_H0H8;

typedef ALIGN struct pointxytz_2w_h0h8_fp448 {
  EltFp448_2w_redradix XY, TZ;
} PointXYZT_2w_H0H8;

typedef ALIGN struct pointxytz_4w_h0h8 {
  EltFp448_4w_redradix XYTZ;
} PointXYTZ_4w_H0H8;

typedef ALIGN struct pointxytz_precompute_4w_h0h8 {
  EltFp448_4w_redradix subYX_addYX_2dT_2Z;
} PointXYTZ_precompute_4w_H0H8;

#endif /* EDWARDS448_H */
