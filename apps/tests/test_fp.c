static void test_Element_1w(const struct _struct_Fp_1way * fp)
{
	int64_t i;
	int64_t cnt =0 , match=0;
	argElement_1w a = fp->new();
	argElement_1w b = fp->new();
	argElement_1w c = fp->new();
	argElement_1w d = fp->new();
	argElement_1w e = fp->new();
	argElement_1w f = fp->new();

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

	fp->clean(a);
	fp->clean(b);
	fp->clean(c);
	fp->clean(d);
	fp->clean(e);
	fp->clean(f);
}


static void test_Element_Nw(const struct _struct_Fp_Nway * fp)
{
	int64_t i;
	int64_t cnt =0 , match=0;

	argElement_Nw a = fp->new();
	argElement_Nw b = fp->new();
	argElement_Nw c = fp->new();
	argElement_Nw d = fp->new();
	argElement_Nw e = fp->new();

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
	fp->clean(a);
	fp->clean(b);
	fp->clean(c);
	fp->clean(d);
	fp->clean(e);
}

static void test_fp(const struct _struct_Fp_Arith * field)
{
	printf("===== 1-way AVX2 =====\n");
	test_Element_1w(&field->_1way);
	printf("===== 2-way AVX2 =====\n");
	test_Element_Nw(&field->_2way);
	printf("===== 4-way AVX2 =====\n");
	test_Element_Nw(&field->_4way);
}

