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

#ifndef EDWARDS255_H
#define EDWARDS255_H

#include <faz_fp_avx2.h>

/*!
 * A point \f$ (x,y)\in\mathbb{F}_p \f$
 *
 */
#define LUT_24KB 24
#define LUT_12KB 12
#define LOOKUP_TABLE_SIZE LUT_24KB

typedef ALIGN struct point_extended_1way_full {
  EltFp25519_1w_fullradix X;
  EltFp25519_1w_fullradix Y;
  EltFp25519_1w_fullradix Z;
  EltFp25519_1w_fullradix T;
} PointXYZT_1way_full;

typedef ALIGN struct point_extended_1way {
  EltFp25519_1w_redradix X;
  EltFp25519_1w_redradix Y;
  EltFp25519_1w_redradix Z;
  EltFp25519_1w_redradix T;
} PointXYZT_1way;

typedef ALIGN struct point_extended_2way {
  EltFp25519_2w_redradix X;
  EltFp25519_2w_redradix Y;
  EltFp25519_2w_redradix Z;
  EltFp25519_2w_redradix T;
} PointXYZT_2way;

typedef ALIGN struct point_precompute_2way {
  EltFp25519_2w_redradix addYX;
  EltFp25519_2w_redradix subYX;
  EltFp25519_2w_redradix _2dYX;
} Point_precmp_2way;

typedef ALIGN struct point_extended_4way {
  EltFp25519_4w_redradix X;
  EltFp25519_4w_redradix Y;
  EltFp25519_4w_redradix Z;
  EltFp25519_4w_redradix T;
} PointXYZT_4way;

typedef ALIGN struct point_precompute_4way {
  EltFp25519_4w_redradix addYX;
  EltFp25519_4w_redradix subYX;
  EltFp25519_4w_redradix _2dYX;
} Point_precmp_4way;


/** structures for verification */
#define OMEGA_DYNAMIC 5
typedef ALIGN struct pointxytz_precompute_2w_h0h5 {
  EltFp25519_2w_redradix subaddYX;
  EltFp25519_2w_redradix _2dT_2Z;
} PointXYZT_precompute_2w_H0H5;

typedef ALIGN struct pointxytz_2w_h0h5 {
  EltFp25519_2w_redradix XY;
  EltFp25519_2w_redradix TZ;
} PointXYZT_2w_H0H5;

typedef ALIGN struct pointxytz_4w_h0h5 {
  EltFp25519_4w_redradix XYTZ;
} PointXYTZ_4w_H0H5;

typedef ALIGN struct pointxytz_precompute_4w_h0h5 {
  EltFp25519_4w_redradix subYX_addYX_2dT_2Z;
} PointXYTZ_precompute_4w_H0H5;

#endif /* EDWARDS255_H */
