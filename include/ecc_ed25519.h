#ifndef _ECC_ED25519_H_
#define _ECC_ED25519_H_

#include "fp.h"

typedef ALIGN struct point_extended_1way_fp25519{
	Element_1w_Fp25519 X,Y,Z,T;
} PointXYZT_1way_Fp25519;

typedef ALIGN struct point_extended_2way_fp25519{
	Element_2w_Fp25519 X,Y,Z,T;
} PointXYZT_2way_Fp25519;

typedef ALIGN struct point_precompute_2way_fp25519{
	Element_2w_Fp25519 addYX,subYX,_2dYX;
} Point_precmp_2way_Fp25519;

typedef ALIGN struct point_extended_4way_fp25519{
	Element_4w_Fp25519 X,Y,Z,T;
} PointXYZT_4way_Fp25519;

typedef ALIGN struct point_precompute_4way_fp25519{
	Element_4w_Fp25519 addYX,subYX,_2dYX;
} Point_precmp_4way_Fp25519;

/** structures for verification */
#define OMEGA_DYNAMIC 5
typedef ALIGN struct pointxytz_precompute_2w_h0h5_fp25519{
	Element_2w_Fp25519 subaddYX,_2dT_2Z;
} PointXYZT_precompute_2w_H0H5;

typedef ALIGN struct pointxytz_2w_h0h5_fp25519{
	Element_2w_Fp25519 XY,TZ;
} PointXYZT_2w_H0H5;

#define LUT_24KB 24
#define LUT_12KB 12
#define LOOKUP_TABLE_SIZE LUT_24KB


void _4way_mixadd_25519(PointXYZT_4way_Fp25519 *Q, Point_precmp_4way_Fp25519 *P);
void _1way_doubling_2w_H0H5(PointXYZT_2w_H0H5 *P);
void _1way_mixadd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_precompute_2w_H0H5 *P);
void _1way_fulladd_2w_H0H5(PointXYZT_2w_H0H5 *Q, PointXYZT_2w_H0H5 *P);

void recoding_signed_scalar_ed25519(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r);
int wnaf_256bits(int8_t *K, const uint8_t *p8_r, int w);
void query_table_ed25519(Point_precmp_4way_Fp25519 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits);

#endif /* _ECC_ED25519_H_ */
