#ifndef _ELEMENT_4W_H0H8_H_
#define _ELEMENT_4W_H0H8_H_

#include<stdint.h>
#include "avx2.h"

#define NUM_WORDS_CURVE448 16
typedef ALIGN __m256i Element_4w_H0H8[NUM_WORDS_CURVE448];
typedef __m256i * argElement_4w_H0H6;

void random_Element_4w_h0h8(argElement_4w_H0H6 X_Y_Z_T);
void print_Element_4w_h0h8(argElement_4w_H0H6 X_Y_Z_T);
int compare_Element_4w_h0h8(argElement_4w_H0H6 X0_X1_X2_X3, argElement_4w_H0H6 Y0_Y1_Y2_Y3);

void add_Element_4w_h0h8(__m256i * C, __m256i * A, __m256i * B);
void sub_Element_4w_h0h8(__m256i * C, __m256i * A, __m256i * B);
void mul_Element_4w_h0h8(__m256i * Z, __m256i * X, __m256i * Y);
void sqr_Element_4w_h0h8(__m256i * C);
void compress_Element_4w_h0h8(__m256i * C);

#endif /* _ELEMENT_4W_H0H8_H_ */
