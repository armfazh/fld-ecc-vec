#ifndef _CLOCKS_H_
#define _CLOCKS_H_

#include<stdint.h>

#ifdef __INTEL_COMPILER
#define BARRIER __memory_barrier()
#else
#define BARRIER asm volatile("" ::: "memory")
#endif

#define CLOCKS(F) \
do{ \
	uint64_t start,end;int64_t i_bench,j_bench; \
	unsigned cycles_high0,cycles_low0; \
	unsigned cycles_high1,cycles_low1;  \
	asm volatile (\
	"mfence\n\t" \
	"RDTSC\n\t" \
	"mov %%edx, %0\n\t" \
	"mov %%eax, %1\n\t": "=r" (cycles_high0), "=r" (cycles_low0)::  \
	"%rax", "%rbx", "%rcx", "%rdx"); \
	BARRIER;\
	i_bench=BENCH;\
	do{\
		j_bench=BENCH;\
		do{\
			F;\
			j_bench--;\
		}while( j_bench != 0 );\
		i_bench--;\
	}while( i_bench != 0 );\
	BARRIER;\
	asm volatile("RDTSCP\n\t"  \
	"mov %%edx, %0\n\t"   \
	"mov %%eax, %1\n\t"   \
	"mfence\n\t" \
	: "=r" (cycles_high1), "=r" (cycles_low1):: \
	"%rax", "%rbx", "%rcx", "%rdx"); \
	start= (((uint64_t)cycles_high0)<<32)|cycles_low0;\
	end  = (((uint64_t)cycles_high1)<<32)|cycles_low1;\
	printf("%s: %lu cc\n",#F,(end-start)/(BENCH*BENCH));\
}while(0)

#include <sys/time.h>
#include <time.h>



static uint64_t
time_now() {
	struct timeval tv;
	uint64_t ret;

	gettimeofday(&tv, NULL);
	ret = tv.tv_sec;
	ret *= 1000000;
	ret += tv.tv_usec;

	return ret;
}

/**
 * Taken from
 * agl/curve25519-donna
 * https://github.com/agl/curve25519-donna/blob/master/speed-curve25519.c
 */

/* ticks - not tested on anything other than x86 */
static uint64_t
cycles_now(void) {
#if defined(__GNUC__)
	uint32_t lo, hi;
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)lo | ((uint64_t)hi << 32));
#else
		return 0;	/* Undefined for now; should be obvious in the output */
        #endif
}

#define oper_second(FUNC) \
do{\
	printf("Operations per second: Curve25519.\n");\
	randomStrBytes(r);\
\
	unsigned i;\
	uint64_t start, end;\
	const unsigned iterations = 100000;\
	uint64_t start_c, end_c;\
\
	/* Load the caches*/\
	for (i = 0; i < 1000; ++i) {\
	     FUNC ;\
	}\
\
	start = time_now();\
	start_c = cycles_now();\
	for (i = 0; i < iterations; ++i) {\
		FUNC; \
	}\
	end = time_now();\
	end_c = cycles_now();\
\
	printf("%lu us, %g op/s, %lu cycles/op\n",\
			(unsigned long) ((end - start) / iterations),\
			iterations*1000000. / (end - start),\
			(unsigned long)((end_c-start_c)/iterations) );\
}while(0)

/* _CLOCKS_H_ */
#endif
