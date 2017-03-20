#include <stdio.h>
#include <cpuid.h>

#define my_cpuid(ra,rb,rc,rd) __asm volatile \
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

#define test_capability(REG,CAP)  printf("%-12s: [%s]\n",#CAP,( (REG & CAP) != 0 )?"Yes":"No");

#ifndef bit_BMI
#define bit_BMI	(1 << 3)
#endif

#ifndef bit_AVX2
#define bit_AVX2	(1 << 5)
#endif

#ifndef bit_BMI2
#define bit_BMI2	(1 << 8)
#endif

#ifndef bit_ADX
#define bit_ADX	(1 << 19)
#endif

#ifndef bit_SSE4_1
#define bit_SSE4_1	(1 << 19)
#endif

#ifndef bit_SSE4_2
#define bit_SSE4_2	(1 << 20)
#endif

int main()
{
	printf("Program compiled with: %s\n",__VERSION__);
	unsigned int eax, ebx, ecx, edx;

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
	return 0;
}
