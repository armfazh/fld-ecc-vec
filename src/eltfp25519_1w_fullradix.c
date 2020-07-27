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

#if defined(FP255)

#define FN(NAME) SUFFIX(NAME,1,fullradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

DECL(void, intmul2_mulxadx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "xorl %%r14d, %%r14d ;"
  "movq   (%1), %%rdx; " /* A[0] */
  "mulx   (%2),  %%r8, %%r12; " /* A[0]*B[0] */  "xorl %%r10d, %%r10d ;"  "movq %%r8, (%0) ;"
  "mulx  8(%2), %%r10, %%rax; " /* A[0]*B[1] */  "adox %%r10, %%r12 ;"
  "mulx 16(%2),  %%r8, %%rbx; " /* A[0]*B[2] */  "adox  %%r8, %%rax ;"
  "mulx 24(%2), %%r10, %%rcx; " /* A[0]*B[3] */  "adox %%r10, %%rbx ;"
  /*******************************************/  "adox %%r14, %%rcx ;"

  "movq  8(%1), %%rdx; " /* A[1] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[1]*B[0] */  "adox %%r12,  %%r8 ;"  "movq  %%r8, 8(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[1]*B[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rax ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[1]*B[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%rbx ;"
  "mulx 24(%2), %%r10, %%r12; " /* A[1]*B[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%rcx ;"
  /*******************************************/  "adox %%r14, %%r12 ;"  "adcx %%r14, %%r12 ;"

  "movq 16(%1), %%rdx; " /* A[2] */              "xorl %%r10d, %%r10d ;"
  "mulx   (%2),  %%r8,  %%r9; " /* A[2]*B[0] */  "adox %%rax,  %%r8 ;"  "movq %%r8, 16(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[2]*B[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rbx ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[2]*B[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%rcx ;"
  "mulx 24(%2), %%r10, %%rax; " /* A[2]*B[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%r12 ;"
  /*******************************************/  "adox %%r14, %%rax ;"  "adcx %%r14, %%rax ;"

  "movq 24(%1), %%rdx; " /* A[3] */              "xorl %%r10d, %%r10d ;"
  "mulx   (%2),  %%r8,  %%r9; " /* A[3]*B[0] */  "adox %%rbx,  %%r8 ;"  "movq %%r8, 24(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[3]*B[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rcx ;"  "movq %%rcx, 32(%0) ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[3]*B[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%r12 ;"  "movq %%r12, 40(%0) ;"
  "mulx 24(%2), %%r10, %%rbx; " /* A[3]*B[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%rax ;"  "movq %%rax, 48(%0) ;"
  /*******************************************/  "adox %%r14, %%rbx ;"  "adcx %%r14, %%rbx ;"  "movq %%rbx, 56(%0) ;"

  "movq 32(%1), %%rdx; " /* C[0] */
  "mulx 32(%2),  %%r8, %%r12; " /* C[0]*D[0] */  "xorl %%r10d, %%r10d ;" "movq %%r8, 64(%0);"
  "mulx 40(%2), %%r10, %%rax; " /* C[0]*D[1] */  "adox %%r10, %%r12 ;"
  "mulx 48(%2),  %%r8, %%rbx; " /* C[0]*D[2] */  "adox  %%r8, %%rax ;"
  "mulx 56(%2), %%r10, %%rcx; " /* C[0]*D[3] */  "adox %%r10, %%rbx ;"
  /*******************************************/  "adox %%r14, %%rcx ;"

  "movq 40(%1), %%rdx; " /* C[1] */              "xorl %%r10d, %%r10d ;"
  "mulx 32(%2),  %%r8,  %%r9; " /* C[1]*D[0] */  "adox %%r12,  %%r8 ;"  "movq  %%r8, 72(%0);"
  "mulx 40(%2), %%r10, %%r11; " /* C[1]*D[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rax ;"
  "mulx 48(%2),  %%r8, %%r13; " /* C[1]*D[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%rbx ;"
  "mulx 56(%2), %%r10, %%r12; " /* C[1]*D[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%rcx ;"
  /*******************************************/  "adox %%r14, %%r12 ;"  "adcx %%r14, %%r12 ;"

  "movq 48(%1), %%rdx; " /* C[2] */              "xorl %%r10d, %%r10d ;"
  "mulx 32(%2),  %%r8,  %%r9; " /* C[2]*D[0] */  "adox %%rax,  %%r8 ;"  "movq  %%r8, 80(%0);"
  "mulx 40(%2), %%r10, %%r11; " /* C[2]*D[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rbx ;"
  "mulx 48(%2),  %%r8, %%r13; " /* C[2]*D[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%rcx ;"
  "mulx 56(%2), %%r10, %%rax; " /* C[2]*D[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%r12 ;"
  /*******************************************/  "adox %%r14, %%rax ;"  "adcx %%r14, %%rax ;"

  "movq 56(%1), %%rdx; " /* C[3] */              "xorl %%r10d, %%r10d ;"
  "mulx 32(%2),  %%r8,  %%r9; " /* C[3]*D[0] */  "adox %%rbx,  %%r8 ;"  "movq  %%r8, 88(%0);"
  "mulx 40(%2), %%r10, %%r11; " /* C[3]*D[1] */  "adox %%r10,  %%r9 ;"  "adcx  %%r9, %%rcx ;"  "movq %%rcx,  96(%0) ;"
  "mulx 48(%2),  %%r8, %%r13; " /* C[3]*D[2] */  "adox  %%r8, %%r11 ;"  "adcx %%r11, %%r12 ;"  "movq %%r12, 104(%0) ;"
  "mulx 56(%2), %%r10, %%rbx; " /* C[3]*D[3] */  "adox %%r10, %%r13 ;"  "adcx %%r13, %%rax ;"  "movq %%rax, 112(%0) ;"
  /*******************************************/  "adox %%r14, %%rbx ;"  "adcx %%r14, %%rbx ;"  "movq %%rbx, 120(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx",
    "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intmul2_mulx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
    "movq   (%1), %%rdx; " /* A[0] */
    "mulx   (%2),  %%r8, %%r12; " /* A[0]*B[0] */                           "movq %%r8,  (%0) ;"
    "mulx  8(%2), %%r10, %%rax; " /* A[0]*B[1] */    "addq %%r10, %%r12 ;"
    "mulx 16(%2),  %%r8, %%rbx; " /* A[0]*B[2] */    "adcq  %%r8, %%rax ;"
    "mulx 24(%2), %%r10, %%rcx; " /* A[0]*B[3] */    "adcq %%r10, %%rbx ;"
    /*******************************************/    "adcq    $0, %%rcx ;"

    "movq  8(%1), %%rdx; " /* A[1] */
    "mulx   (%2),  %%r8,  %%r9; " /* A[1]*B[0] */    "addq %%r12,  %%r8 ;"  "movq %%r8, 8(%0) ;"
    "mulx  8(%2), %%r10, %%r11; " /* A[1]*B[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 16(%2),  %%r8, %%r13; " /* A[1]*B[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 24(%2), %%r10, %%r12; " /* A[1]*B[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%r12 ;"

    "addq  %%r9, %%rax ;"
    "adcq %%r11, %%rbx ;"
    "adcq %%r13, %%rcx ;"
    "adcq    $0, %%r12 ;"

    "movq 16(%1), %%rdx; " /* A[2] */
    "mulx   (%2),  %%r8,  %%r9; " /* A[2]*B[0] */    "addq %%rax,  %%r8 ;"  "movq %%r8, 16(%0) ;"
    "mulx  8(%2), %%r10, %%r11; " /* A[2]*B[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 16(%2),  %%r8, %%r13; " /* A[2]*B[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 24(%2), %%r10, %%rax; " /* A[2]*B[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%rax ;"

    "addq  %%r9, %%rbx ;"
    "adcq %%r11, %%rcx ;"
    "adcq %%r13, %%r12 ;"
    "adcq    $0, %%rax ;"

    "movq 24(%1), %%rdx; " /* A[3] */
    "mulx   (%2),  %%r8,  %%r9; " /* A[3]*B[0] */    "addq %%rbx,  %%r8 ;"  "movq %%r8, 24(%0) ;"
    "mulx  8(%2), %%r10, %%r11; " /* A[3]*B[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 16(%2),  %%r8, %%r13; " /* A[3]*B[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 24(%2), %%r10, %%rbx; " /* A[3]*B[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%rbx ;"

    "addq  %%r9, %%rcx ;"  "movq %%rcx, 32(%0) ;"
    "adcq %%r11, %%r12 ;"  "movq %%r12, 40(%0) ;"
    "adcq %%r13, %%rax ;"  "movq %%rax, 48(%0) ;"
    "adcq    $0, %%rbx ;"  "movq %%rbx, 56(%0) ;"

    "movq 32(%1), %%rdx; " /* C[0] */
    "mulx 32(%2),  %%r8, %%r12; " /* C[0]*D[0] */                           "movq %%r8, 64(%0) ;"
    "mulx 40(%2), %%r10, %%rax; " /* C[0]*D[1] */    "addq %%r10, %%r12 ;"
    "mulx 48(%2),  %%r8, %%rbx; " /* C[0]*D[2] */    "adcq  %%r8, %%rax ;"
    "mulx 56(%2), %%r10, %%rcx; " /* C[0]*D[3] */    "adcq %%r10, %%rbx ;"
    /*******************************************/    "adcq    $0, %%rcx ;"

    "movq 40(%1), %%rdx; " /* C[1] */
    "mulx 32(%2),  %%r8,  %%r9; " /* C[1]*D[0] */    "addq %%r12,  %%r8 ;"  "movq %%r8, 72(%0) ;"
    "mulx 40(%2), %%r10, %%r11; " /* C[1]*D[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 48(%2),  %%r8, %%r13; " /* C[1]*D[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 56(%2), %%r10, %%r12; " /* C[1]*D[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%r12 ;"

    "addq  %%r9, %%rax ;"
    "adcq %%r11, %%rbx ;"
    "adcq %%r13, %%rcx ;"
    "adcq    $0, %%r12 ;"

    "movq 48(%1), %%rdx; " /* C[2] */
    "mulx 32(%2),  %%r8,  %%r9; " /* C[2]*D[0] */    "addq %%rax,  %%r8 ;"  "movq %%r8, 80(%0) ;"
    "mulx 40(%2), %%r10, %%r11; " /* C[2]*D[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 48(%2),  %%r8, %%r13; " /* C[2]*D[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 56(%2), %%r10, %%rax; " /* C[2]*D[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%rax ;"

    "addq  %%r9, %%rbx ;"
    "adcq %%r11, %%rcx ;"
    "adcq %%r13, %%r12 ;"
    "adcq    $0, %%rax ;"

    "movq 56(%1), %%rdx; " /* C[3] */
    "mulx 32(%2),  %%r8,  %%r9; " /* C[3]*D[0] */    "addq %%rbx,  %%r8 ;"  "movq %%r8, 88(%0) ;"
    "mulx 40(%2), %%r10, %%r11; " /* C[3]*D[1] */    "adcq %%r10,  %%r9 ;"
    "mulx 48(%2),  %%r8, %%r13; " /* C[3]*D[2] */    "adcq  %%r8, %%r11 ;"
    "mulx 56(%2), %%r10, %%rbx; " /* C[3]*D[3] */    "adcq %%r10, %%r13 ;"
    /*******************************************/    "adcq    $0, %%rbx ;"

    "addq  %%r9, %%rcx ;"  "movq %%rcx,  96(%0) ;"
    "adcq %%r11, %%r12 ;"  "movq %%r12, 104(%0) ;"
    "adcq %%r13, %%rax ;"  "movq %%rax, 112(%0) ;"
    "adcq    $0, %%rbx ;"  "movq %%rbx, 120(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx",
    "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
  );
}

DECL(void, intmul2_mulq)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
    "movq   (%1),%%r15 ;"
    "movq   (%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax,  (%0) ;"  "movq %%rdx,  %%r9 ;"
    "movq  8(%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax, %%r10 ;"  "movq %%rdx, %%r11 ;"
    "movq 16(%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax, %%r12 ;"  "movq %%rdx, %%r13 ;"
    "movq 24(%2),%%rax ;"  "mulq %%r15 ;"

    "addq %%r10,  %%r9 ;"
    "adcq %%r12, %%r11 ;"
    "adcq %%rax, %%r13 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r15 ;"

    "movq 8(%1), %%r14 ;"
    "movq  (%2), %%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r8 ;"
    "movq 8(%2), %%rax ;"  "mulq %%r14 ;"                         "movq %%rdx, %%r10 ;"
    "addq  %%r9, %%rcx ;"  "movq %%rcx, 8(%0) ;"
    "adcq %%rax,  %%r8 ;"
    "adcq    $0, %%r10 ;"

    "movq 16(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r12 ;"
    "movq 24(%2),%%rax ;"  "mulq %%r14 ;"
    "addq %%rcx, %%r10 ;"
    "adcq %%rax, %%r12 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r11,  %%r8 ;"
    "adcq %%r13, %%r10 ;"
    "adcq %%r15, %%r12 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r14 ;"

    "movq 16(%1),%%r15 ;"
    "movq  (%2), %%rax ;"  "mulq %%r15 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r9 ;"
    "movq 8(%2), %%rax ;"  "mulq %%r15 ;"                         "movq %%rdx, %%r11 ;"
    "addq  %%r8, %%rcx ;"  "movq %%rcx, 16(%0) ;"
    "adcq %%rax,  %%r9 ;"
    "adcq    $0, %%r11 ;"

    "movq 16(%2),%%rax ;"  "mulq %%r15 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r13 ;"
    "movq 24(%2),%%rax ;"  "mulq %%r15 ;"
    "addq %%rcx, %%r11 ;"
    "adcq %%rax, %%r13 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r10,  %%r9 ;"
    "adcq %%r12, %%r11 ;"
    "adcq %%r14, %%r13 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r15 ;"

    "movq 24(%1),%%r14 ;"
    "movq  (%2), %%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r8 ;"
    "movq 8(%2), %%rax ;"  "mulq %%r14 ;"                         "movq %%rdx, %%r10 ;"
    "addq  %%r9, %%rcx ;"  "movq %%rcx, 24(%0) ;"
    "adcq %%rax,  %%r8 ;"
    "adcq    $0, %%r10 ;"

    "movq 16(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r12 ;"
    "movq 24(%2),%%rax ;"  "mulq %%r14 ;"
    "addq %%rcx, %%r10 ;"
    "adcq %%rax, %%r12 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r11,  %%r8 ;"  "movq  %%r8, 32(%0) ;"
    "adcq %%r13, %%r10 ;"  "movq %%r10, 40(%0) ;"
    "adcq %%r15, %%r12 ;"  "movq %%r12, 48(%0) ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, 56(%0) ;"


    "movq 32(%1),%%r15 ;"
    "movq 32(%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax, 64(%0) ;"  "movq %%rdx,  %%r9 ;"
    "movq 40(%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax, %%r10 ;"  "movq %%rdx, %%r11 ;"
    "movq 48(%2),%%rax ;"  "mulq %%r15 ;"   "movq %%rax, %%r12 ;"  "movq %%rdx, %%r13 ;"
    "movq 56(%2),%%rax ;"  "mulq %%r15 ;"

    "addq %%r10,  %%r9 ;"
    "adcq %%r12, %%r11 ;"
    "adcq %%rax, %%r13 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r15 ;"

    "movq 40(%1),%%r14 ;"
    "movq 32(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r8 ;"
    "movq 40(%2),%%rax ;"  "mulq %%r14 ;"                         "movq %%rdx, %%r10 ;"
    "addq  %%r9, %%rcx ;"  "movq %%rcx, 72(%0) ;"
    "adcq %%rax,  %%r8 ;"
    "adcq    $0, %%r10 ;"

    "movq 48(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r12 ;"
    "movq 56(%2),%%rax ;"  "mulq %%r14 ;"
    "addq %%rcx, %%r10 ;"
    "adcq %%rax, %%r12 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r11,  %%r8 ;"
    "adcq %%r13, %%r10 ;"
    "adcq %%r15, %%r12 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r14 ;"

    "movq 48(%1),%%r15 ;"
    "movq 32(%2),%%rax ;"  "mulq %%r15 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r9 ;"
    "movq 40(%2),%%rax ;"  "mulq %%r15 ;"                         "movq %%rdx, %%r11 ;"
    "addq  %%r8, %%rcx ;"  "movq %%rcx, 80(%0) ;"
    "adcq %%rax,  %%r9 ;"
    "adcq    $0, %%r11 ;"

    "movq 48(%2),%%rax ;"  "mulq %%r15 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r13 ;"
    "movq 56(%2),%%rax ;"  "mulq %%r15 ;"
    "addq %%rcx, %%r11 ;"
    "adcq %%rax, %%r13 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r10,  %%r9 ;"
    "adcq %%r12, %%r11 ;"
    "adcq %%r14, %%r13 ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, %%r15 ;"

    "movq 56(%1),%%r14 ;"
    "movq 32(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx,  %%r8 ;"
    "movq 40(%2),%%rax ;"  "mulq %%r14 ;"                         "movq %%rdx, %%r10 ;"
    "addq  %%r9, %%rcx ;"  "movq %%rcx, 88(%0) ;"
    "adcq %%rax,  %%r8 ;"
    "adcq    $0, %%r10 ;"

    "movq 48(%2),%%rax ;"  "mulq %%r14 ;"  "movq %%rax, %%rcx ;"  "movq %%rdx, %%r12 ;"
    "movq 56(%2),%%rax ;"  "mulq %%r14 ;"
    "addq %%rcx, %%r10 ;"
    "adcq %%rax, %%r12 ;"
    "adcq    $0, %%rdx ;"

    "addq %%r11,  %%r8 ;"  "movq  %%r8,  96(%0) ;"
    "adcq %%r13, %%r10 ;"  "movq %%r10, 104(%0) ;"
    "adcq %%r15, %%r12 ;"  "movq %%r12, 112(%0) ;"
    "adcq    $0, %%rdx ;"  "movq %%rdx, 120(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9",
    "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
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
  __asm__ __volatile__(
    "movq   (%1), %%rdx        ;" /* A[0]      */
    "mulx  8(%1),  %%r8, %%r14 ;" /* A[1]*A[0] */  "xorl %%r15d, %%r15d;"
    "mulx 16(%1),  %%r9, %%r10 ;" /* A[2]*A[0] */  "adcx %%r14,  %%r9 ;"
    "mulx 24(%1), %%rax, %%rcx ;" /* A[3]*A[0] */  "adcx %%rax, %%r10 ;"
    "movq 24(%1), %%rdx        ;" /* A[3]      */
    "mulx  8(%1), %%r11, %%r12 ;" /* A[1]*A[3] */  "adcx %%rcx, %%r11 ;"
    "mulx 16(%1), %%rax, %%r13 ;" /* A[2]*A[3] */  "adcx %%rax, %%r12 ;"
    "movq  8(%1), %%rdx        ;" /* A[1]      */  "adcx %%r15, %%r13 ;"
    "mulx 16(%1), %%rax, %%rcx ;" /* A[2]*A[1] */  "movq    $0, %%r14 ;"
    /*******************************************/  "adcx %%r15, %%r14 ;"

    "xorl %%r15d, %%r15d;"
    "adox %%rax, %%r10 ;"  "adcx  %%r8,  %%r8 ;"
    "adox %%rcx, %%r11 ;"  "adcx  %%r9,  %%r9 ;"
    "adox %%r15, %%r12 ;"  "adcx %%r10, %%r10 ;"
    "adox %%r15, %%r13 ;"  "adcx %%r11, %%r11 ;"
    "adox %%r15, %%r14 ;"  "adcx %%r12, %%r12 ;"
    "adcx %%r13, %%r13 ;"
    "adcx %%r14, %%r14 ;"

    "movq   (%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[0]^2 */
    /********************/  "movq %%rax,  0(%0) ;"
    "addq %%rcx,  %%r8 ;"   "movq  %%r8,  8(%0) ;"
    "movq  8(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[1]^2 */
    "adcq %%rax,  %%r9 ;"   "movq  %%r9, 16(%0) ;"
    "adcq %%rcx, %%r10 ;"   "movq %%r10, 24(%0) ;"
    "movq 16(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[2]^2 */
    "adcq %%rax, %%r11 ;"   "movq %%r11, 32(%0) ;"
    "adcq %%rcx, %%r12 ;"   "movq %%r12, 40(%0) ;"
    "movq 24(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[3]^2 */
    "adcq %%rax, %%r13 ;"   "movq %%r13, 48(%0) ;"
    "adcq %%rcx, %%r14 ;"   "movq %%r14, 56(%0) ;"


    "movq 32(%1), %%rdx        ;" /* B[0]      */
    "mulx 40(%1),  %%r8, %%r14 ;" /* B[1]*B[0] */  "xorl %%r15d, %%r15d;"
    "mulx 48(%1),  %%r9, %%r10 ;" /* B[2]*B[0] */  "adcx %%r14,  %%r9 ;"
    "mulx 56(%1), %%rax, %%rcx ;" /* B[3]*B[0] */  "adcx %%rax, %%r10 ;"
    "movq 56(%1), %%rdx        ;" /* B[3]      */
    "mulx 40(%1), %%r11, %%r12 ;" /* B[1]*B[3] */  "adcx %%rcx, %%r11 ;"
    "mulx 48(%1), %%rax, %%r13 ;" /* B[2]*B[3] */  "adcx %%rax, %%r12 ;"
    "movq 40(%1), %%rdx        ;" /* B[1]      */  "adcx %%r15, %%r13 ;"
    "mulx 48(%1), %%rax, %%rcx ;" /* B[2]*B[1] */  "movq    $0, %%r14 ;"
    /*******************************************/  "adcx %%r15, %%r14 ;"

    "xorl %%r15d, %%r15d;"
    "adox %%rax, %%r10 ;"  "adcx  %%r8,  %%r8 ;"
    "adox %%rcx, %%r11 ;"  "adcx  %%r9,  %%r9 ;"
    "adox %%r15, %%r12 ;"  "adcx %%r10, %%r10 ;"
    "adox %%r15, %%r13 ;"  "adcx %%r11, %%r11 ;"
    "adox %%r15, %%r14 ;"  "adcx %%r12, %%r12 ;"
    "adcx %%r13, %%r13 ;"
    "adcx %%r14, %%r14 ;"

    "movq 32(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* B[0]^2 */
    /********************/  "movq %%rax,  64(%0) ;"
    "addq %%rcx,  %%r8 ;"   "movq  %%r8,  72(%0) ;"
    "movq 40(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* B[1]^2 */
    "adcq %%rax,  %%r9 ;"   "movq  %%r9,  80(%0) ;"
    "adcq %%rcx, %%r10 ;"   "movq %%r10,  88(%0) ;"
    "movq 48(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* B[2]^2 */
    "adcq %%rax, %%r11 ;"   "movq %%r11,  96(%0) ;"
    "adcq %%rcx, %%r12 ;"   "movq %%r12, 104(%0) ;"
    "movq 56(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* B[3]^2 */
    "adcq %%rax, %%r13 ;"   "movq %%r13, 112(%0) ;"
    "adcq %%rcx, %%r14 ;"   "movq %%r14, 120(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
}

DECL(void, intsqr2_mulx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
    "movq  8(%1), %%rdx        ;" /* A[1]      */
    "mulx   (%1),  %%r8,  %%r9 ;" /* A[0]*A[1] */
    "mulx 16(%1), %%r10, %%r11 ;" /* A[2]*A[1] */
    "mulx 24(%1), %%rcx, %%r14 ;" /* A[3]*A[1] */

    "movq 16(%1), %%rdx        ;" /* A[2]      */
    "mulx 24(%1), %%r12, %%r13 ;" /* A[3]*A[2] */
    "mulx   (%1), %%rax, %%rdx ;" /* A[0]*A[2] */

    "addq %%rax,  %%r9 ;"
    "adcq %%rdx, %%r10 ;"
    "adcq %%rcx, %%r11 ;"
    "adcq %%r14, %%r12 ;"
    "adcq    $0, %%r13 ;"
    "movq    $0, %%r14 ;"
    "adcq    $0, %%r14 ;"

    "movq   (%1), %%rdx        ;" /* A[0]      */
    "mulx 24(%1), %%rax, %%rcx ;" /* A[0]*A[3] */

    "addq %%rax, %%r10 ;"
    "adcq %%rcx, %%r11 ;"
    "adcq    $0, %%r12 ;"
    "adcq    $0, %%r13 ;"
    "adcq    $0, %%r14 ;"

    "shldq $1, %%r13, %%r14 ;"
    "shldq $1, %%r12, %%r13 ;"
    "shldq $1, %%r11, %%r12 ;"
    "shldq $1, %%r10, %%r11 ;"
    "shldq $1,  %%r9, %%r10 ;"
    "shldq $1,  %%r8,  %%r9 ;"
    "shlq  $1,  %%r8        ;"

    /********************/ "mulx %%rdx, %%rax, %%rcx ; " /* A[0]^2 */
    /********************/ "movq %%rax,  0(%0) ;"
    "addq %%rcx,  %%r8 ;"  "movq  %%r8,  8(%0) ;"
    "movq  8(%1), %%rdx ;" "mulx %%rdx, %%rax, %%rcx ; " /* A[1]^2 */
    "adcq %%rax,  %%r9 ;"  "movq  %%r9, 16(%0) ;"
    "adcq %%rcx, %%r10 ;"  "movq %%r10, 24(%0) ;"
    "movq 16(%1), %%rdx ;" "mulx %%rdx, %%rax, %%rcx ; " /* A[2]^2 */
    "adcq %%rax, %%r11 ;"  "movq %%r11, 32(%0) ;"
    "adcq %%rcx, %%r12 ;"  "movq %%r12, 40(%0) ;"
    "movq 24(%1), %%rdx ;" "mulx %%rdx, %%rax, %%rcx ; " /* A[3]^2 */
    "adcq %%rax, %%r13 ;"  "movq %%r13, 48(%0) ;"
    "adcq %%rcx, %%r14 ;"  "movq %%r14, 56(%0) ;"

    "movq 40(%1), %%rdx        ;" /* B[1]      */
    "mulx 32(%1),  %%r8,  %%r9 ;" /* B[0]*B[1] */
    "mulx 48(%1), %%r10, %%r11 ;" /* B[2]*B[1] */
    "mulx 56(%1), %%rcx, %%r14 ;" /* B[3]*B[1] */

    "movq 48(%1), %%rdx        ;" /* B[2]      */
    "mulx 56(%1), %%r12, %%r13 ;" /* B[3]*B[2] */
    "mulx 32(%1), %%rax, %%rdx ;" /* B[0]*B[2] */

    "addq %%rax,  %%r9 ;"
    "adcq %%rdx, %%r10 ;"
    "adcq %%rcx, %%r11 ;"
    "adcq %%r14, %%r12 ;"
    "adcq    $0, %%r13 ;"
    "movq    $0, %%r14 ;"
    "adcq    $0, %%r14 ;"

    "movq 32(%1), %%rdx        ;" /* B[0]      */
    "mulx 56(%1), %%rax, %%rcx ;" /* B[0]*B[3] */

    "addq %%rax, %%r10 ;"
    "adcq %%rcx, %%r11 ;"
    "adcq    $0, %%r12 ;"
    "adcq    $0, %%r13 ;"
    "adcq    $0, %%r14 ;"

    "shldq $1, %%r13, %%r14 ;"
    "shldq $1, %%r12, %%r13 ;"
    "shldq $1, %%r11, %%r12 ;"
    "shldq $1, %%r10, %%r11 ;"
    "shldq $1,  %%r9, %%r10 ;"
    "shldq $1,  %%r8,  %%r9 ;"
    "shlq  $1,  %%r8        ;"

    /********************/  "mulx %%rdx, %%rax, %%rcx ; " /* B[0]^2 */
    /********************/  "movq %%rax,  64(%0) ;"
    "addq %%rcx,  %%r8 ;"   "movq  %%r8,  72(%0) ;"
    "movq 40(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ; " /* B[1]^2 */
    "adcq %%rax,  %%r9 ;"   "movq  %%r9,  80(%0) ;"
    "adcq %%rcx, %%r10 ;"   "movq %%r10,  88(%0) ;"
    "movq 48(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ; " /* B[2]^2 */
    "adcq %%rax, %%r11 ;"   "movq %%r11,  96(%0) ;"
    "adcq %%rcx, %%r12 ;"   "movq %%r12, 104(%0) ;"
    "movq 56(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ; " /* B[3]^2 */
    "adcq %%rax, %%r13 ;"   "movq %%r13, 112(%0) ;"
    "adcq %%rcx, %%r14 ;"   "movq %%r14, 120(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
    "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intsqr2_mulq)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__ (
  "movq  0(%1),  %%r8;"
  "movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax,  %%r9;" "movq %%rdx, %%r10;" /* a[0]*a[1] */
  "movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;" /* a[0]*a[2] */
  "movq 24(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r15;" "movq %%rdx, %%r12;" /* a[0]*a[3] */
  "movq 24(%1),  %%r8;"
  "movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%rcx;" "movq %%rdx, %%r13;" /* a[3]*a[1] */
  "movq 16(%1), %%rax;" "mulq %%r8;" /* a[3]*a[2] */

  "addq %%r14, %%r10;"
  "adcq %%r15, %%r11;" "movq $0, %%r15;"
  "adcq %%rcx, %%r12;"
  "adcq %%rax, %%r13;"
  "adcq    $0, %%rdx;" "movq %%rdx, %%r14;"
  "movq 8(%1), %%rax;" "mulq 16(%1);"

  "addq %%rax, %%r11;"
  "adcq %%rdx, %%r12;"
  "adcq $0, %%r13;"
  "adcq $0, %%r14;"
  "adcq $0, %%r15;"

  "shldq $1, %%r14, %%r15;" "movq %%r15, 56(%0);"
  "shldq $1, %%r13, %%r14;" "movq %%r14, 48(%0);"
  "shldq $1, %%r12, %%r13;" "movq %%r13, 40(%0);"
  "shldq $1, %%r11, %%r12;" "movq %%r12, 32(%0);"
  "shldq $1, %%r10, %%r11;" "movq %%r11, 24(%0);"
  "shldq $1,  %%r9, %%r10;" "movq %%r10, 16(%0);"
  "shlq  $1,  %%r9;"        "movq  %%r9,  8(%0);"

  "movq  0(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, 0(%0);"  "movq %%rdx,  %%r9;"
  "movq  8(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r10;"  "movq %%rdx, %%r11;"
  "movq 16(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r12;"  "movq %%rdx, %%r13;"
  "movq 24(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r14;"  "movq %%rdx, %%r15;"

  "addq  8(%0),  %%r9;"  "movq  %%r9,  8(%0);"
  "adcq 16(%0), %%r10;"  "movq %%r10, 16(%0);"
  "adcq 24(%0), %%r11;"  "movq %%r11, 24(%0);"
  "adcq 32(%0), %%r12;"  "movq %%r12, 32(%0);"
  "adcq 40(%0), %%r13;"  "movq %%r13, 40(%0);"
  "adcq 48(%0), %%r14;"  "movq %%r14, 48(%0);"
  "adcq 56(%0), %%r15;"  "movq %%r15, 56(%0);"

  "movq 32(%1),  %%r8;"
  "movq 40(%1), %%rax;" "mulq %%r8;" "movq %%rax,  %%r9;" "movq %%rdx, %%r10;" /* a[0]*a[1] */
  "movq 48(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;" /* a[0]*a[2] */
  "movq 56(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r15;" "movq %%rdx, %%r12;" /* a[0]*a[3] */
  "movq 56(%1),  %%r8;"
  "movq 40(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%rcx;" "movq %%rdx, %%r13;" /* a[3]*a[1] */
  "movq 48(%1), %%rax;" "mulq %%r8;" /* a[3]*a[2] */

  "addq %%r14, %%r10;"
  "adcq %%r15, %%r11;" "movq $0, %%r15;"
  "adcq %%rcx, %%r12;"
  "adcq %%rax, %%r13;"
  "adcq    $0, %%rdx;" "movq %%rdx, %%r14;"
  "movq 40(%1),%%rax;" "mulq 48(%1);"

  "addq %%rax, %%r11;"
  "adcq %%rdx, %%r12;"
  "adcq $0, %%r13;"
  "adcq $0, %%r14;"
  "adcq $0, %%r15;"

  "shldq $1, %%r14, %%r15;" "movq %%r15,120(%0);"
  "shldq $1, %%r13, %%r14;" "movq %%r14,112(%0);"
  "shldq $1, %%r12, %%r13;" "movq %%r13,104(%0);"
  "shldq $1, %%r11, %%r12;" "movq %%r12, 96(%0);"
  "shldq $1, %%r10, %%r11;" "movq %%r11, 88(%0);"
  "shldq $1,  %%r9, %%r10;" "movq %%r10, 80(%0);"
  "shlq  $1,  %%r9;"        "movq  %%r9, 72(%0);"

  "movq 32(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, 64(%0);" "movq %%rdx,  %%r9;"
  "movq 40(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r10;"  "movq %%rdx, %%r11;"
  "movq 48(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r12;"  "movq %%rdx, %%r13;"
  "movq 56(%1),%%rax;"  "mulq %%rax;"  "movq %%rax, %%r14;"  "movq %%rdx, %%r15;"

  "addq  72(%0),  %%r9;"  "movq  %%r9,  72(%0);"
  "adcq  80(%0), %%r10;"  "movq %%r10,  80(%0);"
  "adcq  88(%0), %%r11;"  "movq %%r11,  88(%0);"
  "adcq  96(%0), %%r12;"  "movq %%r12,  96(%0);"
  "adcq 104(%0), %%r13;"  "movq %%r13, 104(%0);"
  "adcq 112(%0), %%r14;"  "movq %%r14, 112(%0);"
  "adcq 120(%0), %%r15;"  "movq %%r15, 120(%0);"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
    "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
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

DECL(void, reduce2_mulxadx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movl    $38, %%edx; " /* 2*c = 38 = 2^256 */
  "mulx 32(%1),  %%r8, %%r10; " /* c*C[4] */   "xorl %%ebx, %%ebx ;"  "adox   (%1),  %%r8 ;"
  "mulx 40(%1),  %%r9, %%r11; " /* c*C[5] */   "adcx %%r10,  %%r9 ;"  "adox  8(%1),  %%r9 ;"
  "mulx 48(%1), %%r10, %%rax; " /* c*C[6] */   "adcx %%r11, %%r10 ;"  "adox 16(%1), %%r10 ;"
  "mulx 56(%1), %%r11, %%rcx; " /* c*C[7] */   "adcx %%rax, %%r11 ;"  "adox 24(%1), %%r11 ;"
  /****************************************/   "adcx %%rbx, %%rcx ;"  "adox  %%rbx, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0, of=0 */
  "adcx %%rcx,  %%r8 ;"
  "adcx %%rbx,  %%r9 ;"  "movq  %%r9,  8(%0) ;"
  "adcx %%rbx, %%r10 ;"  "movq %%r10, 16(%0) ;"
  "adcx %%rbx, %%r11 ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8,   (%0) ;"

  "mulx  96(%1),  %%r8, %%r10; " /* c*C[4] */  "xorl %%ebx, %%ebx ;"  "adox 64(%1),  %%r8 ;"
  "mulx 104(%1),  %%r9, %%r11; " /* c*C[5] */  "adcx %%r10,  %%r9 ;"  "adox 72(%1),  %%r9 ;"
  "mulx 112(%1), %%r10, %%rax; " /* c*C[6] */  "adcx %%r11, %%r10 ;"  "adox 80(%1), %%r10 ;"
  "mulx 120(%1), %%r11, %%rcx; " /* c*C[7] */  "adcx %%rax, %%r11 ;"  "adox 88(%1), %%r11 ;"
  /*****************************************/  "adcx %%rbx, %%rcx ;"  "adox  %%rbx, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0, of=0 */
  "adcx %%rcx,  %%r8 ;"
  "adcx %%rbx,  %%r9 ;"  "movq  %%r9, 40(%0) ;"
  "adcx %%rbx, %%r10 ;"  "movq %%r10, 48(%0) ;"
  "adcx %%rbx, %%r11 ;"  "movq %%r11, 56(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8, 32(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, reduce2_mulx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movl    $38, %%edx ; " /* 2*c = 38 = 2^256 */
  "mulx 32(%1),  %%r8, %%r10 ;" /* c*C[4] */
  "mulx 40(%1),  %%r9, %%r11 ;" /* c*C[5] */  "addq %%r10,  %%r9 ;"
  "mulx 48(%1), %%r10, %%rax ;" /* c*C[6] */  "adcq %%r11, %%r10 ;"
  "mulx 56(%1), %%r11, %%rcx ;" /* c*C[7] */  "adcq %%rax, %%r11 ;"
  /****************************************/  "adcq    $0, %%rcx ;"
  "addq   (%1),  %%r8 ;"
  "adcq  8(%1),  %%r9 ;"
  "adcq 16(%1), %%r10 ;"
  "adcq 24(%1), %%r11 ;"
  "adcq     $0, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0 */
  "addq %%rcx,  %%r8 ;"
  "adcq    $0,  %%r9 ;"  "movq  %%r9,  8(%0) ;"
  "adcq    $0, %%r10 ;"  "movq %%r10, 16(%0) ;"
  "adcq    $0, %%r11 ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8,   (%0) ;"

  "mulx  96(%1),  %%r8, %%r10 ;" /* c*C[4] */
  "mulx 104(%1),  %%r9, %%r11 ;" /* c*C[5] */  "addq %%r10,  %%r9 ;"
  "mulx 112(%1), %%r10, %%rax ;" /* c*C[6] */  "adcq %%r11, %%r10 ;"
  "mulx 120(%1), %%r11, %%rcx ;" /* c*C[7] */  "adcq %%rax, %%r11 ;"
  /*****************************************/  "adcq    $0, %%rcx ;"
  "addq 64(%1),  %%r8 ;"
  "adcq 72(%1),  %%r9 ;"
  "adcq 80(%1), %%r10 ;"
  "adcq 88(%1), %%r11 ;"
  "adcq     $0, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0 */
  "addq %%rcx,  %%r8 ;"
  "adcq    $0,  %%r9 ;"  "movq  %%r9, 40(%0) ;"
  "adcq    $0, %%r10 ;"  "movq %%r10, 48(%0) ;"
  "adcq    $0, %%r11 ;"  "movq %%r11, 56(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8, 32(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, reduce2_mulq)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
    /* 2*C = 38 = 2^256 */ \
	"movl $38, %%eax;" "mulq 32(%1);" "movq %%rax,  %%r8;" "movq %%rdx,  %%r9;" /* c*c[4] */
	"movl $38, %%eax;" "mulq 40(%1);" "movq %%rax, %%r12;" "movq %%rdx, %%r10;" /* c*c[5] */
	"movl $38, %%eax;" "mulq 48(%1);" "movq %%rax, %%r13;" "movq %%rdx, %%r11;" /* c*c[6] */
	"movl $38, %%eax;" "mulq 56(%1);" /* c*c[7] */
	"addq %%r12,  %%r9;"
	"adcq %%r13, %%r10;"
	"adcq %%rax, %%r11;"
	"adcq    $0, %%rdx;"
	"addq  0(%1),  %%r8;"
	"adcq  8(%1),  %%r9;"
	"adcq 16(%1), %%r10;"
	"adcq 24(%1), %%r11;"
	"adcq     $0, %%rdx;"
	"movl $38, %%eax;"
    "imulq %%rax, %%rdx;" /* c*c[4], cf=0, of=0 */
    "addq %%rdx,  %%r8;"
    "adcq $0,  %%r9;" "movq  %%r9,  8(%0);"
    "adcq $0, %%r10;" "movq %%r10, 16(%0);"
    "adcq $0, %%r11;" "movq %%r11, 24(%0);"
    "movl $0, %%edx;"
	"cmovc %%rax, %%rdx;"
    "addq %%rdx, %%r8;" "movq %%r8, 0(%0);"

    "movl $38, %%eax;" "mulq  96(%1);" "movq %%rax,  %%r8;" "movq %%rdx,  %%r9;" /* c*c[4] */
    "movl $38, %%eax;" "mulq 104(%1);" "movq %%rax, %%r12;" "movq %%rdx, %%r10;" /* c*c[5] */
    "movl $38, %%eax;" "mulq 112(%1);" "movq %%rax, %%r13;" "movq %%rdx, %%r11;" /* c*c[6] */
    "movl $38, %%eax;" "mulq 120(%1);" /* c*c[7] */
    "addq %%r12,  %%r9;"
    "adcq %%r13, %%r10;"
    "adcq %%rax, %%r11;"
    "adcq    $0, %%rdx;"
    "addq 64(%1),  %%r8;"
    "adcq 72(%1),  %%r9;"
    "adcq 80(%1), %%r10;"
    "adcq 88(%1), %%r11;"
    "adcq     $0, %%rdx;"
    "movl $38, %%eax;"
    "imulq %%rax, %%rdx;" /* c*c[4], cf=0, of=0 */
    "addq %%rdx,  %%r8;"
    "adcq $0,  %%r9;" "movq  %%r9, 40(%0);"
    "adcq $0, %%r10;" "movq %%r10, 48(%0);"
    "adcq $0, %%r11;" "movq %%r11, 56(%0);"
    "movl $0, %%edx;"
    "cmovc %%rax, %%rdx;"
    "addq %%rdx, %%r8;" "movq %%r8, 32(%0);"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
  );
}

DECL(void, reduce2)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(reduce2_mulxadx)(c,a);
#else /* No __ADX__ */
  FN(reduce2_mulx)(c, a);
#endif
#else /* No __BMI2__ */
  FN(reduce2_mulq)(c,a);
#endif
}

DECL(argElement_1w, alloc)(void) {
  return (argElement_1w) allocate_bytes(NUM_DIGITS_FP25519_X64 * sizeof(uint64_t));
}

DECL(void, copy)(argElement_1w c, argElement_1w a) {
  c[0] = a[0];
  c[1] = a[1];
  c[2] = a[2];
  c[3] = a[3];
}

DECL(void, prime)(argElement_1w a) {
  a[0] = 0xffffffffffffffed;
  a[1] = 0xffffffffffffffff;
  a[2] = 0xffffffffffffffff;
  a[3] = 0x7fffffffffffffff;
}

DECL(void, rand)(argElement_1w a) {
  random_bytes((uint8_t *) a, SIZE_FP25519);
}

DECL(void, zero)(argElement_1w a) {
  a[0] = 0;
  a[1] = 0;
  a[2] = 0;
  a[3] = 0;
}

DECL(void, modp)(argElement_1w c) {
  uint64_t tmp0, tmp1;
  __asm__ __volatile__ (
  "movl   $19,   %k5 ;"
  "movl   $38,   %k4 ;"

  "btrq   $63,    %3 ;" /* Put bit 255 in carry flag and clear */
  "cmovncl %k5,   %k4 ;" /* c[255] ? 38 : 19 */

  /* Add either 19 or 38 to c */
  "addq    %4,   %0 ;"
  "adcq    $0,   %1 ;"
  "adcq    $0,   %2 ;"
  "adcq    $0,   %3 ;"

  /* Test for bit 255 again; only triggered on overflow modulo 2^255-19 */
  "movl    $0,  %k4 ;"
  "cmovnsl %k5,  %k4 ;" /* c[255] ? 0 : 19 */
  "btrq   $63,   %3 ;" /* Clear bit 255 */

  /* Subtract 19 if necessary */
  "subq    %4,   %0 ;"
  "sbbq    $0,   %1 ;"
  "sbbq    $0,   %2 ;"
  "sbbq    $0,   %3 ;"

  : "+r"(c[0]), "+r"(c[1]), "+r"(c[2]), "+r"(c[3]), "=r"(tmp0), "=r"(tmp1)
  :
  : "memory", "cc"
  );
}

DECL(void, ser)(uint8_t *buffer, argElement_1w c) {
  FN(copy)((argElement_1w)buffer,c);
  FN(modp)((argElement_1w)buffer);
}

DECL(void, unser)(argElement_1w c, uint8_t *buffer) {
  FN(copy)(c,(argElement_1w)buffer);
  FN(modp)(c);
}

DECL(int, sgn)(argElement_1w c) {
  FN(modp)(c);
  return c[0]&0x1;
}

DECL(int, cmp)(argElement_1w a, argElement_1w b) {
  FN(modp)(a);
  FN(modp)(b);
  return cmp_bytes((uint8_t *) a, (uint8_t *) b, SIZE_FP25519);
}

DECL(void, print)(FILE *file, argElement_1w a) {
  print_hex_bytes(file, (uint8_t *) a, SIZE_FP25519);
}

DECL(void, intmul_mulxadx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "movq   (%1), %%rdx; " /* A[0] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[0]*B[0] */    "xorl %%r10d, %%r10d ;"                           "movq  %%r8,  (%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[0]*B[1] */    "adox  %%r9, %%r10 ;"                             "movq %%r10, 8(%0) ;"
  "mulx 16(%2), %%r12, %%r13; " /* A[0]*B[2] */    "adox %%r11, %%r12 ;"
  "mulx 24(%2), %%r14, %%rdx; " /* A[0]*B[3] */    "adox %%r13, %%r14 ;"                                                       "movq $0, %%rax ;"
  /*******************************************/    "adox %%rdx, %%rax ;"

  "movq  8(%1), %%rdx; " /* A[1] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[1]*B[0] */    "xorl %%r10d, %%r10d ;"  "adcx 8(%0),  %%r8 ;"    "movq  %%r8,  8(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[1]*B[1] */    "adox  %%r9, %%r10 ;"    "adcx %%r12, %%r10 ;"    "movq %%r10, 16(%0) ;"
  "mulx 16(%2), %%r12, %%r13; " /* A[1]*B[2] */    "adox %%r11, %%r12 ;"    "adcx %%r14, %%r12 ;"                              "movq $0, %%r8  ;"
  "mulx 24(%2), %%r14, %%rdx; " /* A[1]*B[3] */    "adox %%r13, %%r14 ;"    "adcx %%rax, %%r14 ;"                              "movq $0, %%rax ;"
  /*******************************************/    "adox %%rdx, %%rax ;"    "adcx  %%r8, %%rax ;"

  "movq 16(%1), %%rdx; " /* A[2] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[2]*B[0] */    "xorl %%r10d, %%r10d ;"  "adcx 16(%0), %%r8 ;"    "movq  %%r8, 16(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[2]*B[1] */    "adox  %%r9, %%r10 ;"    "adcx %%r12, %%r10 ;"    "movq %%r10, 24(%0) ;"
  "mulx 16(%2), %%r12, %%r13; " /* A[2]*B[2] */    "adox %%r11, %%r12 ;"    "adcx %%r14, %%r12 ;"                              "movq $0, %%r8  ;"
  "mulx 24(%2), %%r14, %%rdx; " /* A[2]*B[3] */    "adox %%r13, %%r14 ;"    "adcx %%rax, %%r14 ;"                              "movq $0, %%rax ;"
  /*******************************************/    "adox %%rdx, %%rax ;"    "adcx  %%r8, %%rax ;"

  "movq 24(%1), %%rdx; " /* A[3] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[3]*B[0] */    "xorl %%r10d, %%r10d ;"  "adcx 24(%0), %%r8 ;"    "movq  %%r8, 24(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[3]*B[1] */    "adox  %%r9, %%r10 ;"    "adcx %%r12, %%r10 ;"    "movq %%r10, 32(%0) ;"
  "mulx 16(%2), %%r12, %%r13; " /* A[3]*B[2] */    "adox %%r11, %%r12 ;"    "adcx %%r14, %%r12 ;"    "movq %%r12, 40(%0) ;"    "movq $0, %%r8  ;"
  "mulx 24(%2), %%r14, %%rdx; " /* A[3]*B[3] */    "adox %%r13, %%r14 ;"    "adcx %%rax, %%r14 ;"    "movq %%r14, 48(%0) ;"    "movq $0, %%rax ;"
  /*******************************************/    "adox %%rdx, %%rax ;"    "adcx  %%r8, %%rax ;"    "movq %%rax, 56(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rdx", "%r8",
  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intmul_mulx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "movq   (%1), %%rdx; " /* A[0] */
  "mulx   (%2),  %%r8, %%r12; " /* A[0]*B[0] */                           "movq %%r8,  (%0) ;"
  "mulx  8(%2), %%r10, %%rax; " /* A[0]*B[1] */    "addq %%r10, %%r12 ;"
  "mulx 16(%2),  %%r8, %%rbx; " /* A[0]*B[2] */    "adcq  %%r8, %%rax ;"
  "mulx 24(%2), %%r10, %%rcx; " /* A[0]*B[3] */    "adcq %%r10, %%rbx ;"
  /*******************************************/    "adcq    $0, %%rcx ;"

  "movq  8(%1), %%rdx; " /* A[1] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[1]*B[0] */    "addq %%r12,  %%r8 ;"  "movq %%r8, 8(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[1]*B[1] */    "adcq %%r10,  %%r9 ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[1]*B[2] */    "adcq  %%r8, %%r11 ;"
  "mulx 24(%2), %%r10, %%r12; " /* A[1]*B[3] */    "adcq %%r10, %%r13 ;"
  /*******************************************/    "adcq    $0, %%r12 ;"

  "addq  %%r9, %%rax ;"
  "adcq %%r11, %%rbx ;"
  "adcq %%r13, %%rcx ;"
  "adcq    $0, %%r12 ;"

  "movq 16(%1), %%rdx; " /* A[2] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[2]*B[0] */    "addq %%rax,  %%r8 ;"  "movq %%r8, 16(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[2]*B[1] */    "adcq %%r10,  %%r9 ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[2]*B[2] */    "adcq  %%r8, %%r11 ;"
  "mulx 24(%2), %%r10, %%rax; " /* A[2]*B[3] */    "adcq %%r10, %%r13 ;"
  /*******************************************/    "adcq    $0, %%rax ;"

  "addq  %%r9, %%rbx ;"
  "adcq %%r11, %%rcx ;"
  "adcq %%r13, %%r12 ;"
  "adcq    $0, %%rax ;"

  "movq 24(%1), %%rdx; " /* A[3] */
  "mulx   (%2),  %%r8,  %%r9; " /* A[3]*B[0] */    "addq %%rbx,  %%r8 ;"  "movq %%r8, 24(%0) ;"
  "mulx  8(%2), %%r10, %%r11; " /* A[3]*B[1] */    "adcq %%r10,  %%r9 ;"
  "mulx 16(%2),  %%r8, %%r13; " /* A[3]*B[2] */    "adcq  %%r8, %%r11 ;"
  "mulx 24(%2), %%r10, %%rbx; " /* A[3]*B[3] */    "adcq %%r10, %%r13 ;"
  /*******************************************/    "adcq    $0, %%rbx ;"

  "addq  %%r9, %%rcx ;"  "movq %%rcx, 32(%0) ;"
  "adcq %%r11, %%r12 ;"  "movq %%r12, 40(%0) ;"
  "adcq %%r13, %%rax ;"  "movq %%rax, 48(%0) ;"
  "adcq    $0, %%rbx ;"  "movq %%rbx, 56(%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
  );
}

DECL(void, intmul_mulq)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
    "movq  0(%2), %%r8;"
	"movq  0(%1), %%rax;" "mulq %%r8;" "movq %%rax, 0(%0);" "movq %%rdx, %%r15;"
	"movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r13;" "movq %%rdx, %%r10;"
	"movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;"
	"movq 24(%1), %%rax;" "mulq %%r8;"
	"addq %%r13, %%r15;"
	"adcq %%r14, %%r10;"  "movq %%r10, 16(%0);"
	"adcq %%rax, %%r11;"  "movq %%r11, 24(%0);"
	"adcq    $0, %%rdx;"  "movq %%rdx, 32(%0);"
	"movq  8(%2), %%r8;"
	"movq  0(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r12;" "movq %%rdx,  %%r9;"
	"movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r13;" "movq %%rdx, %%r10;"
	"movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;"
	"movq 24(%1), %%rax;" "mulq %%r8;"
	"addq %%r12, %%r15;" "movq %%r15,  8(%0);"
	"adcq %%r13,  %%r9;"
	"adcq %%r14, %%r10;"
	"adcq %%rax, %%r11;"
	"adcq    $0, %%rdx;"
	"adcq 16(%0),  %%r9;"  "movq  %%r9,  %%r15;"
	"adcq 24(%0), %%r10;"  "movq %%r10, 24(%0);"
	"adcq 32(%0), %%r11;"  "movq %%r11, 32(%0);"
	"adcq     $0, %%rdx;"  "movq %%rdx, 40(%0);"
	"movq 16(%2), %%r8;"
	"movq  0(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r12;" "movq %%rdx,  %%r9;"
	"movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r13;" "movq %%rdx, %%r10;"
	"movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;"
	"movq 24(%1), %%rax;" "mulq %%r8;"
	"addq %%r12, %%r15;"  "movq %%r15, 16(%0);"
	"adcq %%r13,  %%r9;"
	"adcq %%r14, %%r10;"
	"adcq %%rax, %%r11;"
	"adcq    $0, %%rdx;"
	"adcq 24(%0),  %%r9;"  "movq  %%r9,  %%r15;"
	"adcq 32(%0), %%r10;"  "movq %%r10, 32(%0);"
	"adcq 40(%0), %%r11;"  "movq %%r11, 40(%0);"
	"adcq     $0, %%rdx;"  "movq %%rdx, 48(%0);"
	"movq 24(%2), %%r8;"
	"movq  0(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r12;" "movq %%rdx,  %%r9;"
	"movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r13;" "movq %%rdx, %%r10;"
	"movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;"
	"movq 24(%1), %%rax;" "mulq %%r8;"
	"addq %%r12, %%r15;" "movq %%r15, 24(%0);"
	"adcq %%r13,  %%r9;"
	"adcq %%r14, %%r10;"
	"adcq %%rax, %%r11;"
	"adcq    $0, %%rdx;"
	"adcq 32(%0),  %%r9;"  "movq  %%r9, 32(%0);"
	"adcq 40(%0), %%r10;"  "movq %%r10, 40(%0);"
	"adcq 48(%0), %%r11;"  "movq %%r11, 48(%0);"
    "adcq     $0, %%rdx;"  "movq %%rdx, 56(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rdx", "%r8", "%r9",
  "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
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
  "movq   (%1), %%rdx        ;" /* A[0]      */
  "mulx  8(%1),  %%r8, %%r14 ;" /* A[1]*A[0] */  "xorl %%r15d, %%r15d;"
  "mulx 16(%1),  %%r9, %%r10 ;" /* A[2]*A[0] */  "adcx %%r14,  %%r9 ;"
  "mulx 24(%1), %%rax, %%rcx ;" /* A[3]*A[0] */  "adcx %%rax, %%r10 ;"
  "movq 24(%1), %%rdx        ;" /* A[3]      */
  "mulx  8(%1), %%r11, %%r12 ;" /* A[1]*A[3] */  "adcx %%rcx, %%r11 ;"
  "mulx 16(%1), %%rax, %%r13 ;" /* A[2]*A[3] */  "adcx %%rax, %%r12 ;"
  "movq  8(%1), %%rdx        ;" /* A[1]      */  "adcx %%r15, %%r13 ;"
  "mulx 16(%1), %%rax, %%rcx ;" /* A[2]*A[1] */  "movq    $0, %%r14 ;"
  /*******************************************/  "adcx %%r15, %%r14 ;"

  "xorl %%r15d, %%r15d;"
  "adox %%rax, %%r10 ;"  "adcx  %%r8,  %%r8 ;"
  "adox %%rcx, %%r11 ;"  "adcx  %%r9,  %%r9 ;"
  "adox %%r15, %%r12 ;"  "adcx %%r10, %%r10 ;"
  "adox %%r15, %%r13 ;"  "adcx %%r11, %%r11 ;"
  "adox %%r15, %%r14 ;"  "adcx %%r12, %%r12 ;"
  "adcx %%r13, %%r13 ;"
  "adcx %%r14, %%r14 ;"

  "movq   (%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[0]^2 */
  /********************/  "movq %%rax,  0(%0) ;"
  "addq %%rcx,  %%r8 ;"   "movq  %%r8,  8(%0) ;"
  "movq  8(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[1]^2 */
  "adcq %%rax,  %%r9 ;"   "movq  %%r9, 16(%0) ;"
  "adcq %%rcx, %%r10 ;"   "movq %%r10, 24(%0) ;"
  "movq 16(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[2]^2 */
  "adcq %%rax, %%r11 ;"   "movq %%r11, 32(%0) ;"
  "adcq %%rcx, %%r12 ;"   "movq %%r12, 40(%0) ;"
  "movq 24(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[3]^2 */
  "adcq %%rax, %%r13 ;"   "movq %%r13, 48(%0) ;"
  "adcq %%rcx, %%r14 ;"   "movq %%r14, 56(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
  );
}

DECL(void, intsqr_mulx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movq  8(%1), %%rdx        ;" /* A[1]      */
  "mulx   (%1),  %%r8,  %%r9 ;" /* A[0]*A[1] */
  "mulx 16(%1), %%r10, %%r11 ;" /* A[2]*A[1] */
  "mulx 24(%1), %%rcx, %%r14 ;" /* A[3]*A[1] */

  "movq 16(%1), %%rdx        ;" /* A[2]      */
  "mulx 24(%1), %%r12, %%r13 ;" /* A[3]*A[2] */
  "mulx   (%1), %%rax, %%rdx ;" /* A[0]*A[2] */

  "addq %%rax,  %%r9 ;"
  "adcq %%rdx, %%r10 ;"
  "adcq %%rcx, %%r11 ;"
  "adcq %%r14, %%r12 ;"
  "adcq    $0, %%r13 ;"
  "movq    $0, %%r14 ;"
  "adcq    $0, %%r14 ;"

  "movq   (%1), %%rdx        ;" /* A[0]      */
  "mulx 24(%1), %%rax, %%rcx ;" /* A[0]*A[3] */

  "addq %%rax, %%r10 ;"
  "adcq %%rcx, %%r11 ;"
  "adcq    $0, %%r12 ;"
  "adcq    $0, %%r13 ;"
  "adcq    $0, %%r14 ;"

  "shldq $1, %%r13, %%r14 ;"
  "shldq $1, %%r12, %%r13 ;"
  "shldq $1, %%r11, %%r12 ;"
  "shldq $1, %%r10, %%r11 ;"
  "shldq $1,  %%r9, %%r10 ;"
  "shldq $1,  %%r8,  %%r9 ;"
  "shlq  $1,  %%r8        ;"

  /********************/  "mulx %%rdx, %%rax, %%rcx ;" /* A[0]^2 */
  /********************/  "movq %%rax,  0(%0) ;"
  "addq %%rcx,  %%r8 ;"   "movq  %%r8,  8(%0) ;"
  "movq  8(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[1]^2 */
  "adcq %%rax,  %%r9 ;"   "movq  %%r9, 16(%0) ;"
  "adcq %%rcx, %%r10 ;"   "movq %%r10, 24(%0) ;"
  "movq 16(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[2]^2 */
  "adcq %%rax, %%r11 ;"   "movq %%r11, 32(%0) ;"
  "adcq %%rcx, %%r12 ;"   "movq %%r12, 40(%0) ;"
  "movq 24(%1), %%rdx ;"  "mulx %%rdx, %%rax, %%rcx ;" /* A[3]^2 */
  "adcq %%rax, %%r13 ;"   "movq %%r13, 48(%0) ;"
  "adcq %%rcx, %%r14 ;"   "movq %%r14, 56(%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
  );
}

DECL(void, intsqr_mulq)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movq  0(%1),  %%r8;"
  "movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax,  %%r9;" "movq %%rdx, %%r10;" /* a[0]*a[1] */
  "movq 16(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r14;" "movq %%rdx, %%r11;" /* a[0]*a[2] */
  "movq 24(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%r15;" "movq %%rdx, %%r12;" /* a[0]*a[3] */
  "movq 24(%1),  %%r8;"
  "movq  8(%1), %%rax;" "mulq %%r8;" "movq %%rax, %%rcx;" "movq %%rdx, %%r13;" /* a[3]*a[1] */
  "movq 16(%1), %%rax;" "mulq %%r8;" /* a[3]*a[2] */

  "addq %%r14, %%r10;"
  "adcq %%r15, %%r11;" "movl $0, %%r15d;"
  "adcq %%rcx, %%r12;"
  "adcq %%rax, %%r13;"
  "adcq    $0, %%rdx;" "movq %%rdx, %%r14;"
  "movq 8(%1), %%rax;" "mulq 16(%1);"

  "addq %%rax, %%r11;"
  "adcq %%rdx, %%r12;"
  "adcq    $0, %%r13;"
  "adcq    $0, %%r14;"
  "adcq    $0, %%r15;"

  "shldq $1, %%r14, %%r15;" "movq %%r15, 56(%0);"
  "shldq $1, %%r13, %%r14;" "movq %%r14, 48(%0);"
  "shldq $1, %%r12, %%r13;" "movq %%r13, 40(%0);"
  "shldq $1, %%r11, %%r12;" "movq %%r12, 32(%0);"
  "shldq $1, %%r10, %%r11;" "movq %%r11, 24(%0);"
  "shldq $1,  %%r9, %%r10;" "movq %%r10, 16(%0);"
  "shlq  $1,  %%r9; "       "movq  %%r9,  8(%0);"

  "movq  0(%1),%%rax;" "mulq %%rax;" "movq %%rax, 0(%0);" "movq %%rdx,  %%r9;"
  "movq  8(%1),%%rax;" "mulq %%rax;" "movq %%rax, %%r10;" "movq %%rdx, %%r11;"
  "movq 16(%1),%%rax;" "mulq %%rax;" "movq %%rax, %%r12;" "movq %%rdx, %%r13;"
  "movq 24(%1),%%rax;" "mulq %%rax;" "movq %%rax, %%r14;" "movq %%rdx, %%r15;"

  "addq  8(%0),  %%r9;" "movq  %%r9,  8(%0);"
  "adcq 16(%0), %%r10;" "movq %%r10, 16(%0);"
  "adcq 24(%0), %%r11;" "movq %%r11, 24(%0);"
  "adcq 32(%0), %%r12;" "movq %%r12, 32(%0);"
  "adcq 40(%0), %%r13;" "movq %%r13, 40(%0);"
  "adcq 48(%0), %%r14;" "movq %%r14, 48(%0);"
  "adcq 56(%0), %%r15;" "movq %%r15, 56(%0);"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
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

DECL(void, reduce_mulxadx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movl    $38, %%edx ;" /* 2*c = 38 = 2^256 */
  "mulx 32(%1),  %%r8, %%r10 ;" /* c*C[4] */  "xorl %%ebx, %%ebx ;"  "adox   (%1),  %%r8 ;"
  "mulx 40(%1),  %%r9, %%r11 ;" /* c*C[5] */  "adcx %%r10,  %%r9 ;"  "adox  8(%1),  %%r9 ;"
  "mulx 48(%1), %%r10, %%rax ;" /* c*C[6] */  "adcx %%r11, %%r10 ;"  "adox 16(%1), %%r10 ;"
  "mulx 56(%1), %%r11, %%rcx ;" /* c*C[7] */  "adcx %%rax, %%r11 ;"  "adox 24(%1), %%r11 ;"
  /****************************************/  "adcx %%rbx, %%rcx ;"  "adox  %%rbx, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0, of=0 */
  "adcx %%rcx,  %%r8 ;"
  "adcx %%rbx,  %%r9 ;"  "movq  %%r9,  8(%0) ;"
  "adcx %%rbx, %%r10 ;"  "movq %%r10, 16(%0) ;"
  "adcx %%rbx, %%r11 ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8,   (%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, reduce_mulx)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  "movl    $38, %%edx ;" /* 2*c = 38 = 2^256 */
  "mulx 32(%1),  %%r8, %%r10 ;" /* c*C[4] */
  "mulx 40(%1),  %%r9, %%r11 ;" /* c*C[5] */  "addq %%r10,  %%r9 ;"
  "mulx 48(%1), %%r10, %%rax ;" /* c*C[6] */  "adcq %%r11, %%r10 ;"
  "mulx 56(%1), %%r11, %%rcx ;" /* c*C[7] */  "adcq %%rax, %%r11 ;"
  /****************************************/  "adcq    $0, %%rcx ;"
  "addq   (%1),  %%r8 ;"
  "adcq  8(%1),  %%r9 ;"
  "adcq 16(%1), %%r10 ;"
  "adcq 24(%1), %%r11 ;"
  "adcq     $0, %%rcx ;"
  "imul %%rdx, %%rcx ;" /* c*C[4], cf=0 */
  "addq %%rcx,  %%r8 ;"
  "adcq    $0,  %%r9 ;"  "movq  %%r9,  8(%0) ;"
  "adcq    $0, %%r10 ;"  "movq %%r10, 16(%0) ;"
  "adcq    $0, %%r11 ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8,   (%0) ;"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, reduce_mulq)(argElement_1w c, argElement_1w a) {
  __asm__ __volatile__(
  /* 2*C = 38 = 2^256 */ \
    "movl $38, %%eax;" "mulq 32(%1);" "movq %%rax,  %%r8;" "movq %%rdx,  %%r9;" /* c*c[4] */
    "movl $38, %%eax;" "mulq 40(%1);" "movq %%rax, %%r12;" "movq %%rdx, %%r10;" /* c*c[5] */
    "movl $38, %%eax;" "mulq 48(%1);" "movq %%rax, %%r13;" "movq %%rdx, %%r11;" /* c*c[6] */
    "movl $38, %%eax;" "mulq 56(%1);" /* c*c[7] */
    "addq %%r12,  %%r9;"
    "adcq %%r13, %%r10;"
    "adcq %%rax, %%r11;"
    "adcq    $0, %%rdx;"
    "addq  0(%1),  %%r8;"
    "adcq  8(%1),  %%r9;"
    "adcq 16(%1), %%r10;"
    "adcq 24(%1), %%r11;"
    "adcq     $0, %%rdx;"
    "movl $38, %%eax;"
    "imulq %%rax, %%rdx;" /* c*c[4], cf=0, of=0 */
    "addq  %%rdx,  %%r8;"
    "adcq $0,  %%r9;" "movq  %%r9,  8(%0);"
    "adcq $0, %%r10;" "movq %%r10, 16(%0);"
    "adcq $0, %%r11;" "movq %%r11, 24(%0);"
    "movl $0, %%edx;"
    "cmovc %%rax, %%rdx;"
    "addq %%rdx, %%r8;" "movq %%r8, 0(%0)"
  :
  : "r" (c), "r" (a)
  : "memory", "cc", "%rax", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
  );
}

DECL(void, reduce)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
#ifdef __ADX__
  FN(reduce_mulxadx)(c,a);
#else /* No __ADX__ */
  FN(reduce_mulx)(c, a);
#endif
#else /* No __BMI2__ */
  FN(reduce_mulq)(c,a);
#endif
}

DECL(void, add_adx)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
    "movl $38, %%eax;"
    "xorl  %%ecx, %%ecx;"
    "movq  0(%2),  %%r8;"  "adcx  0(%1),  %%r8;"
    "movq  8(%2),  %%r9;"  "adcx  8(%1),  %%r9;"
    "movq 16(%2), %%r10;"  "adcx 16(%1), %%r10;"
    "movq 24(%2), %%r11;"  "adcx 24(%1), %%r11;"
    "cmovc %%eax, %%ecx;"
    "xorl %%eax, %%eax;"
    "adcx %%rcx,  %%r8;"
    "adcx %%rax,  %%r9;"  "movq  %%r9,  8(%0);"
    "adcx %%rax, %%r10;"  "movq %%r10, 16(%0);"
    "adcx %%rax, %%r11;"  "movq %%r11, 24(%0);"
    "movl $38, %%ecx;"
    "cmovc %%ecx, %%eax;"
    "addq %%rax,  %%r8;"  "movq  %%r8,  0(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rcx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, add_adc)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "movl $38, %%eax;"
  "movq  0(%2),  %%r8;"  "addq  0(%1),  %%r8;"
  "movq  8(%2),  %%r9;"  "adcq  8(%1),  %%r9;"
  "movq 16(%2), %%r10;"  "adcq 16(%1), %%r10;"
  "movq 24(%2), %%r11;"  "adcq 24(%1), %%r11;"
  "movl $0, %%ecx;"
  "cmovc %%eax, %%ecx;"
  "addq %%rcx,  %%r8;"
  "adcq $0,  %%r9;"   "movq  %%r9,  8(%0);"
  "adcq $0, %%r10;"   "movq %%r10, 16(%0);"
  "adcq $0, %%r11;"   "movq %%r11, 24(%0);"
  "movl $0, %%ecx;"
  "cmovc %%eax, %%ecx;"
  "addq %%rcx, %%r8;" "movq  %%r8,  0(%0);"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rcx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, add)(argElement_1w c, argElement_1w a, argElement_1w b) {
#ifdef __ADX__
  FN(add_adx)(c,a,b);
#else
  FN(add_adc)(c, a, b);
#endif
}

DECL(void, sub)(argElement_1w c, argElement_1w a, argElement_1w b) {
  __asm__ __volatile__(
  "mov     $38, %%eax ;"
  "movq   (%1),  %%r8 ;"  "subq   (%2),  %%r8 ;"
  "movq  8(%1),  %%r9 ;"  "sbbq  8(%2),  %%r9 ;"
  "movq 16(%1), %%r10 ;"  "sbbq 16(%2), %%r10 ;"
  "movq 24(%1), %%r11 ;"  "sbbq 24(%2), %%r11 ;"
  "mov      $0, %%ecx ;"
  "cmovc %%eax, %%ecx ;"
  "subq %%rcx,  %%r8  ;"
  "sbbq    $0,  %%r9  ;"  "movq  %%r9,  8(%0) ;"
  "sbbq    $0, %%r10  ;"  "movq %%r10, 16(%0) ;"
  "sbbq    $0, %%r11  ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx  ;"
  "cmovc %%eax, %%ecx ;"
  "subq %%rcx,  %%r8  ;"  "movq  %%r8,   (%0) ;"
  :
  : "r" (c), "r" (a), "r" (b)
  : "memory", "cc", "%rax", "%rcx", "%r8", "%r9", "%r10", "%r11"
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
  :
  : "r" (c), "r" (a), "r" (b), "r" (bit)
  : "memory", "cc", "%rax"
  );
}

DECL(void, neg)(argElement_1w c) {
  EltFp25519_1w_fullradix zero = {0};
  FN(sub)(c, zero, c);
}

DECL(void, mul)(argElement_1w c, argElement_1w a, argElement_1w b) {
  EltFp25519_1w_fullradix_buffer buffer_1w;
  FN(intmul)(buffer_1w, a, b);
  FN(reduce)(c, buffer_1w);
}

DECL(void, sqr)(argElement_1w a) {
  EltFp25519_1w_fullradix_buffer buffer_1w;
  FN(intsqr)(buffer_1w, a);
  FN(reduce)(a, buffer_1w);
}

#define sqrn_Fp255_1w_fullradix(A, times) \
  counter = times;\
  while ( counter-- > 0) {\
      sqr_Fp255_1w_fullradix(A);\
  }

DECL(void, inv)(argElement_1w c, argElement_1w a) {
  EltFp25519_1w_fullradix x0, x1, x2;
  argElement_1w T[5];
  int counter ;

  T[0] = x0;
  T[1] = c; /* x^(-1) */
  T[2] = x1;
  T[3] = x2;
  T[4] = a; /* x */

  FN(copy)(T[1], T[4]);
  sqrn_Fp255_1w_fullradix(T[1], 1);
  FN(copy)(T[2], T[1]);
  sqrn_Fp255_1w_fullradix(T[2], 2);
  FN(mul )(T[0], T[4], T[2]);
  FN(mul )(T[1], T[1], T[0]);
  FN(copy)(T[2], T[1]);
  sqrn_Fp255_1w_fullradix(T[2], 1);
  FN(mul )(T[0], T[0], T[2]);
  FN(copy)(T[2], T[0]);
  sqrn_Fp255_1w_fullradix(T[2], 5);
  FN(mul )(T[0], T[0], T[2]);
  FN(copy)(T[2], T[0]);
  sqrn_Fp255_1w_fullradix(T[2], 10);
  FN(mul )(T[2], T[2], T[0]);
  FN(copy)(T[3], T[2]);
  sqrn_Fp255_1w_fullradix(T[3], 20);
  FN(mul )(T[3], T[3], T[2]);
  sqrn_Fp255_1w_fullradix(T[3], 10);
  FN(mul )(T[3], T[3], T[0]);
  FN(copy)(T[0], T[3]);
  sqrn_Fp255_1w_fullradix(T[0], 50);
  FN(mul )(T[0], T[0], T[3]);
  FN(copy)(T[2], T[0]);
  sqrn_Fp255_1w_fullradix(T[2], 100);
  FN(mul )(T[2], T[2], T[0]);
  sqrn_Fp255_1w_fullradix(T[2], 50);
  FN(mul )(T[2], T[2], T[3]);
  sqrn_Fp255_1w_fullradix(T[2], 5);
  FN(mul )(T[1], T[1], T[2]);
}

DECL(void, invsqrt)(argElement_1w uv_p38, argElement_1w u, argElement_1w v) {
  EltFp25519_1w_fullradix sqrt_minus_one = {
      0xc4ee1b274a0ea0b0,
      0x2f431806ad2fe478,
      0x2b4d00993dfbd7a7,
      0x2b8324804fc1df0b
  };
  EltFp25519_1w_fullradix v2;
  EltFp25519_1w_fullradix uv, uv3, uv7;
  EltFp25519_1w_fullradix x2, x9, uv7_p58;
  argElement_1w Tab[4];
  int counter;

  /*  uv = u*v     */
  FN(mul)(uv, u, v);
  FN(copy)(v2, v);
  /*  v2 = v^2     */
  FN(sqr)(v2);
  /*  uv3 = uv*v^2 */
  FN(mul)(uv3, uv, v2);
  /*  v2 = (v^2)^2 */
  FN(sqr)(v2);
  /*  uv7 = uv3*v2 */
  FN(mul)(uv7, uv3, v2);

  Tab[0] = x2;
  Tab[1] = x9;
  Tab[2] = uv7_p58;
  Tab[3] = uv7;

  FN(copy)(Tab[0], uv7);
  FN(sqr)(Tab[0]);/* x^2 */
  /* 0 */
  FN(copy)(Tab[1], Tab[0]);
  sqrn_Fp255_1w_fullradix(Tab[1], 2);
  FN(mul)(Tab[1], Tab[1], Tab[3]);
  /* 1 */
  FN(mul)(Tab[0], Tab[0], Tab[1]);
  /* 2 */
  sqrn_Fp255_1w_fullradix(Tab[0], 1);
  FN(mul)(Tab[0], Tab[0], Tab[1]);
  /* 3 */
  FN(copy)(Tab[1], Tab[0]);
  sqrn_Fp255_1w_fullradix(Tab[1], 5);
  FN(mul)(Tab[1], Tab[1], Tab[0]);
  /* 4 */
  FN(copy)(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 5);
  FN(mul)(Tab[2], Tab[2], Tab[0]);
  /* 5 */
  FN(copy)(Tab[1], Tab[2]);
  sqrn_Fp255_1w_fullradix(Tab[1], 15);
  FN(mul)(Tab[1], Tab[1], Tab[2]);
  /* 6 */
  FN(copy)(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 30);
  FN(mul)(Tab[2], Tab[2], Tab[1]);
  /* 7 */
  FN(copy)(Tab[1], Tab[2]);
  sqrn_Fp255_1w_fullradix(Tab[1], 60);
  FN(mul)(Tab[1], Tab[1], Tab[2]);
  /* 8 */
  sqrn_Fp255_1w_fullradix(Tab[1], 5);
  FN(mul)(Tab[1], Tab[1], Tab[0]);
  /* 9 */
  FN(copy)(Tab[2], Tab[1]);
  sqrn_Fp255_1w_fullradix(Tab[2], 125);
  FN(mul)(Tab[2], Tab[2], Tab[1]);
  /* 10 */
  sqrn_Fp255_1w_fullradix(Tab[2], 2);
  FN(mul)(Tab[2], Tab[2], Tab[3]);

  /* uv^(p+3)/8 = uv3*uv^(p-5)/8  */
  FN(mul)(uv_p38, uv7_p58, uv3);

  /**
   * Checking whether
   *      v*uv_p38^2 == -u
   **/
  FN(copy)(uv3, uv_p38);
  /* uv3 = uv_p38^2  */
  FN(sqr)(uv3);
  /* uv  = uv3*v     */
  FN(mul)(uv, uv3, v);
  /*  u  = -u        */
  FN(neg)(u);

  if (FN(cmp)(uv, u) == 0) {
    /*  uv_p38 = uv_p38*sqrt(-1) */
    FN(mul)(uv_p38, uv_p38, sqrt_minus_one);
  }
}

#undef sqrn_Fp255_1w_fullradix

DECL(void, srt)(argElement_1w c, argElement_1w a) {
  FN(invsqrt)(c, c, a);
}

DECL(void, mula24_mulx)(argElement_1w c, argElement_1w a) {
  const uint64_t a24 = 121666;
  __asm__ __volatile__(
  "movq     %2, %%rdx ;"
  "mulx   (%1),  %%r8, %%r10 ;"
  "mulx  8(%1),  %%r9, %%r11 ;"  "addq %%r10,  %%r9 ;"
  "mulx 16(%1), %%r10, %%rax ;"  "adcq %%r11, %%r10 ;"
  "mulx 24(%1), %%r11, %%rcx ;"  "adcq %%rax, %%r11 ;"
  /***************************/  "adcq    $0, %%rcx ;"
  "movl   $38, %%edx ;" /* 2*c = 38 = 2^256 mod 2^255-19*/
  "imul %%rdx, %%rcx ;"
  "addq %%rcx,  %%r8 ;"
  "adcq    $0,  %%r9 ;"  "movq  %%r9,  8(%0) ;"
  "adcq    $0, %%r10 ;"  "movq %%r10, 16(%0) ;"
  "adcq    $0, %%r11 ;"  "movq %%r11, 24(%0) ;"
  "mov     $0, %%ecx ;"
  "cmovc %%edx, %%ecx ;"
  "addq %%rcx,  %%r8 ;"  "movq  %%r8,   (%0) ;"
  :
  : "r" (c), "r" (a), "r" (a24)
  : "memory", "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
  );
}

DECL(void, mula24_mulq)(argElement_1w c, argElement_1w a) {
  const uint64_t a24 = 121666;
  __asm__ __volatile__(
  "movq %2, %%rax;" "mulq  0(%1);" "movq %%rax,  %%r8;" "movq %%rdx,  %%r9;"
  "movq %2, %%rax;" "mulq  8(%1);" "movq %%rax, %%r12;" "movq %%rdx, %%r10;"
  "movq %2, %%rax;" "mulq 16(%1);" "movq %%rax, %%r13;" "movq %%rdx, %%r11;"
  "movq %2, %%rax;" "mulq 24(%1);"
  "addq %%r12,  %%r9;"
  "adcq %%r13, %%r10;"
  "adcq %%rax, %%r11;"
  "adcq    $0, %%rdx;"
  "movl   $38, %%eax;" /* 2*c = 38 = 2^256 mod 2^255-19*/
  "imulq %%rax, %%rdx;"
  "addq %%rdx, %%r8;"
  "adcq $0,  %%r9;"  "movq  %%r9,  8(%0);"
  "adcq $0, %%r10;"  "movq %%r10, 16(%0);"
  "adcq $0, %%r11;"  "movq %%r11, 24(%0);"
  "movq $0, %%rdx;"
  "cmovc %%rax, %%rdx;"
  "addq %%rdx, %%r8;" "movq %%r8,  0(%0);"
  :
  : "r" (c), "r" (a), "r" (a24)
  : "memory", "cc", "%rax", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
  );
}

DECL(void, mula24)(argElement_1w c, argElement_1w a) {
#ifdef __BMI2__
  FN(mula24_mulx)(c, a);
#else /* Without BMI2 */
  FN(mula24_mulq)(c,a);
#endif
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP255 */
