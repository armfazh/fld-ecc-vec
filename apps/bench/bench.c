#include "clocks.h"
#include <random.h>
#include <time.h>
#include <stdio.h>

#include "bench_Element_1w_h0h8.c"
#include "bench_Element_2w_h0h8.c"
#include "bench_Element_4w_h0h8.c"
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

	bench_element_1w_h0h8();
	bench_element_2w_h0h8();
	bench_element_4w_h0h8();
	bench_ecc();
	bench_x448();
	bench_ed448();

	return 0;
}


