#ifndef _ECC_H_
#define _ECC_H_

#include "fp.h"

typedef ALIGN struct point_extended_1way{
	Element_1w_Fp448 X;
	Element_1w_Fp448 Y;
	Element_1w_Fp448 Z;
	Element_1w_Fp448 T;
} PointXYZT_1way;

typedef ALIGN struct point_extended_2way{
	Element_2w_Fp448 X;
	Element_2w_Fp448 Y;
	Element_2w_Fp448 Z;
	Element_2w_Fp448 T;
} PointXYZT_2way;

typedef ALIGN struct point_precompute_2way{
    Element_2w_Fp448 addYX;
	Element_2w_Fp448 subYX;
	Element_2w_Fp448 _2dYX;
} Point_precmp_2way;

typedef ALIGN struct point_extended_4way{
	Element_4w_Fp448 X;
	Element_4w_Fp448 Y;
	Element_4w_Fp448 Z;
	Element_4w_Fp448 T;
} PointXYZT_4way;

typedef ALIGN struct point_precompute_4way{
    Element_4w_Fp448 addYX;
	Element_4w_Fp448 subYX;
	Element_4w_Fp448 _2dYX;
} Point_precmp_4way;


/** structures for verification */
#define OMEGA_DYNAMIC 5
typedef ALIGN struct pointxytz_precompute_2w_h0h8{
	Element_2w_Fp448 subaddYX;
	Element_2w_Fp448 _2dT_2Z;
} PointXYZT_precompute_2w_H0H8;

typedef ALIGN struct pointxytz_2w_h0h8{
	Element_2w_Fp448 XY;
	Element_2w_Fp448 TZ;
} PointXYZT_2w_H0H8;

#define NUM_LUT (28)
#define SIZE_POINT (3*448/8)
#define SIZE_ONE_LUT (8*SIZE_POINT)
#define SIZE_ALL_LUT (NUM_LUT*SIZE_POINT)

void _4way_mixadd_448(PointXYZT_4way *Q, Point_precmp_4way *P);
void _1way_doubling_2w_H0H8(PointXYZT_2w_H0H8 * P);
void _1way_mixadd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_precompute_2w_H0H8 *P);
void _1way_fulladd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_2w_H0H8 *P);

void recoding_signed_scalar_fold4w4_448(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r);
int wnaf_448bits(int8_t *K, const uint8_t *p8_r, int w);
void query_table_448(Point_precmp_4way *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits);


#endif /* _ECC_H_ */
