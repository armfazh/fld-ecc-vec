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

#if defined(BYTESTRING_C)

#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#define ALIGN_BYTES 32

extern void prgn_random_bytes(uint8_t *buffer, size_t num_bytes);

static inline void random_bytes(uint8_t *buffer, size_t num_bytes) {
  prgn_random_bytes(buffer, num_bytes);
}

/**
 *
 * @param pFile
 * @param buffer
 * @param num_bytes
 */
static inline void print_hex_bytes(FILE *pFile, uint8_t *buffer, size_t num_bytes) {
  size_t i;
  fprintf(pFile, "0x");
  for (i = 0; i < num_bytes; i++) {
    fprintf(pFile, "%02x", buffer[num_bytes - 1 - i]);
  }
  fprintf(pFile, "\n");
}

static inline int cmp_bytes(uint8_t *a, uint8_t *b, size_t num_bytes) {
  size_t i = 0;
  int ret = 0;
  for (i = 0; i < num_bytes; i++) {
    ret |= a[i] ^ b[i];
  }
  return ret;
}

static uint8_t * allocate_bytes(size_t num_bytes) {
  return (uint8_t *) _mm_malloc(num_bytes, ALIGN_BYTES);
}

static void deallocate_bytes(void *a) {
  if (a != NULL) {
    _mm_free(a);
  }
}

/**
 *
 * @param C
 * @param A
 * @param numA
 * @param B
 * @param numB
 */
static inline void word64_multiplier(
    uint64_t *C,
    const uint64_t *A, const int numA,
    const uint64_t *B, const int numB) {
  int i, j;
#define mul64x64(Z, X, Y)    __asm__ __volatile__ (\
            "movq 0(%3), %%rax     ;"\
            "mulq 0(%4)            ;"\
            "addq %%rax, %0        ;"\
            "adcq %%rdx, %1        ;"\
            "adcq $0x0,  %2        ;"\
    :/* out  */ "+r" ((Z)[0]),"+r" ((Z)[1]),"+r" ((Z)[2])\
    :/* in   */ "r" (X),"r" (Y)\
    :/* regs */ "memory","cc","%rax","%rdx");

  for (i = 0; i < numA; i++) {
    for (j = 0; j < numB; j++) {
      mul64x64(C + i + j, A + i, B + j);
    }
  }
#undef mul64x64
}

volatile void *spc_memset(volatile void *dst, int c, size_t len) {
  volatile char *buf;

  for (buf = (volatile char *) dst; len; buf[--len] = c);
  return dst;
}

#else
extern int version;
#endif /* BYTESTRING_C */
