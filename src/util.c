/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of eddsa_avx2.
 *
 * eddsa_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eddsa_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eddsa_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>

/**
 *
 * @param buffer
 * @param num_bytes
 */
void print_bytes(uint8_t * buffer, int num_bytes)
{
	int i;
	printf("0x");
	for(i=num_bytes-1;i>=0;i--)
	{
		printf("%02x", buffer[i]);
	}
	printf("\n");
}

/**
 *
 * @param A
 * @param B
 * @param num_bytes
 * @return
 */
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

 /** Random number Generator:
  * Taken from: https://github.com/relic-toolkit/relic/src/rand/relic_rand_call.c
  *
  * @warning Provide a secure random number generator.
  * @param buffer
  * @param num_bytes
  */
#include <unistd.h>
#include <fcntl.h>
void random_bytes(uint8_t *buffer, int num_bytes)
{
    int c, l, fd = open("/dev/urandom", O_RDONLY);

    if (fd == -1)
    {
        printf("Error opening /dev/urandom\n");
    }

    l = 0;
    do {
        c = read(fd, buffer + l, num_bytes - l);
        l += c;
        if (c == -1)
        {
            printf("Error reading /dev/urandom\n");
        }
    } while (l < num_bytes);

    close(fd);
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


volatile void *spc_memset(volatile void *dst, int c, size_t len) {
	volatile char *buf;

	for (buf = (volatile char *)dst;  len;  buf[--len] = c);
	return dst;
}
