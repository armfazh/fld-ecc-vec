#include <faz_fp_avx2.h>

static void bench_Element_1w(const struct _struct_Fp_1way * fp)
{
	long int BENCH = 1000;
	argElement_1w a = fp->init();
	argElement_1w b = fp->init();
	argElement_1w c = fp->init();

	fp->rand(a);
	fp->rand(b);
	fp->rand(c);

	CLOCKS(addition,       fp->add(c,a,b));
	CLOCKS(subtraction,    fp->sub(c,c,b));
	CLOCKS(multiplication, fp->mul(c,a,b));
	CLOCKS(squaring,       fp->sqr(c));
	CLOCKS(coef_reduction, fp->cred(c));
	BENCH = BENCH/10;
	CLOCKS(inversion,      fp->inv(c,a));
	CLOCKS(square_root,    fp->sqrt(c,a));

	fp->clear(a);
	fp->clear(b);
	fp->clear(c);
}

static void bench_Element_Nw(const struct _struct_Fp_Nway * fp)
{
	long int BENCH = 1000;
	argElement_Nw a = fp->init();
	argElement_Nw b = fp->init();
	argElement_Nw c = fp->init();

	fp->rand(a);
	fp->rand(b);
	fp->rand(c);

	CLOCKS(addition,       fp->add(c,a,b));
	CLOCKS(subtraction,    fp->sub(c,c,b));
	CLOCKS(multiplication, fp->mul(c,a,b));
	CLOCKS(squaring,       fp->sqr(c));
	CLOCKS(coef_reduction, fp->cred(c));

	fp->clear(a);
	fp->clear(b);
	fp->clear(c);
}

static void bench_fp25519()
{
	printf("======  1-way AVX2  ======\n");
	bench_Element_1w(&Fp.fp25519._1way);
	printf("======  1-way x64  ======\n");
	bench_Element_1w(&Fp.fp25519._1way_x64);
	printf("======  2-way AVX2  ======\n");
	bench_Element_Nw(&Fp.fp25519._2way);
	printf("======  4-way AVX2  ======\n");
	bench_Element_Nw(&Fp.fp25519._4way);
}

static void bench_fp448()
{
	printf("======  1-way AVX2  ======\n");
	bench_Element_1w(&Fp.fp448._1way);
	printf("======  2-way AVX2  ======\n");
	bench_Element_Nw(&Fp.fp448._2way);
	printf("======  4-way AVX2  ======\n");
	bench_Element_Nw(&Fp.fp448._4way);
}
