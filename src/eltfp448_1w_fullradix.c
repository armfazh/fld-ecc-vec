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

#if defined(FP448)

#define FN(NAME) SUFFIX(NAME,1,fullradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

DECL(argElement_1w, alloc)(void) {
  return (argElement_1w) allocate_bytes(NUM_DIGITS_FP448_X64 * sizeof(uint64_t));
}

DECL(void, copy)(argElement_1w c, argElement_1w a) {
  c[0] = a[0];
  c[1] = a[1];
  c[2] = a[2];
  c[3] = a[3];
  c[4] = a[4];
  c[5] = a[5];
  c[6] = a[6];
}

DECL(void, prime)(argElement_1w a) {
  a[0] = 0xffffffffffffffff;
  a[1] = 0xffffffffffffffff;
  a[2] = 0xffffffffffffffff;
  a[3] = 0xfffffffeffffffff;
  a[4] = 0xffffffffffffffff;
  a[5] = 0xffffffffffffffff;
  a[6] = 0xffffffffffffffff;
}

DECL(void, rand)(argElement_1w a) {
  random_bytes((uint8_t *) a, SIZE_FP448);
}

DECL(void, zero)(argElement_1w a) {
  a[0] = 0;
  a[1] = 0;
  a[2] = 0;
  a[3] = 0;
  a[4] = 0;
  a[5] = 0;
  a[6] = 0;
}

DECL(void, sub)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "xorl %%eax, %%eax ;"
  "movq $0x100000000, %%rdx ;"

  "movq  0(%1),  %%r8 ;"
  "movq  8(%1),  %%r9 ;"
  "movq 16(%1), %%r10 ;"
  "movq 24(%1), %%r11 ;"
  "movq 32(%1), %%r12 ;"
  "movq 40(%1), %%r13 ;"
  "movq 48(%1), %%r14 ;"

  "subq  0(%2),  %%r8 ;"
  "sbbq  8(%2),  %%r9 ;"
  "sbbq 16(%2), %%r10 ;"
  "sbbq 24(%2), %%r11 ;"
  "sbbq 32(%2), %%r12 ;"
  "sbbq 40(%2), %%r13 ;"
  "sbbq 48(%2), %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "sbbq  %%rax,  %%r8 ;"
  "sbbq  %%rax,  %%r9 ;"
  "sbbq  %%rax, %%r10 ;"
  "sbbq  %%rcx, %%r11 ;"
  "sbbq  %%rax, %%r12 ;"
  "sbbq  %%rax, %%r13 ;"
  "sbbq  %%rax, %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "sbbq  %%rax,  %%r8 ;"
  "sbbq  %%rax,  %%r9 ;"
  "sbbq  %%rax, %%r10 ;"
  "sbbq  %%rcx, %%r11 ;"
  "sbbq  %%rax, %%r12 ;"
  "sbbq  %%rax, %%r13 ;"
  "sbbq  %%rax, %%r14 ;"

  "movq  %%r8,  0(%0) ;"
  "movq  %%r9,  8(%0) ;"
  "movq %%r10, 16(%0) ;"
  "movq %%r11, 24(%0) ;"
  "movq %%r12, 32(%0) ;"
  "movq %%r13, 40(%0) ;"
  "movq %%r14, 48(%0) ;"
  :
  : "r" (c), "r" (a), "r"  (b)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, cmv)(int bit, argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "test       %3,     %3 ;"
  "movq    0(%1),  %%rax ;"
  "cmovnz  0(%2),  %%rax ;"
  "movq    %%rax,  0(%0) ;"

  "movq    8(%1),  %%rax ;"
  "cmovnz  8(%2),  %%rax ;"
  "movq    %%rax,  8(%0) ;"

  "movq   16(%1),  %%rax ;"
  "cmovnz 16(%2),  %%rax ;"
  "movq    %%rax, 16(%0) ;"

  "movq   24(%1),  %%rax ;"
  "cmovnz 24(%2),  %%rax ;"
  "movq    %%rax, 24(%0) ;"

  "movq   32(%1),  %%rax ;"
  "cmovnz 32(%2),  %%rax ;"
  "movq    %%rax, 32(%0) ;"

  "movq   40(%1),  %%rax ;"
  "cmovnz 40(%2),  %%rax ;"
  "movq    %%rax, 40(%0) ;"

  "movq   48(%1),  %%rax ;"
  "cmovnz 48(%2),  %%rax ;"
  "movq    %%rax, 48(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b), "r" (bit)
  : "memory", "cc", "%rax"
  );
}

DECL(void, modp)(argElement_1w c) {
  EltFp448_1w_fullradix p;
  FN(prime)(p);
  FN(sub)(c, c, p);
}

DECL(int, sgn)(argElement_1w c) {
  FN(modp)(c);
  return c[0]&0x1;
}

DECL(void, ser)(uint8_t *buffer, argElement_1w c) {
  FN(copy)((argElement_1w)buffer,c);
  FN(modp)((argElement_1w)buffer);
}

DECL(void, unser)(argElement_1w c, uint8_t *buffer) {
  FN(copy)(c,(argElement_1w)buffer);
  FN(modp)(c);
}

DECL(int, cmp)(argElement_1w a, argElement_1w b) {
  FN(modp)(a);
  FN(modp)(b);
  return cmp_bytes((uint8_t *) a, (uint8_t *) b, SIZE_FP448);
}

DECL(void, print)(FILE *file, argElement_1w a) {
  print_hex_bytes(file, (uint8_t *) a, SIZE_FP448);
}

DECL(void, intmul_mulxadx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  /*  C[i] += A[i] x B  */
  ".macro MULACC_mulxadx I, R0, R1, R2, R3, R4, R5, R6;"
  "movq \\I(%1), %%rdx;"
  "xorl %%eax, %%eax;"
  "mulx  0(%2), %%rax, %%r8;"  "adox %%rax, \\R0;"  "adcx %%r8, \\R1;"  "movq \\R0, \\I(%0);"
  "mulx  8(%2), %%rax, %%r8;"  "adox %%rax, \\R1;"  "adcx %%r8, \\R2;"  "movq $0, \\R0;"
  "mulx 16(%2), %%rax, %%r8;"  "adox %%rax, \\R2;"  "adcx %%r8, \\R3;"
  "mulx 24(%2), %%rax, %%r8;"  "adox %%rax, \\R3;"  "adcx %%r8, \\R4;"
  "mulx 32(%2), %%rax, %%r8;"  "adox %%rax, \\R4;"  "adcx %%r8, \\R5;"
  "mulx 40(%2), %%rax, %%r8;"  "adox %%rax, \\R5;"  "adcx %%r8, \\R6;"
  "mulx 48(%2), %%rax, %%r8;"  "adox %%rax, \\R6;"  "adcx %%r8, \\R0;"
  "movq $0, %%rdx;"/********/  "adox %%rdx, \\R0;"
  ".endm;"

  /*  C[0] = A[0] x B  */
  "movq  0(%1), %%rdx;"
  "xorl %%eax, %%eax;"
  "movq $0, %%r8;"
  "mulx  0(%2), %%rax,  %%r9;"  "movq %%rax, 0(%0);"
  "mulx  8(%2), %%rax, %%r10;"  "adcx %%rax,  %%r9;"
  "mulx 16(%2), %%rax, %%r11;"  "adcx %%rax, %%r10;"
  "mulx 24(%2), %%rax, %%r12;"  "adcx %%rax, %%r11;"
  "mulx 32(%2), %%rax, %%r13;"  "adcx %%rax, %%r12;"
  "mulx 40(%2), %%rax, %%r14;"  "adcx %%rax, %%r13;"
  "mulx 48(%2), %%rax, %%r15;"  "adcx %%rax, %%r14;"
  /**************************/  "adcx  %%r8, %%r15;"

  "MULACC_mulxadx  8,  %%r9, %%r10, %%r11, %%r12, %%r13, %%r14, %%r15;"
  "MULACC_mulxadx 16, %%r10, %%r11, %%r12, %%r13, %%r14, %%r15,  %%r9;"
  "MULACC_mulxadx 24, %%r11, %%r12, %%r13, %%r14, %%r15,  %%r9, %%r10;"
  "MULACC_mulxadx 32, %%r12, %%r13, %%r14, %%r15,  %%r9, %%r10, %%r11;"
  "MULACC_mulxadx 40, %%r13, %%r14, %%r15,  %%r9, %%r10, %%r11, %%r12;"
  "MULACC_mulxadx 48, %%r14, %%r15,  %%r9, %%r10, %%r11, %%r12, %%r13;"
  ".purgem MULACC_mulxadx;"

  "movq %%r15, 56(%0);"
  "movq  %%r9, 64(%0);"
  "movq %%r10, 72(%0);"
  "movq %%r11, 80(%0);"
  "movq %%r12, 88(%0);"
  "movq %%r13, 96(%0);"
  "movq %%r14,104(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rdx", "%r8", "%r9",
    "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
}

DECL(void, intmul_mulx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "movq  0(%1), %%rdx;"
  "mulx  0(%2), %%rax,  %%r8;"  "movq %%rax, 0(%0);"
  "mulx  8(%2), %%rax,  %%r9;"  "addq %%rax,  %%r8;"
  "mulx 16(%2), %%rax, %%r10;"  "adcq %%rax,  %%r9;"
  "mulx 24(%2), %%rax, %%r11;"  "adcq %%rax, %%r10;"
  "mulx 32(%2), %%rax, %%r12;"  "adcq %%rax, %%r11;"
  "mulx 40(%2), %%rax, %%r13;"  "adcq %%rax, %%r12;"
  "mulx 48(%2), %%rax, %%r14;"  "adcq %%rax, %%r13;"
  /**************************/  "adcq    $0, %%r14;"

  ".macro MULACC_mulx I, R0, R1, R2, R3, R4, R5, R6;"
  "movq \\I(%1), %%rdx;"
  "mulx  0(%2), %%rax, %%rcx;"  "addq %%rax, \\R0;"  "movq \\R0, \\I(%0);"
  "mulx  8(%2), %%rax, %%rbx;"  "adcq %%rax, \\R1;"  "adcq $0, %%rbx;"  "addq %%rcx, \\R1;"
  "mulx 16(%2), %%rax, %%rcx;"  "adcq %%rax, \\R2;"  "adcq $0, %%rcx;"  "addq %%rbx, \\R2;"
  "mulx 24(%2), %%rax, %%rbx;"  "adcq %%rax, \\R3;"  "adcq $0, %%rbx;"  "addq %%rcx, \\R3;"
  "mulx 32(%2), %%rax, %%rcx;"  "adcq %%rax, \\R4;"  "adcq $0, %%rcx;"  "addq %%rbx, \\R4;"
  "mulx 40(%2), %%rax, %%rbx;"  "adcq %%rax, \\R5;"  "adcq $0, %%rbx;"  "addq %%rcx, \\R5;"
  "mulx 48(%2), %%rax,  \\R0;"  "adcq %%rax, \\R6;"  "adcq $0,  \\R0;"  "addq %%rbx, \\R6;"  "adcq $0, \\R0;"
  ".endm;"

  "MULACC_mulx  8,  %%r8,  %%r9, %%r10, %%r11, %%r12, %%r13, %%r14;"
  "MULACC_mulx 16,  %%r9, %%r10, %%r11, %%r12, %%r13, %%r14,  %%r8;"
  "MULACC_mulx 24, %%r10, %%r11, %%r12, %%r13, %%r14,  %%r8,  %%r9;"
  "MULACC_mulx 32, %%r11, %%r12, %%r13, %%r14,  %%r8,  %%r9, %%r10;"
  "MULACC_mulx 40, %%r12, %%r13, %%r14,  %%r8,  %%r9, %%r10, %%r11;"
  "MULACC_mulx 48, %%r13, %%r14,  %%r8,  %%r9, %%r10, %%r11, %%r12;"
  ".purgem MULACC_mulx;"

  "movq %%r14,  56(%0);"
  "movq  %%r8,  64(%0);"
  "movq  %%r9,  72(%0);"
  "movq %%r10,  80(%0);"
  "movq %%r11,  88(%0);"
  "movq %%r12,  96(%0);"
  "movq %%r13, 104(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intmul_mulq)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
    "movq  0(%1), %%rcx;"
    "movq  0(%2), %%rax;"  "mulq %%rcx;"  "movq %%rax, 0(%0);"  /***************/  "movq %%rdx,  %%r8;"
    "movq  8(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax,  %%r8;"  "adcq $0, %%rdx;"  "movq %%rdx,  %%r9;"
    "movq 16(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax,  %%r9;"  "adcq $0, %%rdx;"  "movq %%rdx, %%r10;"
    "movq 24(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r10;"  "adcq $0, %%rdx;"  "movq %%rdx, %%r11;"
    "movq 32(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r11;"  "adcq $0, %%rdx;"  "movq %%rdx, %%r12;"
    "movq 40(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r12;"  "adcq $0, %%rdx;"  "movq %%rdx, %%r13;"
    "movq 48(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r13;"  "adcq $0, %%rdx;"  "movq %%rdx, %%r14;"

    ".macro MULACC I;"
    "movq \\I(%1), %%rcx;"
    "movq  0(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax,  %%r8;"                     "movq  %%r8, \\I(%0);" "movq %%rdx,  %%r8;"  "adcq $0,  %%r8;"
    "movq  8(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax,  %%r9;"  "adcq $0, %%rdx;"  "addq  %%r9,  %%r8;"   "movq %%rdx,  %%r9;"  "adcq $0,  %%r9;"
    "movq 16(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r10;"  "adcq $0, %%rdx;"  "addq %%r10,  %%r9;"   "movq %%rdx, %%r10;"  "adcq $0, %%r10;"
    "movq 24(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r11;"  "adcq $0, %%rdx;"  "addq %%r11, %%r10;"   "movq %%rdx, %%r11;"  "adcq $0, %%r11;"
    "movq 32(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r12;"  "adcq $0, %%rdx;"  "addq %%r12, %%r11;"   "movq %%rdx, %%r12;"  "adcq $0, %%r12;"
    "movq 40(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r13;"  "adcq $0, %%rdx;"  "addq %%r13, %%r12;"   "movq %%rdx, %%r13;"  "adcq $0, %%r13;"
    "movq 48(%2), %%rax;"  "mulq %%rcx;"  "addq %%rax, %%r14;"  "adcq $0, %%rdx;"  "addq %%r14, %%r13;"   "movq %%rdx, %%r14;"  "adcq $0, %%r14;"
    ".endm;"

    "MULACC  8;"
    "MULACC 16;"
    "MULACC 24;"
    "MULACC 32;"
    "MULACC 40;"
    "MULACC 48;"
    ".purgem MULACC;"

    "movq  %%r8,  56(%0);"
    "movq  %%r9,  64(%0);"
    "movq %%r10,  72(%0);"
    "movq %%r11,  80(%0);"
    "movq %%r12,  88(%0);"
    "movq %%r13,  96(%0);"
    "movq %%r14, 104(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9",
  "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intmul)(argElement_1w c, argElement_1w a, argElement_1w b) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(intmul_mulxadx)(c,a,b);
#else /* No __ADX__ */
  FN(intmul_mulx)(c, a, b);
#endif
#else /* No __BMI2__ */
  FN(intmul_mulq)(c,a,b);
#endif
}

DECL(void, intsqr_mulxadx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movq  0(%1), %%rdx        ;" /* A[0]   */
  "mulx  %%rdx, %%rax, %%rbx ;" /* A[0]^2 */
  "movq  8(%1), %%rdx        ;" /* A[1]   */
  "mulx  %%rdx,  %%r8, %%r9  ;" /* A[1]^2 */
  "movq  %%rax,  0(%0) ;"
  "movq  %%rbx,  8(%0) ;"
  "movq   %%r8, 16(%0) ;"
  "movq   %%r9, 24(%0) ;"
  "movq 16(%1), %%rdx        ;" /* A[2]   */
  "mulx  %%rdx, %%r10, %%r11 ;" /* A[2]^2 */
  "movq 24(%1), %%rdx        ;" /* A[3]   */
  "mulx  %%rdx, %%r12, %%r13 ;" /* A[3]^2 */
  "movq  %%r10, 32(%0) ;"
  "movq  %%r11, 40(%0) ;"
  "movq  %%r12, 48(%0) ;"
  "movq  %%r13, 56(%0) ;"
  "movq 32(%1), %%rdx        ;" /* A[4]   */
  "mulx  %%rdx, %%rax, %%rbx ;" /* A[4]^2 */
  "movq 40(%1), %%rdx        ;" /* A[5]   */
  "mulx  %%rdx,  %%r8, %%r9  ;" /* A[5]^2 */
  "movq  %%rax, 64(%0) ;"
  "movq  %%rbx, 72(%0) ;"
  "movq   %%r8, 80(%0) ;"
  "movq   %%r9, 88(%0) ;"
  "movq 48(%1), %%rdx        ;" /* A[6]   */
  "mulx  %%rdx, %%r10, %%r11 ;" /* A[6]^2 */
  "movq  %%r10, 96(%0) ;"
  "movq  %%r11,104(%0) ;"

  "movq  0(%1), %%rdx  ;"       /* A[0]     */
  "mulx  8(%1),  %%r8,  %%r9 ;" /* A[0]A[1] */  "xorl %%r10d,%%r10d;"  "adox  %%r8,  %%r8 ;"
  "mulx 16(%1), %%r10, %%r11 ;" /* A[0]A[2] */  "adcx %%r10,  %%r9 ;"  "adox  %%r9,  %%r9 ;"
  "mulx 24(%1), %%r12, %%r13 ;" /* A[0]A[3] */  "adcx %%r12, %%r11 ;"  "adox %%r11, %%r11 ;"
  "mulx 32(%1), %%r14, %%rax ;" /* A[0]A[4] */  "adcx %%r14, %%r13 ;"  "adox %%r13, %%r13 ;"
  "mulx 40(%1), %%r10, %%rbx ;" /* A[0]A[5] */  "adcx %%r10, %%rax ;"  "adox %%rax, %%rax ;"
  "mulx 48(%1), %%r12, %%rcx ;" /* A[0]A[6] */  "adcx %%r12, %%rbx ;"  "adox %%rbx, %%rbx ;"
  "movq 24(%1), %%rdx        ;" /* A[3]     */  "movq    $0, %%r12 ;"  "movq    $0, %%r10 ;"
  "mulx 32(%1), %%r14, %%rdx ;" /* A[3]A[4] */  "adcx %%r14, %%rcx ;"  "adox %%rcx, %%rcx ;"
  /******************************************/  "adcx %%r12, %%rdx ;"  "adox %%rdx, %%rdx ;"
  /*****************************************************************/  "adox %%r12, %%r10 ;"

  "addq  8(%0),  %%r8 ;" "movq  %%r8,  8(%0) ;"
  "adcq 16(%0),  %%r9 ;" "movq  %%r9, 16(%0) ;"
  "adcq 24(%0), %%r11 ;" "movq %%r11, 24(%0) ;"
  "adcq 32(%0), %%r13 ;" "movq %%r13, 32(%0) ;"
  "adcq 40(%0), %%rax ;" "movq %%rax, 40(%0) ;"
  "adcq 48(%0), %%rbx ;" "movq %%rbx, 48(%0) ;"
  "adcq 56(%0), %%rcx ;" "movq %%rcx, 56(%0) ;"
  "adcq 64(%0), %%rdx ;" "movq %%rdx, 64(%0) ;"
  "adcq 72(%0), %%r10 ;" "movq %%r10, 72(%0) ;"

  "movq  8(%1), %%rdx ;"        /* A[1]     */
  "mulx 16(%1),  %%r8,  %%r9 ;" /* A[1]A[2] */  "xorl %%r10d,%%r10d;"  "adox  %%r8,  %%r8 ;"
  "mulx 24(%1), %%r10, %%r11 ;" /* A[1]A[3] */  "adcx %%r10,  %%r9 ;"  "adox  %%r9,  %%r9 ;"
  "mulx 32(%1), %%r12, %%r13 ;" /* A[1]A[4] */  "adcx %%r12, %%r11 ;"  "adox %%r11, %%r11 ;"
  "mulx 40(%1), %%r14, %%rax ;" /* A[1]A[5] */  "adcx %%r14, %%r13 ;"  "adox %%r13, %%r13 ;"
  "mulx 48(%1), %%r10, %%rbx ;" /* A[1]A[6] */  "adcx %%r10, %%rax ;"  "adox %%rax, %%rax ;"
  "movq 40(%1), %%rdx ;"        /* A[5]     */
  "mulx 24(%1), %%r12, %%rcx ;" /* A[5]A[3] */  "adcx %%r12, %%rbx ;"  "adox %%rbx, %%rbx ;"
  "mulx 32(%1), %%r14, %%rdx ;" /* A[5]A[4] */  "adcx %%r14, %%rcx ;"  "adox %%rcx, %%rcx ;"
  /******************************************/  "movq    $0, %%r12 ;"  "movq    $0, %%r10 ;"
  /******************************************/  "adcx %%r12, %%rdx ;"  "adox %%rdx, %%rdx ;"
  /*****************************************************************/  "adox %%r12, %%r10 ;"

  "addq 24(%0),  %%r8 ;" "movq  %%r8, 24(%0) ;"
  "adcq 32(%0),  %%r9 ;" "movq  %%r9, 32(%0) ;"
  "adcq 40(%0), %%r11 ;" "movq %%r11, 40(%0) ;"
  "adcq 48(%0), %%r13 ;" "movq %%r13, 48(%0) ;"
  "adcq 56(%0), %%rax ;" "movq %%rax, 56(%0) ;"
  "adcq 64(%0), %%rbx ;" "movq %%rbx, 64(%0) ;"
  "adcq 72(%0), %%rcx ;" "movq %%rcx, 72(%0) ;"
  "adcq 80(%0), %%rdx ;" "movq %%rdx, 80(%0) ;"
  "adcq 88(%0), %%r10 ;" "movq %%r10, 88(%0) ;"

  "movq 16(%1), %%rdx ;"        /* A[2]     */
  "mulx 24(%1),  %%r8,  %%r9 ;" /* A[2]A[3] */  "xorl %%r10d,%%r10d;"  "adox  %%r8,  %%r8 ;"
  "mulx 32(%1), %%r10, %%r11 ;" /* A[2]A[4] */  "adcx %%r10,  %%r9 ;"  "adox  %%r9,  %%r9 ;"
  "mulx 40(%1), %%r12, %%r13 ;" /* A[2]A[5] */  "adcx %%r12, %%r11 ;"  "adox %%r11, %%r11 ;"
  "mulx 48(%1), %%r14, %%rax ;" /* A[2]A[6] */  "adcx %%r14, %%r13 ;"  "adox %%r13, %%r13 ;"
  "movq 48(%1), %%rdx ;"        /* A[6]     */
  "mulx 24(%1), %%r10, %%rbx ;" /* A[6]A[3] */  "adcx %%r10, %%rax ;"  "adox %%rax, %%rax ;"
  "mulx 32(%1), %%r12, %%rcx ;" /* A[6]A[4] */  "adcx %%r12, %%rbx ;"  "adox %%rbx, %%rbx ;"
  "mulx 40(%1), %%r14, %%rdx ;" /* A[6]A[5] */  "adcx %%r14, %%rcx ;"  "adox %%rcx, %%rcx ;"
  /******************************************/  "movq    $0, %%r12 ;"  "movq    $0, %%r10 ;"
  /******************************************/  "adcx %%r12, %%rdx ;"  "adox %%rdx, %%rdx ;"
  /*****************************************************************/  "adox %%r12, %%r10 ;"

  "addq  40(%0),  %%r8 ;" "movq  %%r8, 40(%0) ;"
  "adcq  48(%0),  %%r9 ;" "movq  %%r9, 48(%0) ;"
  "adcq  56(%0), %%r11 ;" "movq %%r11, 56(%0) ;"
  "adcq  64(%0), %%r13 ;" "movq %%r13, 64(%0) ;"
  "adcq  72(%0), %%rax ;" "movq %%rax, 72(%0) ;"
  "adcq  80(%0), %%rbx ;" "movq %%rbx, 80(%0) ;"
  "adcq  88(%0), %%rcx ;" "movq %%rcx, 88(%0) ;"
  "adcq  96(%0), %%rdx ;" "movq %%rdx, 96(%0) ;"
  "adcq 104(%0), %%r10 ;" "movq %%r10,104(%0) ;"
  :
  : "r"  (c), "r" (a)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intsqr_mulx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movq  0(%1), %%rdx ;"        /* A[0]   */
  "mulx  %%rdx, %%rax, %%rbx ;" /* A[0]^2 */
  "movq  8(%1), %%rdx ;"        /* A[1]   */
  "mulx  %%rdx,  %%r8,  %%r9 ;" /* A[1]^2 */
  "movq  %%rax,   (%0) ;"
  "movq  %%rbx,  8(%0) ;"
  "movq   %%r8, 16(%0) ;"
  "movq   %%r9, 24(%0) ;"
  "movq 16(%1), %%rdx ;"        /* A[2]   */
  "mulx  %%rdx, %%r10, %%r11 ;" /* A[2]^2 */
  "movq 24(%1), %%rdx ;"        /* A[3]   */
  "mulx  %%rdx, %%r12, %%r13 ;" /* A[3]^2 */
  "movq  %%r10, 32(%0) ;"
  "movq  %%r11, 40(%0) ;"
  "movq  %%r12, 48(%0) ;"
  "movq  %%r13, 56(%0) ;"
  "movq 32(%1), %%rdx        ;" /* A[4]   */
  "mulx  %%rdx, %%rax, %%rbx ;" /* A[4]^2 */
  "movq 40(%1), %%rdx ;"        /* A[5]   */
  "mulx  %%rdx,  %%r8,  %%r9 ;" /* A[5]^2 */
  "movq  %%rax, 64(%0) ;"
  "movq  %%rbx, 72(%0) ;"
  "movq   %%r8, 80(%0) ;"
  "movq   %%r9, 88(%0) ;"
  "movq 48(%1), %%rdx  ;"       /* A[6]   */
  "mulx  %%rdx, %%r10, %%r11 ;" /* A[6]^2 */
  "movq  %%r10, 96(%0) ;"
  "movq  %%r11,104(%0) ;"

  "movq   (%1), %%rdx ;"        /* A[0]     */
  "mulx  8(%1),  %%r8,  %%r9 ;" /* A[0]A[1] */
  "mulx 16(%1), %%r10, %%r11 ;" /* A[0]A[2] */  "addq %%r10,  %%r9 ;"
  "mulx 24(%1), %%r12, %%r13 ;" /* A[0]A[3] */  "adcq %%r12, %%r11 ;"
  "mulx 32(%1), %%r14, %%rax ;" /* A[0]A[4] */  "adcq %%r14, %%r13 ;"
  "mulx 40(%1), %%r10, %%rbx ;" /* A[0]A[5] */  "adcq %%r10, %%rax ;"
  "mulx 48(%1), %%r12, %%rcx ;" /* A[0]A[6] */  "adcq %%r12, %%rbx ;"
  "movq 24(%1), %%rdx ;"        /* A[3]     */
  "mulx 32(%1), %%r14, %%rdx ;" /* A[3]A[4] */  "adcq %%r14, %%rcx ;"
  /******************************************/  "adcq    $0, %%rdx ;"
  "xorq %%r10, %%r10 ;"
  "shldq $1, %%rdx, %%r10 ;"
  "shldq $1, %%rcx, %%rdx ;"
  "shldq $1, %%rbx, %%rcx ;"
  "shldq $1, %%rax, %%rbx ;"
  "shldq $1, %%r13, %%rax ;"
  "shldq $1, %%r11, %%r13 ;"
  "shldq $1,  %%r9, %%r11 ;"
  "shldq $1,  %%r8,  %%r9 ;"
  "shlq  $1,  %%r8 ;"

  "addq  8(%0),  %%r8 ;"  "movq  %%r8,  8(%0) ;"
  "adcq 16(%0),  %%r9 ;"  "movq  %%r9, 16(%0) ;"
  "adcq 24(%0), %%r11 ;"  "movq %%r11, 24(%0) ;"
  "adcq 32(%0), %%r13 ;"  "movq %%r13, 32(%0) ;"
  "adcq 40(%0), %%rax ;"  "movq %%rax, 40(%0) ;"
  "adcq 48(%0), %%rbx ;"  "movq %%rbx, 48(%0) ;"
  "adcq 56(%0), %%rcx ;"  "movq %%rcx, 56(%0) ;"
  "adcq 64(%0), %%rdx ;"  "movq %%rdx, 64(%0) ;"
  "adcq 72(%0), %%r10 ;"  "movq %%r10, 72(%0) ;"

  "movq  8(%1), %%rdx ;"        /* A[1]     */
  "mulx 16(%1),  %%r8,  %%r9 ;" /* A[1]A[2] */
  "mulx 24(%1), %%r10, %%r11 ;" /* A[1]A[3] */   "addq %%r10,  %%r9 ;"
  "mulx 32(%1), %%r12, %%r13 ;" /* A[1]A[4] */   "adcq %%r12, %%r11 ;"
  "mulx 40(%1), %%r14, %%rax ;" /* A[1]A[5] */   "adcq %%r14, %%r13 ;"
  "mulx 48(%1), %%r10, %%rbx ;" /* A[1]A[6] */   "adcq %%r10, %%rax ;"
  "movq 40(%1), %%rdx ;"        /* A[5]     */
  "mulx 24(%1), %%r12, %%rcx ;" /* A[5]A[3] */   "adcq %%r12, %%rbx ;"
  "mulx 32(%1), %%r14, %%rdx ;" /* A[5]A[4] */   "adcq %%r14, %%rcx ;"
  /******************************************/   "adcq    $0, %%rdx ;"

  "xorq %%r10, %%r10 ;"
  "shldq $1, %%rdx, %%r10 ;"
  "shldq $1, %%rcx, %%rdx ;"
  "shldq $1, %%rbx, %%rcx ;"
  "shldq $1, %%rax, %%rbx ;"
  "shldq $1, %%r13, %%rax ;"
  "shldq $1, %%r11, %%r13 ;"
  "shldq $1,  %%r9, %%r11 ;"
  "shldq $1,  %%r8,  %%r9 ;"
  "shlq  $1,  %%r8 ;"

  "addq 24(%0),  %%r8 ;"  "movq  %%r8, 24(%0) ;"
  "adcq 32(%0),  %%r9 ;"  "movq  %%r9, 32(%0) ;"
  "adcq 40(%0), %%r11 ;"  "movq %%r11, 40(%0) ;"
  "adcq 48(%0), %%r13 ;"  "movq %%r13, 48(%0) ;"
  "adcq 56(%0), %%rax ;"  "movq %%rax, 56(%0) ;"
  "adcq 64(%0), %%rbx ;"  "movq %%rbx, 64(%0) ;"
  "adcq 72(%0), %%rcx ;"  "movq %%rcx, 72(%0) ;"
  "adcq 80(%0), %%rdx ;"  "movq %%rdx, 80(%0) ;"
  "adcq 88(%0), %%r10 ;"  "movq %%r10, 88(%0) ;"

  "movq 16(%1), %%rdx ;"        /* A[2]     */
  "mulx 24(%1),  %%r8,  %%r9 ;" /* A[2]A[3] */
  "mulx 32(%1), %%r10, %%r11 ;" /* A[2]A[4] */  "addq %%r10,  %%r9 ;"
  "mulx 40(%1), %%r12, %%r13 ;" /* A[2]A[5] */  "adcq %%r12, %%r11 ;"
  "mulx 48(%1), %%r14, %%rax ;" /* A[2]A[6] */  "adcq %%r14, %%r13 ;"
  "movq 48(%1), %%rdx ;"        /* A[6]     */
  "mulx 24(%1), %%r10, %%rbx ;" /* A[6]A[3] */  "adcq %%r10, %%rax ;"
  "mulx 32(%1), %%r12, %%rcx ;" /* A[6]A[4] */  "adcq %%r12, %%rbx ;"
  "mulx 40(%1), %%r14, %%rdx ;" /* A[6]A[5] */  "adcq %%r14, %%rcx ;"
  /******************************************/  "adcq    $0, %%rdx ;"
  "xorq %%r10, %%r10 ;"
  "shldq $1, %%rdx, %%r10 ;"
  "shldq $1, %%rcx, %%rdx ;"
  "shldq $1, %%rbx, %%rcx ;"
  "shldq $1, %%rax, %%rbx ;"
  "shldq $1, %%r13, %%rax ;"
  "shldq $1, %%r11, %%r13 ;"
  "shldq $1,  %%r9, %%r11 ;"
  "shldq $1,  %%r8,  %%r9 ;"
  "shlq  $1,  %%r8 ;"

  "addq  40(%0),  %%r8 ;"   "movq  %%r8, 40(%0) ;"
  "adcq  48(%0),  %%r9 ;"   "movq  %%r9, 48(%0) ;"
  "adcq  56(%0), %%r11 ;"   "movq %%r11, 56(%0) ;"
  "adcq  64(%0), %%r13 ;"   "movq %%r13, 64(%0) ;"
  "adcq  72(%0), %%rax ;"   "movq %%rax, 72(%0) ;"
  "adcq  80(%0), %%rbx ;"   "movq %%rbx, 80(%0) ;"
  "adcq  88(%0), %%rcx ;"   "movq %%rcx, 88(%0) ;"
  "adcq  96(%0), %%rdx ;"   "movq %%rdx, 96(%0) ;"
  "adcq 104(%0), %%r10 ;"   "movq %%r10,104(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intsqr_mulq)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
    ".macro times2;"
    "xorl %%ecx, %%ecx;"
    "shldq $1,%%r15,%%rcx;"
    "shldq $1,%%r14,%%r15;"
    "shldq $1,%%r13,%%r14;"
    "shldq $1,%%r12,%%r13;"
    "shldq $1,%%r11,%%r12;"
    "shldq $1,%%r10,%%r11;"
    "shldq $1, %%r9,%%r10;"
    "shldq $1, %%r8, %%r9;"
    "shlq  $1, %%r8;"
    ".endm;"

    "movq  0(%1),%%rcx;"
    "movq  8(%1),%%rax;" "mulq %%rcx;"  "movq %%rax, %%r8;"  /**************/  "movq %%rdx, %%r9;"
    "movq 16(%1),%%rax;" "mulq %%rcx;"  "addq %%rax, %%r9;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r10;"
    "movq 24(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r10;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r11;"
    "movq 32(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r11;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r12;"
    "movq 40(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r12;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r13;"
    "movq 48(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r13;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r14;"
    "movq 24(%1),%%rax;" "mulq 32(%1);" "addq %%rax,%%r14;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r15;"

    "times2;"

    "movq %%rcx,72(%0);"
    "movq %%r15,64(%0);"
    "movq %%r14,56(%0);"
    "movq %%r13,48(%0);"
    "movq %%r12,40(%0);"
    "movq %%r11,32(%0);"
    "movq %%r10,24(%0);"
    "movq  %%r9,16(%0);"
    "movq  %%r8, 8(%0);"

    "movq  8(%1),%%rcx;"
    "movq 16(%1),%%rax;" "mulq %%rcx;"  "movq %%rax, %%r8;"  /**************/  "movq %%rdx, %%r9;"
    "movq 24(%1),%%rax;" "mulq %%rcx;"  "addq %%rax, %%r9;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r10;"
    "movq 32(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r10;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r11;"
    "movq 40(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r11;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r12;"
    "movq 48(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r12;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r13;"
    "movq 40(%1),%%rcx;"
    "movq 24(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r13;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r14;"
    "movq 32(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r14;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r15;"

    "times2;"

    "addq 24(%0), %%r8;"  "movq  %%r8,24(%0);"
    "adcq 32(%0), %%r9;"  "movq  %%r9,32(%0);"
    "adcq 40(%0),%%r10;"  "movq %%r10,40(%0);"
    "adcq 48(%0),%%r11;"  "movq %%r11,48(%0);"
    "adcq 56(%0),%%r12;"  "movq %%r12,56(%0);"
    "adcq 64(%0),%%r13;"  "movq %%r13,64(%0);"
    "adcq 72(%0),%%r14;"  "movq %%r14,72(%0);"
    "adcq     $0,%%r15;"  "movq %%r15,80(%0);"
    "adcq     $0,%%rcx;"  "movq %%rcx,88(%0);"

    "movq 16(%1),%%rcx;"
    "movq 24(%1),%%rax;" "mulq %%rcx;"  "movq %%rax, %%r8;"  /**************/  "movq %%rdx, %%r9;"
    "movq 32(%1),%%rax;" "mulq %%rcx;"  "addq %%rax, %%r9;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r10;"
    "movq 40(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r10;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r11;"
    "movq 48(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r11;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r12;"
    "movq 48(%1),%%rcx;"
    "movq 24(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r12;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r13;"
    "movq 32(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r13;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r14;"
    "movq 40(%1),%%rax;" "mulq %%rcx;"  "addq %%rax,%%r14;"  "adcq $0,%%rdx;"  "movq %%rdx,%%r15;"

    "times2;"
    ".purgem times2;"

    "addq 40(%0), %%r8;"  "movq  %%r8, 40(%0);"
    "adcq 48(%0), %%r9;"  "movq  %%r9, 48(%0);"
    "adcq 56(%0),%%r10;"  "movq %%r10, 56(%0);"
    "adcq 64(%0),%%r11;"  "movq %%r11, 64(%0);"
    "adcq 72(%0),%%r12;"  "movq %%r12, 72(%0);"
    "adcq 80(%0),%%r13;"  "movq %%r13, 80(%0);"
    "adcq 88(%0),%%r14;"  "movq %%r14, 88(%0);"
    "adcq     $0,%%r15;"  "movq %%r15, 96(%0);"
    "adcq     $0,%%rcx;"  "movq %%rcx,104(%0);"

    "movq  0(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r8;"  "movq %%rdx, %%r9;"
    "movq  8(%1),%%rax;"  "mulq %%rax;"  "movq %%rax,%%r10;"  "movq %%rdx,%%r11;"
    "movq 16(%1),%%rax;"  "mulq %%rax;"  "movq %%rax,%%r12;"  "movq %%rdx,%%r13;"
    "movq 24(%1),%%rax;"  "mulq %%rax;"  "movq %%rax,%%r14;"  "movq %%rdx,%%r15;"
    "xorl %%ecx, %%ecx;"  "movq  %%r8, 0(%0);"
    "addq  8(%0), %%r9;"  "movq  %%r9, 8(%0);"
    "adcq 16(%0),%%r10;"  "movq %%r10,16(%0);"
    "adcq 24(%0),%%r11;"  "movq %%r11,24(%0);"
    "adcq 32(%0),%%r12;"  "movq %%r12,32(%0);"
    "adcq 40(%0),%%r13;"  "movq %%r13,40(%0);"
    "adcq 48(%0),%%r14;"  "movq %%r14,48(%0);"
    "adcq 56(%0),%%r15;"  "movq %%r15,56(%0);"
    "setc %%cl;"

    "movq 32(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r8;" "movq %%rdx, %%r9;"
    "movq 40(%1),%%rax;"  "mulq %%rax;"  "movq %%rax,%%r10;" "movq %%rdx,%%r11;"
    "movq 48(%1),%%rax;"  "mulq %%rax;"  "movq %%rax,%%r12;" "movq %%rdx,%%r13;"
    "btl $0, %%ecx;"
    "adcq  64(%0), %%r8;"  "movq  %%r8, 64(%0);"
    "adcq  72(%0), %%r9;"  "movq  %%r9, 72(%0);"
    "adcq  80(%0),%%r10;"  "movq %%r10, 80(%0);"
    "adcq  88(%0),%%r11;"  "movq %%r11, 88(%0);"
    "adcq  96(%0),%%r12;"  "movq %%r12, 96(%0);"
    "adcq 104(%0),%%r13;"  "movq %%r13,104(%0);"
  :
  : "r"  (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8",
    "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
}

DECL(void, intsqr)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(intsqr_mulxadx)(c, a);
#else /* No __ADX__ */
  FN(intsqr_mulx)(c, a);
#endif
#else /* No __BMI2__ */
  FN(intsqr_mulq)(c, a);
#endif
}

DECL(void, reduce_adx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
    /**
     * (   ,2C13,2C12,2C11,2C10|C10,C9,C8, C7) + (C6,...,C0)
     * (r14, r13, r12, r11,     r10,r9,r8,r15)
     */
    "movq 80(%1),%%rax; movq %%rax,%%r10;"
    "movq $0xffffffff00000000, %%r8;"
    "andq %%r8,%%r10;"

    "movq $0,%%r14;"
    "movq 104(%1),%%r13; shldq $1,%%r13,%%r14;"
    "movq  96(%1),%%r12; shldq $1,%%r12,%%r13;"
    "movq  88(%1),%%r11; shldq $1,%%r11,%%r12;"
    "movq  72(%1), %%r9; shldq $1,%%r10,%%r11;"
    "movq  64(%1), %%r8; shlq  $1,%%r10;"
    "movq $0xffffffff,%%r15; andq %%r15,%%rax; orq %%rax,%%r10;"
    "movq  56(%1),%%r15;"

    "xorl  %%eax,%%eax;"
    "adcx  0(%1),%%r15; movq %%r15, 0(%0); movq  56(%1),%%r15;"
    "adcx  8(%1), %%r8; movq  %%r8, 8(%0); movq  64(%1), %%r8;"
    "adcx 16(%1), %%r9; movq  %%r9,16(%0); movq  72(%1), %%r9;"
    "adcx 24(%1),%%r10; movq %%r10,24(%0); movq  80(%1),%%r10;"
    "adcx 32(%1),%%r11; movq %%r11,32(%0); movq  88(%1),%%r11;"
    "adcx 40(%1),%%r12; movq %%r12,40(%0); movq  96(%1),%%r12;"
    "adcx 48(%1),%%r13; movq %%r13,48(%0); movq 104(%1),%%r13;"
    "adcx  %%rax,%%r14;"

    /**
     * (c10c9,c9c8,c8c7,c7c13,c13c12,c12c11,c11c10) + (c6,...,c0)
     * (   r9,  r8, r15,  r13,   r12,   r11,   r10)
     */
    "movq %%r10, %%rax;"
    "shrdq $32,%%r11,%%r10;"
    "shrdq $32,%%r12,%%r11;"
    "shrdq $32,%%r13,%%r12;"
    "shrdq $32,%%r15,%%r13;"
    "shrdq $32, %%r8,%%r15;"
    "shrdq $32, %%r9, %%r8;"
    "shrdq $32,%%rax, %%r9;"

    "xorl  %%eax,%%eax;"
    "adcx  0(%0),%%r10;"
    "adcx  8(%0),%%r11;"
    "adcx 16(%0),%%r12;"
    "adcx 24(%0),%%r13;"
    "adcx 32(%0),%%r15;"
    "adcx 40(%0), %%r8;"
    "adcx 48(%0), %%r9;"
    "adcx  %%rax,%%r14;"

    /**
     * ( c7) + (c6,...,c0)
     * (r14)
     */
    "movq %%r14,%%rax; shlq $32,%%rax;"
    "clc;"
    "adcx %%r14,%%r10; movq  $0,%%r14;"
    "adcx %%r14,%%r11;"
    "adcx %%r14,%%r12;"
    "adcx %%rax,%%r13;"
    "adcx %%r14,%%r15;"
    "adcx %%r14, %%r8;"
    "adcx %%r14, %%r9;"
    "adcx %%r14,%%r14;"

    "movq %%r14,%%rax; shlq $32,%%rax;"
    "clc;"
    "adcx %%r14,%%r10; movq %%r10, 0(%0); movq  $0,%%r14;"
    "adcx %%r14,%%r11; movq %%r11, 8(%0);"
    "adcx %%r14,%%r12; movq %%r12,16(%0);"
    "adcx %%rax,%%r13; movq %%r13,24(%0);"
    "adcx %%r14,%%r15; movq %%r15,32(%0);"
    "adcx %%r14, %%r8; movq  %%r8,40(%0);"
    "adcx %%r14, %%r9; movq  %%r9,48(%0);"
    :
    : "r"(c), "r"(a)
    : "memory", "cc", "%rax", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13",
      "%r14", "%r15");
}

DECL(void, reduce_leg)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
     /**
      * (   ,2C13,2C12,2C11,2C10|C10,C9,C8, C7) + (C6,...,C0)
      * (r14, r13, r12, r11,     r10,r9,r8,r15)
      */
     "movq 80(%1),%%rax; movq %%rax,%%r10;"
     "movq $0xffffffff00000000, %%r8;"
     "andq %%r8,%%r10;"

     "movq $0,%%r14;"
     "movq 104(%1),%%r13; shldq $1,%%r13,%%r14;"
     "movq  96(%1),%%r12; shldq $1,%%r12,%%r13;"
     "movq  88(%1),%%r11; shldq $1,%%r11,%%r12;"
     "movq  72(%1), %%r9; shldq $1,%%r10,%%r11;"
     "movq  64(%1), %%r8; shlq  $1,%%r10;"
     "movq $0xffffffff,%%r15; andq %%r15,%%rax; orq %%rax,%%r10;"
     "movq  56(%1),%%r15;"

     "addq  0(%1),%%r15; movq %%r15, 0(%0); movq  56(%1),%%r15;"
     "adcq  8(%1), %%r8; movq  %%r8, 8(%0); movq  64(%1), %%r8;"
     "adcq 16(%1), %%r9; movq  %%r9,16(%0); movq  72(%1), %%r9;"
     "adcq 24(%1),%%r10; movq %%r10,24(%0); movq  80(%1),%%r10;"
     "adcq 32(%1),%%r11; movq %%r11,32(%0); movq  88(%1),%%r11;"
     "adcq 40(%1),%%r12; movq %%r12,40(%0); movq  96(%1),%%r12;"
     "adcq 48(%1),%%r13; movq %%r13,48(%0); movq 104(%1),%%r13;"
     "adcq     $0,%%r14;"

     /**
      * (c10c9,c9c8,c8c7,c7c13,c13c12,c12c11,c11c10) + (c6,...,c0)
      * (   r9,  r8, r15,  r13,   r12,   r11,   r10)
      */
     "movq %%r10, %%rax;"
     "shrdq $32,%%r11,%%r10;"
     "shrdq $32,%%r12,%%r11;"
     "shrdq $32,%%r13,%%r12;"
     "shrdq $32,%%r15,%%r13;"
     "shrdq $32, %%r8,%%r15;"
     "shrdq $32, %%r9, %%r8;"
     "shrdq $32,%%rax, %%r9;"

     "addq  0(%0),%%r10;"
     "adcq  8(%0),%%r11;"
     "adcq 16(%0),%%r12;"
     "adcq 24(%0),%%r13;"
     "adcq 32(%0),%%r15;"
     "adcq 40(%0), %%r8;"
     "adcq 48(%0), %%r9;"
     "adcq     $0,%%r14;"

     /**
      * ( c7) + (c6,...,c0)
      * (r14)
      */
     "movq %%r14,%%rax; shlq $32,%%rax;"
     "addq %%r14,%%r10; movq  $0,%%r14;"
     "adcq    $0,%%r11;"
     "adcq    $0,%%r12;"
     "adcq %%rax,%%r13;"
     "adcq    $0,%%r15;"
     "adcq    $0, %%r8;"
     "adcq    $0, %%r9;"
     "adcq    $0,%%r14;"

     "movq %%r14,%%rax; shlq $32,%%rax;"
     "addq %%r14,%%r10; movq %%r10, 0(%0);"
     "adcq    $0,%%r11; movq %%r11, 8(%0);"
     "adcq    $0,%%r12; movq %%r12,16(%0);"
     "adcq %%rax,%%r13; movq %%r13,24(%0);"
     "adcq    $0,%%r15; movq %%r15,32(%0);"
     "adcq    $0, %%r8; movq  %%r8,40(%0);"
     "adcq    $0, %%r9; movq  %%r9,48(%0);"
     :
     : "r"(c), "r"(a)
     : "memory", "cc", "%rax", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13",
       "%r14", "%r15");
}

DECL(void, reduce)(argElement_1w c, argElement_1w a) {
#ifdef __ADX__
  FN(reduce_adx)(c,a);
#else /* No __ADX__ */
  FN(reduce_leg)(c,a);
#endif
}

DECL(void, intmul2_mulxadx)(argElement_1w c, argElement_1w a, argElement_1w b)  {
  FN(intmul_mulxadx)(c,a,b);
  FN(intmul_mulxadx)(c+2*SIZE_FP448,a+SIZE_FP448,b+SIZE_FP448);
}

DECL(void, intmul2_mulx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  FN(intmul_mulx)(c,a,b);
  FN(intmul_mulx)(c+2*SIZE_FP448,a+SIZE_FP448,b+SIZE_FP448);
}

DECL(void, intmul2_mulq)(argElement_1w c, argElement_1w a, argElement_1w b) {
  FN(intmul_mulq)(c,a,b);
  FN(intmul_mulq)(c+2*SIZE_FP448,a+SIZE_FP448,b+SIZE_FP448);
}

DECL(void, intmul2)(argElement_1w c, argElement_1w a, argElement_1w b) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(intmul2_mulxadx)(c,a,b);
#else /* No __ADX__ */
  FN(intmul2_mulx)(c, a, b);
#endif
#else /* No __BMI2__ */
  FN(intmul2_mulq)(c,a,b);
#endif
}

DECL(void, intsqr2_mulxadx)(argElement_1w c, argElement_1w a) {
  FN(intsqr_mulxadx)(c,a);
  FN(intsqr_mulxadx)(c+2*SIZE_FP448,a+SIZE_FP448);
}

DECL(void, intsqr2_mulx)(argElement_1w c, argElement_1w a) {
  FN(intsqr_mulx)(c,a);
  FN(intsqr_mulx)(c+2*SIZE_FP448,a+SIZE_FP448);
}

DECL(void, intsqr2_mulq)(argElement_1w c, argElement_1w a) {
  FN(intsqr_mulq)(c,a);
  FN(intsqr_mulq)(c+2*SIZE_FP448,a+SIZE_FP448);
}

DECL(void, intsqr2)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(intsqr2_mulxadx)(c,a);
#else /* No __ADX__ */
  FN(intsqr2_mulx)(c, a);
#endif
#else /* No __BMI2__ */
  FN(intsqr2_mulq)(c,a);
#endif
}

DECL(void, reduce2_adx)(argElement_1w c, argElement_1w a) {
  FN(reduce_adx)(c,a);
  FN(reduce_adx)(c+2*SIZE_FP448,a+SIZE_FP448);
}

DECL(void, reduce2_leg)(argElement_1w c, argElement_1w a) {
  FN(reduce_leg)(c,a);
  FN(reduce_leg)(c+2*SIZE_FP448,a+SIZE_FP448);
}

DECL(void, reduce2)(argElement_1w c, argElement_1w a) {
#ifdef __ADX__
  FN(reduce2_adx)(c,a);
#else /* No __ADX__ */
  FN(reduce2_leg)(c,a);
#endif
}

DECL(void, add_adx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "xorl %%eax, %%eax ;"
  "movq $0x100000000, %%rdx ;"

  "movq  0(%2),  %%r8 ;"
  "movq  8(%2),  %%r9 ;"
  "movq 16(%2), %%r10 ;"
  "movq 24(%2), %%r11 ;"
  "movq 32(%2), %%r12 ;"
  "movq 40(%2), %%r13 ;"
  "movq 48(%2), %%r14 ;"

  "adcx  0(%1),  %%r8 ;"
  "adcx  8(%1),  %%r9 ;"
  "adcx 16(%1), %%r10 ;"
  "adcx 24(%1), %%r11 ;"
  "adcx 32(%1), %%r12 ;"
  "adcx 40(%1), %%r13 ;"
  "adcx 48(%1), %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "adcx  %%rax,  %%r8 ;"
  "adcx  %%rax,  %%r9 ;"
  "adcx  %%rax, %%r10 ;"
  "adcx  %%rcx, %%r11 ;"
  "adcx  %%rax, %%r12 ;"
  "adcx  %%rax, %%r13 ;"
  "adcx  %%rax, %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "adcx  %%rax,  %%r8 ;"
  "adcx  %%rax,  %%r9 ;"
  "adcx  %%rax, %%r10 ;"
  "adcx  %%rcx, %%r11 ;"
  "adcx  %%rax, %%r12 ;"
  "adcx  %%rax, %%r13 ;"
  "adcx  %%rax, %%r14 ;"

  "movq  %%r8,  0(%0) ;"
  "movq  %%r9,  8(%0) ;"
  "movq %%r10, 16(%0) ;"
  "movq %%r11, 24(%0) ;"
  "movq %%r12, 32(%0) ;"
  "movq %%r13, 40(%0) ;"
  "movq %%r14, 48(%0) ;"
  :
  : "r" (c), "r" (a), "r"  (b)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, add_adc)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "xorl %%eax, %%eax ;"
  "movq $0x100000000, %%rdx ;"

  "movq  0(%2),  %%r8 ;"
  "movq  8(%2),  %%r9 ;"
  "movq 16(%2), %%r10 ;"
  "movq 24(%2), %%r11 ;"
  "movq 32(%2), %%r12 ;"
  "movq 40(%2), %%r13 ;"
  "movq 48(%2), %%r14 ;"

  "addq  0(%1),  %%r8 ;"
  "adcq  8(%1),  %%r9 ;"
  "adcq 16(%1), %%r10 ;"
  "adcq 24(%1), %%r11 ;"
  "adcq 32(%1), %%r12 ;"
  "adcq 40(%1), %%r13 ;"
  "adcq 48(%1), %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "adcq  %%rax,  %%r8 ;"
  "adcq  %%rax,  %%r9 ;"
  "adcq  %%rax, %%r10 ;"
  "adcq  %%rcx, %%r11 ;"
  "adcq  %%rax, %%r12 ;"
  "adcq  %%rax, %%r13 ;"
  "adcq  %%rax, %%r14 ;"

  "movq   $0x0, %%rcx ;"
  "cmovc %%rdx, %%rcx ;"
  "adcq  %%rax,  %%r8 ;"
  "adcq  %%rax,  %%r9 ;"
  "adcq  %%rax, %%r10 ;"
  "adcq  %%rcx, %%r11 ;"
  "adcq  %%rax, %%r12 ;"
  "adcq  %%rax, %%r13 ;"
  "adcq  %%rax, %%r14 ;"

  "movq  %%r8,  0(%0) ;"
  "movq  %%r9,  8(%0) ;"
  "movq %%r10, 16(%0) ;"
  "movq %%r11, 24(%0) ;"
  "movq %%r12, 32(%0) ;"
  "movq %%r13, 40(%0) ;"
  "movq %%r14, 48(%0) ;"
  :
  : "r" (c), "r" (a), "r"  (b)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, add)(argElement_1w c, argElement_1w a, argElement_1w b) {
#ifdef __ADX__
  FN(add_adx)(c,a,b);
#else
  FN(add_adc)(c, a, b);
#endif
}

DECL(void, neg)(argElement_1w c) {
  EltFp448_1w_fullradix zero = {0};
  FN(sub)(c, zero, c);
}

DECL(void, mul)(argElement_1w c, argElement_1w a, argElement_1w b) {
  EltFp448_1w_fullradix_buffer buffer_1w;
  FN(intmul)(buffer_1w, a, b);
  FN(reduce)(c, buffer_1w);
}

DECL(void, sqr)(argElement_1w a) {
  EltFp448_1w_fullradix_buffer buffer_1w;
  FN(intsqr)(buffer_1w, a);
  FN(reduce)(a, buffer_1w);
}

DECL(void, sqrn)(argElement_1w a, unsigned int times) {
  unsigned int i = 0;
  EltFp448_1w_fullradix_buffer buffer_1w;
  for (i = 0; i < times; i++) {
    FN(intsqr)(buffer_1w, a);
    FN(reduce)(a, buffer_1w);
  }
}

DECL(void, invsqrt)(argElement_1w c, argElement_1w a, const int only_inverse) {
  EltFp448_1w_fullradix x0, x1;
  argElement_1w T[4];

  T[0] = x0;
  T[1] = c;
  T[2] = x1;
  T[3] = a;

  FN(copy)(T[1], T[3]);
  FN(sqrn)(T[1], 1);
  FN(mul)(T[1], T[1], T[3]);

  FN(copy)(T[0], T[1]);
  FN(sqrn)(T[0], 1);
  FN(mul)(T[0], T[0], T[3]);

  FN(copy)(T[1], T[0]);
  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 6);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 12);
  FN(mul)(T[1], T[1], T[2]);

  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 27);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 54);
  FN(mul)(T[1], T[1], T[2]);

  FN(sqrn)(T[1], 3);
  FN(mul)(T[1], T[1], T[0]);

  FN(copy)(T[2], T[1]);
  FN(sqrn)(T[2], 111);
  FN(mul)(T[2], T[2], T[1]);

  FN(copy)(T[1], T[2]);
  FN(sqrn)(T[1], 1);
  FN(mul)(T[1], T[1], T[3]);

  FN(sqrn)(T[1], 223);
  FN(mul)(T[1], T[1], T[2]);

  if (only_inverse != 0) {
    FN(sqrn)(T[1], 2);
    FN(mul)(T[1], T[1], T[3]);
  }
}

DECL(void, inv)(argElement_1w c, argElement_1w a) { FN(invsqrt)(c, a, 1); }

DECL(void, srt)(argElement_1w c, argElement_1w a) { FN(invsqrt)(c, a, 0); }

DECL(void, mula24_mulx)(argElement_1w c, argElement_1w a) {
  const uint64_t a24 = 39082;
  __asm__ __volatile__(
      "movq %2, %%rdx;"
      "mulx  0(%1), %%rcx,  %%r8;"
      "mulx  8(%1), %%rax,  %%r9; addq %%rax,  %%r8;"
      "mulx 16(%1), %%rax, %%r10; adcq %%rax,  %%r9;"
      "mulx 24(%1), %%rax, %%r11; adcq %%rax, %%r10;"
      "mulx 32(%1), %%rax, %%r12; adcq %%rax, %%r11;"
      "mulx 40(%1), %%rax, %%r13; adcq %%rax, %%r12;"
      "mulx 48(%1), %%rax, %%rdx; adcq %%rax, %%r13;"
      "adcq     $0, %%rdx;"

      /**
       * ( c7) + (c6,...,c0)
       * (rdx)
       */
      "movq %%rdx,%%rax; shlq $32,%%rax;"
      "addq %%rdx,%%rcx; movq  $0,%%rdx;"
      "adcq    $0, %%r8;"
      "adcq    $0, %%r9;"
      "adcq %%rax,%%r10;"
      "adcq    $0,%%r11;"
      "adcq    $0,%%r12;"
      "adcq    $0,%%r13;"
      "adcq    $0,%%rdx;"

      "movq %%rdx,%%rax; shlq $32,%%rax;"
      "addq %%rdx,%%rcx; movq %%rcx, 0(%0);"
      "adcq    $0, %%r8; movq  %%r8, 8(%0);"
      "adcq    $0, %%r9; movq  %%r9,16(%0);"
      "adcq %%rax,%%r10; movq %%r10,24(%0);"
      "adcq    $0,%%r11; movq %%r11,32(%0);"
      "adcq    $0,%%r12; movq %%r12,40(%0);"
      "adcq    $0,%%r13; movq %%r13,48(%0);"
      :
      : "r"(c), "r"(a), "r"(a24)
      : "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12",
        "%r13", "%r15");
}

DECL(void, mula24_mulq)(argElement_1w c, argElement_1w a) {
  const uint64_t a24 = 39082;
  __asm__ __volatile__(
      "movq %2, %%r15;"
      "movq  0(%1),%%rax; mulq %%r15;"
      "movq %%rax, %%rcx;                movq %%rdx,  %%r8;"
      "movq  8(%1),%%rax; mulq %%r15;"
      "addq %%rax,  %%r8; adcq $0,%%rdx; movq %%rdx,  %%r9;"
      "movq 16(%1),%%rax; mulq %%r15;"
      "addq %%rax,  %%r9; adcq $0,%%rdx; movq %%rdx, %%r10;"
      "movq 24(%1),%%rax; mulq %%r15;"
      "addq %%rax, %%r10; adcq $0,%%rdx; movq %%rdx, %%r11;"
      "movq 32(%1),%%rax; mulq %%r15;"
      "addq %%rax, %%r11; adcq $0,%%rdx; movq %%rdx, %%r12;"
      "movq 40(%1),%%rax; mulq %%r15;"
      "addq %%rax, %%r12; adcq $0,%%rdx; movq %%rdx, %%r13;"
      "movq 48(%1),%%rax; mulq %%r15;"
      "addq %%rax, %%r13; adcq $0,%%rdx;"

      /**
       * ( c7) + (c6,...,c0)
       * (rdx)
       */
      "movq %%rdx,%%rax; shlq $32,%%rax;"
      "addq %%rdx,%%rcx; movq  $0,%%rdx;"
      "adcq    $0, %%r8;"
      "adcq    $0, %%r9;"
      "adcq %%rax,%%r10;"
      "adcq    $0,%%r11;"
      "adcq    $0,%%r12;"
      "adcq    $0,%%r13;"
      "adcq    $0,%%rdx;"

      "movq %%rdx,%%rax; shlq $32,%%rax;"
      "addq %%rdx,%%rcx; movq %%rcx, 0(%0);"
      "adcq    $0, %%r8; movq  %%r8, 8(%0);"
      "adcq    $0, %%r9; movq  %%r9,16(%0);"
      "adcq %%rax,%%r10; movq %%r10,24(%0);"
      "adcq    $0,%%r11; movq %%r11,32(%0);"
      "adcq    $0,%%r12; movq %%r12,40(%0);"
      "adcq    $0,%%r13; movq %%r13,48(%0);"
      :
      : "r"(c), "r"(a), "r"(a24)
      : "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12",
        "%r13", "%r15");
}

DECL(void, mula24)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
  FN(mula24_mulx)(c, a);
#else /* Without BMI2 */
  FN(mula24_mulq)(c, a);
#endif
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP448 */
