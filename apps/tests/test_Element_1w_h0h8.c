#include "element_1w_h0h8.h"

static void test_Element_1w_h0h8()
{
	int64_t i;
	int64_t cnt =0 , match=0;
	Element_1w_H0H8 a,b,c,d,e,f;

	printf("===== 1-way AVX2 =====\n");
	/**
	* Test identity:
	*    (a^2-b^2) == (a+b)(a-b)
	*/
	printf("Test mul/sqr:\n");
	cnt = 0;
	for(i=0;i<TEST_TIMES;i++)
	{
		random_Element_1w_h0h8(a);
		random_Element_1w_h0h8(b);

		add_Element_1w_h0h8(e,a,b);
		sub_Element_1w_h0h8(f,a,b);compress_Element_1w_h0h8(f);
		mul_Element_1w_h0h8(e,e,f);
		compress_Element_1w_h0h8(e);

		sqr_Element_1w_h0h8(a);compress_Element_1w_h0h8(a);
		sqr_Element_1w_h0h8(b);compress_Element_1w_h0h8(b);
		sub_Element_1w_h0h8(c, a, b); compress_Element_1w_h0h8(c);

		match = compare_Element_1w_h0h8(c,e)==0;
		if(!match)
		{
			printf("A:\n");print_Element_1w_h0h8(a);
			printf("B:\n");print_Element_1w_h0h8(b);
			break;
		}
		cnt += match;
	}
	printf(" %ld %s\n",cnt , cnt == TEST_TIMES? "OK" : "FAIL" );

	/**
	* Test identity:
	*    (a*b*a^1) == b
	*/
	printf("Test mul/inv:\n");
	cnt = 0;
	for(i=0;i<TEST_TIMES;i++)
	{
		random_Element_1w_h0h8(a);
		random_Element_1w_h0h8(b);

        invsqrt_Element_1w_h0h8(d,a,1);
        mul_Element_1w_h0h8(a,a,b);
        compress_Element_1w_h0h8(a);
		mul_Element_1w_h0h8(a,a,d);
		compress_Element_1w_h0h8(a);

		match = compare_Element_1w_h0h8(a,b)==0;
		if(!match)
		{
			printf("A:\n");print_Element_1w_h0h8(a);
			printf("B:\n");print_Element_1w_h0h8(b);
			break;
		}
		cnt += match;
	}
	printf(" %ld %s\n",cnt , cnt == TEST_TIMES? "OK" : "FAIL" );
}

