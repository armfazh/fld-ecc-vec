#ifndef _ELEMENT_1W_H0H8_H_
#define _ELEMENT_1W_H0H8_H_

#include <stdint.h>
#include "avx2.h"

#define SIZE_ELEMENT_BYTES 56
typedef ALIGN uint8_t STR_BYTES[SIZE_ELEMENT_BYTES];

#define MEM_SIZE_CURVE448 16
typedef ALIGN uint64_t Element_1w_H0H8[MEM_SIZE_CURVE448];
typedef uint64_t * argElement_1w_H0H8;
#define NUM_WORDS_64B_CURVE448 16
#define NUM_WORDS_256B_SINGLE_H0H8 4

#define VECT_BASE 28

int compare_bytes(uint8_t* A, uint8_t* B,unsigned int num_bytes);
void random_Element_1w_h0h8(uint64_t *A);
void print_Element_1w_h0h8(uint64_t *A);
int compare_Element_1w_h0h8(uint64_t *A, uint64_t *B);

void add_Element_1w_h0h8(uint64_t *C, uint64_t *A, uint64_t *B);
void sub_Element_1w_h0h8(uint64_t *C, uint64_t *A, uint64_t *B);
void mul_Element_1w_h0h8(uint64_t *C, uint64_t *A, uint64_t *B);
void sqr_Element_1w_h0h8(uint64_t *A);
void compress_Element_1w_h0h8(uint64_t *A);
void invsqrt_Element_1w_h0h8(uint64_t * pC, uint64_t * pA, const int only_inverse);

void inv_Element_1w_h0h8(uint64_t * pC, uint64_t * pA);
void sqrt_Element_1w_h0h8(uint64_t * pC, uint64_t * pA);

#define copy_Element_1w_h0h8(C,A)\
	STORE(C+0,LOAD(A+0));\
	STORE(C+1,LOAD(A+1));\
	STORE(C+2,LOAD(A+2));\
	STORE(C+3,LOAD(A+3));


#endif /* _ELEMENT_1W_H0H8_H_ */
