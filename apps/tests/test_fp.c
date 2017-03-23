#include <faz_fp_avx2.h>

static void test_Element_1w(const struct _struct_Fp_1way * fp)
{
	int64_t i;
	int64_t cnt =0 , match=0;
	argElement_1w a = fp->init();
	argElement_1w b = fp->init();
	argElement_1w c = fp->init();
	argElement_1w d = fp->init();
	argElement_1w e = fp->init();
	argElement_1w f = fp->init();

	/**
	* Test identity:
	*    (a^2-b^2) == (a+b)(a-b)
	*/
	printf("Test mul/sqr:\n");
	cnt = 0;
	for(i=0;i<TEST_TIMES;i++)
	{
		fp->rand(a);
		fp->rand(b);

		fp->add(e,a,b);
		fp->sub(f,a,b);fp->cred(f);
		fp->mul(e,e,f);
		fp->cred(e);

		fp->sqr(a);fp->cred(a);
		fp->sqr(b);fp->cred(b);
		fp->sub(c, a, b); fp->cred(c);

		match = fp->cmp(c,e)==0;
		if(!match)
		{
			printf("A:\n");fp->print(a);
			printf("B:\n");fp->print(b);
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
		fp->rand(a);
		fp->rand(b);

		fp->inv(d,a);
		fp->mul(a,a,b);
		fp->cred(a);
		fp->mul(a,a,d);
		fp->cred(a);

		match = fp->cmp(a,b)==0;
		if(!match)
		{
			printf("A:\n");fp->print(a);
			printf("B:\n");fp->print(b);
			break;
		}
		cnt += match;
	}
	printf(" %ld %s\n",cnt , cnt == TEST_TIMES? "OK" : "FAIL" );

	fp->clear(a);
	fp->clear(b);
	fp->clear(c);
	fp->clear(d);
	fp->clear(e);
	fp->clear(f);
}


static void test_Element_Nw(const struct _struct_Fp_Nway * fp)
{
	int64_t i;
	int64_t cnt =0 , match=0;

	argElement_Nw a = fp->init();
	argElement_Nw b = fp->init();
	argElement_Nw c = fp->init();
	argElement_Nw d = fp->init();
	argElement_Nw e = fp->init();

	/**
	* Test identity:
	*    (a^2-b^2) == (a+b)(a-b)
	*/
	printf("Test mul/sqr:\n");
	cnt = 0;
	for(i=0;i<TEST_TIMES;i++)
	{
		fp->rand(a);
		fp->rand(b);

		fp->add(e,a,b);
		fp->sub(d,a,b);		fp->cred(d);

		fp->mul(e,e,d);		fp->cred(e);

		fp->sqr(a);		fp->cred(a);
		fp->sqr(b);		fp->cred(b);
		fp->sub(c, a, b);

		match = fp->cmp(c, e)==0;
		if(!match)
		{
			printf("A:\n");fp->print(a);
			printf("B:\n");fp->print(b);
			break;
		}
		cnt += match;
	}
	printf(" %ld %s\n",cnt , cnt == TEST_TIMES? "OK" : "FAIL" );
	fp->clear(a);
	fp->clear(b);
	fp->clear(c);
	fp->clear(d);
	fp->clear(e);
}

static void test_fp25519()
{
	printf("===== 1-way AVX2 =====\n");
	test_Element_1w(&Fp.fp25519._1way);
	printf("===== 1-way x64 =====\n");
	test_Element_1w(&Fp.fp25519._1way_x64);
	printf("===== 2-way AVX2 =====\n");
	test_Element_Nw(&Fp.fp25519._2way);
	printf("===== 4-way AVX2 =====\n");
	test_Element_Nw(&Fp.fp25519._4way);
}

static void test_fp448()
{
	printf("===== 1-way AVX2 =====\n");
	test_Element_1w(&Fp.fp448._1way);
	printf("===== 2-way AVX2 =====\n");
	test_Element_Nw(&Fp.fp448._2way);
	printf("===== 4-way AVX2 =====\n");
	test_Element_Nw(&Fp.fp448._4way);
}

