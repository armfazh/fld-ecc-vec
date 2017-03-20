#include "clocks.h"
#include <random.h>
#include <time.h>
#include <stdio.h>

#include "bench_fp.c"
#include "bench_ecc.c"
#include "bench_ecdh.c"
#include "bench_eddsa.c"

int main(void)
{
	srand((unsigned int)time(NULL));
	printf("==========================================================\n");
	printf("  High Performance Implementation of the Edwards Digital  \n");
	printf("       Signature Algorithm using Vector Instructions      \n");
	printf("==========================================================\n\n");

	printf("===== Benchmarking Ed25519 =====\n");
	bench_fp25519();
	bench_x25519();
	bench_ed25519();

	printf("===== Benchmarking Ed448 =====\n");
	bench_fp448();
	bench_ecc();
	bench_x448();
	bench_ed448();

	return 0;
}


