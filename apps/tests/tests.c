#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random.h>

#define TEST_TIMES 50000

#include "test_fp.c"
#include "test_ecdh.c"
#include "test_eddsa.c"

int main(void)
{
	srand((unsigned int)time(NULL));
	printf("==========================================================\n");
	printf("  High Performance Implementation of the Edwards Digital  \n");
	printf("       Signature Algorithm using Vector Instructions      \n");
	printf("==========================================================\n\n");
	printf("===== Testing Ed25519 =====\n");
	
//	test_fp25519();
//	test_x25519();
//	test_ed25519();

	printf("===== Testing Ed448 =====\n");
	test_fp448();
	test_x448();
	test_ed448();

	return 0;
}
