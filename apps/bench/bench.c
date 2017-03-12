#include "clocks.h"
#include <random.h>
#include <time.h>
#include <stdio.h>

#include <fp.h>
#include <eddsa_avx2.h>

#include "bench_fp.c"
#include "bench_ecc.c"
#include "bench_x448.c"
#include "bench_ed448.c"

int main(void)
{
	srand((unsigned int)time(NULL));
	printf("==========================================================\n");
	printf("  High Performance Implementation of the Edwards Digital  \n");
	printf("       Signature Algorithm using Vector Instructions      \n");
	printf("==========================================================\n\n");
	printf("===== Benchmarking Ed448 =====\n");

	bench_fp(&Fp.fp448);
	bench_ecc();
	bench_x448();
	bench_ed448();

	return 0;
}


