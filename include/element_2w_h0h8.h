#ifndef _ELEMENT_2W_H0H8_H_
#define _ELEMENT_2W_H0H8_H_

#include <stdint.h>
#include "avx2.h"

#define NUM_WORDS_128B_CURVE448 8
typedef ALIGN __m256i Element_2w_H0H8[NUM_WORDS_128B_CURVE448];
typedef __m256i * argElement_2w_H0H8;

void random_Element_2w_h0h8(argElement_2w_H0H8 X_Y);
void print_Element_2w_h0h8(argElement_2w_H0H8 X_Y);
int compare_Element_2w_h0h8(argElement_2w_H0H8 X0_X1,argElement_2w_H0H8 Y0_Y1);

void add_Element_2w_h0h8(argElement_2w_H0H8  C,argElement_2w_H0H8 A,argElement_2w_H0H8 B);
void sub_Element_2w_h0h8(argElement_2w_H0H8 C,argElement_2w_H0H8 A,argElement_2w_H0H8 B);
void mul_Element_2w_h0h8(__m256i * C, __m256i * A, __m256i * B);
void sqr_Element_2w_h0h8(__m256i * C);
void compress_Element_2w_h0h8(__m256i * C);

#define copy_Element_2w_h0h8(C,A) \
do{                               \
	C[0] = A[0];                  \
	C[1] = A[1];                  \
	C[2] = A[2];                  \
	C[3] = A[3];                  \
	C[4] = A[4];                  \
	C[5] = A[5];                  \
	C[6] = A[6];                  \
	C[7] = A[7];                  \
}while(0)

#endif /* _ELEMENT_2W_H0H8_H_ */
