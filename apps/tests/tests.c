#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEST_TIMES 50000
#define LABEL "Test passed? [%s]\n"
#define OK "Ok"
#define ERROR "error"

#include "cpu_id.c"
#include "test_fp.c"
#include "test_ecdh.c"
#include "test_eddsa.c"

int main(void)
{
	printf("==========================================================\n");
	printf("  High Performance Implementation of the Edwards Digital  \n");
	printf("       Signature Algorithm using Vector Instructions      \n");
	printf("==========================================================\n\n");

	printf("=== Environment Information ====\n");
	machine_info();

	printf("===== Testing Ed25519 =====\n");
	test_fp25519();
	test_x25519();
	test_ed25519();

	printf("===== Testing Ed448 =====\n");
	test_fp448();
	test_x448();
	test_ed448();

	return 0;
}
