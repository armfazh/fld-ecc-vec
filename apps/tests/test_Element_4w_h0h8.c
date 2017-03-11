#include "element_4w_h0h8.h"

static void test_Element_4w_h0h8()
{
	int64_t i;
	int64_t cnt =0 , match=0;
	Element_4w_H0H8 a,b,c,e,f;

	printf("===== 4-way AVX2 =====\n");
	/**
	* Test identity:
	*    (a^2-b^2) == (a+b)(a-b)
	*/
	printf("Test mul/sqr:\n");
	cnt = 0;
	for(i=0;i<TEST_TIMES;i++)
	{
		random_Element_4w_h0h8(a);
		random_Element_4w_h0h8(b);

		add_Element_4w_h0h8(e,a,b);
		sub_Element_4w_h0h8(f,a,b);		compress_Element_4w_h0h8(f);

		mul_Element_4w_h0h8(e,e,f);		compress_Element_4w_h0h8(e);

		sqr_Element_4w_h0h8(a);		compress_Element_4w_h0h8(a);
		sqr_Element_4w_h0h8(b);		compress_Element_4w_h0h8(b);
		sub_Element_4w_h0h8(c,a,b);

		match = compare_Element_4w_h0h8(c,e) == 0;
		if(!match)
		{
			printf("A:\n");print_Element_4w_h0h8(a);
			printf("B:\n");print_Element_4w_h0h8(b);
			break;
		}
		cnt += match;
	}
	printf(" %ld %s\n",cnt , cnt == TEST_TIMES? "OK" : "FAIL" );
}

