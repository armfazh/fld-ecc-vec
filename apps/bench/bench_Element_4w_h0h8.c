#include <element_4w_h0h8.h>

static void bench_element_4w_h0h8()
{
	long int BENCH = 3000;
	Element_4w_H0H8 a,b,c;
	printf("======  4-way AVX2  ======\n");

	random_Element_4w_h0h8(a);
	random_Element_4w_h0h8(b);
	random_Element_4w_h0h8(c);

	CLOCKS(addition,       add_Element_4w_h0h8(c,a,b));
	CLOCKS(subtraction,    sub_Element_4w_h0h8(c,c,b));
	CLOCKS(multiplication, mul_Element_4w_h0h8(c,a,b));
	CLOCKS(squaring,       sqr_Element_4w_h0h8(c));
	CLOCKS(coef_reduction, compress_Element_4w_h0h8(c));
}
