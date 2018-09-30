#include <stdio.h>
#include <cpuid.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum Capabilities {
  CMOV = bit_CMOV,
  SSE = bit_SSE,
  SSE2 = bit_SSE2,
  SSE3 = bit_SSE3,
  SSSE3 = bit_SSSE3,
  SSE4_1 = bit_SSE4_1,
  SSE4_2 = bit_SSE4_2,
  AVX = bit_AVX,
  AVX2 = bit_AVX2,
  BMI = bit_BMI,
  BMI2 = bit_BMI2,
  ADX = bit_ADX
};

#define my_cpuid(ra, rb, rc, rd) __asm volatile \
(    \
    "mov %0, %%eax      \n"\
    "mov %1, %%ebx      \n"\
    "mov %2, %%ecx      \n"\
    "mov %3, %%edx      \n"\
    "cpuid      \n"\
    "mov %%eax, %0      \n"\
    "mov %%ebx, %1      \n"\
    "mov %%ecx, %2      \n"\
    "mov %%edx, %3      \n"\
:"+r" (ra), "+r" (rb), "+r" (rc), "+r" (rd)\
: : "%eax", "%ebx", "%ecx", "%edx")

#define test_capability(REG, CAP) \
 printf("%-12s: [%s]\n",#CAP,( (REG & CAP) != 0 )?"Yes":"No");\
 run += (REG & CAP) != 0;

int cpu_has(enum Capabilities c) {
  int ret = 0;
  unsigned int eax, ebx, ecx, edx;

  eax = 1;
  ebx = 0;
  ecx = 0;
  edx = 0;
  my_cpuid(eax, ebx, ecx, edx);

  ret |= (edx & c) != 0;
  ret |= (ecx & c) != 0;

  eax = 7;
  ebx = 0;
  ecx = 0;
  edx = 0;
  my_cpuid(eax, ebx, ecx, edx);

  ret |= (ebx & c) != 0;
  return ret;
}

void machine_info() {
  printf("=== Environment Information ====\n");
  printf("Program compiled with: %s\n", __VERSION__);
  unsigned int eax, ebx, ecx, edx;
  unsigned int run = 0;

  eax = 1;
  ebx = 0;
  ecx = 0;
  edx = 0;
  my_cpuid(eax, ebx, ecx, edx);

  test_capability(edx, bit_CMOV);
  test_capability(edx, bit_SSE);
  test_capability(edx, bit_SSE2);
  test_capability(ecx, bit_SSE3);
  test_capability(ecx, bit_SSSE3);
  test_capability(ecx, bit_SSE4_1);
  test_capability(ecx, bit_SSE4_2);
  test_capability(ecx, bit_AVX);

  eax = 7;
  ebx = 0;
  ecx = 0;
  edx = 0;
  my_cpuid(eax, ebx, ecx, edx);
  test_capability(ebx, bit_AVX2);
  test_capability(ebx, bit_BMI);
  test_capability(ebx, bit_BMI2);
  test_capability(ebx, bit_ADX);

  printf("Machine supports our library: %s\n", (run == 11 || run == 12) ? "Yes" : "No");
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
