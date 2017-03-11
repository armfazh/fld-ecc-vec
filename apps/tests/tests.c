#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random.h>

#define TEST_TIMES 50000

#include "test_Element_1w_h0h8.c"
#include "test_Element_2w_h0h8.c"
#include "test_Element_4w_h0h8.c"
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

	test_Element_1w_h0h8();
	test_Element_2w_h0h8();
	test_Element_4w_h0h8();
	test_eddsa();
	test_x448();

	return 0;
}
