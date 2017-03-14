#include <fp.h>

static void bench_Element_1w(const struct _struct_Fp_1way * fp)
{
	long int BENCH = 3000;
	argElement_1w a = fp->new();
	argElement_1w b = fp->new();
	argElement_1w c = fp->new();

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

	fp->clean(a);
	fp->clean(b);
	fp->clean(c);
}

static void bench_Element_Nw(const struct _struct_Fp_Nway * fp)
{
	long int BENCH = 3000;
	argElement_Nw a = fp->new();
	argElement_Nw b = fp->new();
	argElement_Nw c = fp->new();

	fp->rand(a);
	fp->rand(b);
	fp->rand(c);

	CLOCKS(addition,       fp->add(c,a,b));
	CLOCKS(subtraction,    fp->sub(c,c,b));
	CLOCKS(multiplication, fp->mul(c,a,b));
	CLOCKS(squaring,       fp->sqr(c));
	CLOCKS(coef_reduction, fp->cred(c));

	fp->clean(a);
	fp->clean(b);
	fp->clean(c);
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
