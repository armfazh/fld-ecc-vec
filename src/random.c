#include <stdlib.h>
#include <stdint.h>

/**
 * [TODO]:  Warning!
 * Provide a secure random number generator.
 **/
void random_bytes(uint8_t *A, int length)
{
	int i;
	for(i=0; i<length; i++)
	{
		A[i] = (uint8_t) rand();
	}
}
