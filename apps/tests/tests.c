#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random.h>

#include <fp.h>
#include <eddsa_avx2.h>

#define TEST_TIMES 50000

#include "test_fp.c"
#include "test_x448.c"
#include "test_ed448.c"

int main(void)
{
	srand((unsigned int)time(NULL));
	printf("==========================================================\n");
	printf("  High Performance Implementation of the Edwards Digital  \n");
	printf("       Signature Algorithm using Vector Instructions      \n");
	printf("==========================================================\n\n");
	printf("===== Testing =====\n");

	test_fp(&Fp.fp448);
	test_eddsa();
	test_x448();

	return 0;
}
