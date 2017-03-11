#include <element_2w_h0h8.h>

static void bench_element_2w_h0h8()
{
	long int BENCH = 3000;
	Element_2w_H0H8 a,b,c;
	printf("======  2-way AVX2  ======\n");

	random_Element_2w_h0h8(a);
	random_Element_2w_h0h8(b);
	random_Element_2w_h0h8(c);

	CLOCKS(addition,       add_Element_2w_h0h8(c,a,b));
	CLOCKS(subtraction,    sub_Element_2w_h0h8(c,c,b));
	CLOCKS(multiplication, mul_Element_2w_h0h8(c,a,b));
	CLOCKS(squaring,       sqr_Element_2w_h0h8(c));
	CLOCKS(coef_reduction, compress_Element_2w_h0h8(c));
}
