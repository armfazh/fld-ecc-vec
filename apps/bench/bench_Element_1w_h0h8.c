#include <element_1w_h0h8.h>

static void bench_element_1w_h0h8()
{
	long int BENCH = 3000;
	Element_1w_H0H8 a,b,c;

	printf("======  1-way AVX2  ======\n");

	random_Element_1w_h0h8(a);
	random_Element_1w_h0h8(b);
	random_Element_1w_h0h8(c);
	CLOCKS(addition,       add_Element_1w_h0h8(c,a,b));
	CLOCKS(subtraction,    sub_Element_1w_h0h8(c,c,b));
	CLOCKS(multiplication, mul_Element_1w_h0h8(c,a,b));
	CLOCKS(squaring,       sqr_Element_1w_h0h8(c));
	CLOCKS(coef_reduction, compress_Element_1w_h0h8(c));

	BENCH = BENCH/10;
	CLOCKS(inversion,invsqrt_Element_1w_h0h8(c,a,1));
	CLOCKS(square_root,invsqrt_Element_1w_h0h8(c,a,0));
}
