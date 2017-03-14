
static void print_bytes(uint8_t * A, int num_bytes)
{
	int i;
	printf("0x");
	for(i=num_bytes-1;i>=0;i--)
	{
		printf("%02x", A[i]);
	}
	printf("\n");
}

static int compare_bytes(uint8_t* A, uint8_t* B,unsigned int num_bytes)
{
	unsigned int i=0;
	uint8_t ret=0;
	for(i=0;i<num_bytes;i++)
	{
		ret += A[i]^B[i];
	}
	return ret;
}

static void * allocate_bytes(size_t num_bytes)
{
	return _mm_malloc(num_bytes,ALIGN_BYTES);
}

static void deallocate_bytes(void * A)
{
	if(A != NULL)
	{
		_mm_free(A);
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
static void word64_multiplier(
		uint64_t*C,
		const uint64_t*A, const int numA,
		const uint64_t*B, const int numB)
{
	int i,j;
#define mul64x64(Z,X,Y)	__asm__ __volatile__ (\
			"movq 0(%3), %%rax     ;"\
			"mulq 0(%4)            ;"\
			"addq %%rax, %0        ;"\
			"adcq %%rdx, %1        ;"\
			"adcq $0x0,  %2        ;"\
	:/* out  */ "+r" ((Z)[0]),"+r" ((Z)[1]),"+r" ((Z)[2])\
	:/* in   */ "r" (X),"r" (Y)\
	:/* regs */ "memory","cc","%rax","%rdx");

	for(i=0;i<numA;i++)
	{
		for(j=0;j<numB;j++)
		{
			mul64x64(C+i+j,A+i,B+j);
		}
	}
#undef mul64x64
}


