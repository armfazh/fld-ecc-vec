#ifndef _ECC_ED448_H_
#define _ECC_ED448_H_

#include "fp.h"

typedef ALIGN struct point_extended_1way_fp448{
	Element_1w_Fp448 X,Y,Z,T;
} PointXYZT_1way_Fp448;

typedef ALIGN struct point_extended_2way_fp448{
	Element_2w_Fp448 X,Y,Z,T;
} PointXYZT_2way_Fp448;

typedef ALIGN struct point_precompute_2way_fp448{
    Element_2w_Fp448 addYX,subYX,_2dYX;
} Point_precmp_2way_Fp448;

typedef ALIGN struct point_extended_4way_fp448{
	Element_4w_Fp448 X,Y,Z,T;
} PointXYZT_4way_Fp448;

typedef ALIGN struct point_precompute_4way_fp448{
    Element_4w_Fp448 addYX,subYX,_2dYX;
} Point_precmp_4way_Fp448;


/** structures for verification */
#define OMEGA_DYNAMIC 5
typedef ALIGN struct pointxytz_precompute_2w_h0h8_fp448{
	Element_2w_Fp448 subaddYX,_2dT_2Z;
} PointXYZT_precompute_2w_H0H8;

typedef ALIGN struct pointxytz_2w_h0h8_fp448{
	Element_2w_Fp448 XY,TZ;
} PointXYZT_2w_H0H8;


void _4way_mixadd_448(PointXYZT_4way_Fp448 *Q, Point_precmp_4way_Fp448 *P);
void _1way_doubling_2w_H0H8(PointXYZT_2w_H0H8 * P);
void _1way_mixadd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_precompute_2w_H0H8 *P);
void _1way_fulladd_2w_H0H8(PointXYZT_2w_H0H8 *Q, PointXYZT_2w_H0H8 *P);

void recoding_signed_scalar_fold4w4_448(uint64_t *list_signs, uint64_t *list_digits, uint8_t *r);
int wnaf_448bits(int8_t *K, const uint8_t *p8_r, int w);
void query_table_ed448(Point_precmp_4way_Fp448 *P, const uint8_t * table,uint64_t * secret_signs,uint64_t *secret_digits);


#endif /* _ECC_ED448_H_ */
