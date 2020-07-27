#include <faz_fp_avx2.h>
#include "clocks.h"

static void bench_1w(const Arith_1w *fp) {
  long int BENCH = 1000;
  argElement_1w a = fp->misc.alloc();
  argElement_1w b = fp->misc.alloc();
  argElement_1w c = fp->misc.alloc();

  fp->misc.rand(a);
  fp->misc.rand(b);
  fp->misc.rand(c);

  CLOCKS(addition, fp->add(c, a, b));
  CLOCKS(subtraction, fp->sub(c, c, b));
  CLOCKS(multiplication, fp->mul(c, a, b));
  CLOCKS(squaring, fp->sqr(c));
  BENCH = BENCH / 10;
  CLOCKS(inversion, fp->inv(c, a));

  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
}

static void bench_2w(const Arith_2w *fp) {
  long int BENCH = 1000;
  argElement_2w a = fp->misc.alloc();
  argElement_2w b = fp->misc.alloc();
  argElement_2w c = fp->misc.alloc();

  fp->misc.rand(a);
  fp->misc.rand(b);
  fp->misc.rand(c);

  CLOCKS(addition, fp->add(c, a, b));
  CLOCKS(subtraction, fp->sub(c, c, b));
  CLOCKS(multiplication, fp->mul(c, a, b));
  CLOCKS(squaring, fp->sqr(c));

  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
}

static void bench_4w(const Arith_4w *fp) {
  long int BENCH = 1000;
  argElement_2w a = fp->misc.alloc();
  argElement_2w b = fp->misc.alloc();
  argElement_2w c = fp->misc.alloc();

  fp->misc.rand(a);
  fp->misc.rand(b);
  fp->misc.rand(c);

  CLOCKS(addition, fp->add(c, a, b));
  CLOCKS(subtraction, fp->sub(c, c, b));
  CLOCKS(multiplication, fp->mul(c, a, b));
  CLOCKS(squaring, fp->sqr(c));

  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
}

void bench_fp25519() {
  printf("=== Benchmarking Arith ===\n");
  printf("======  p=2^255-19  ======\n");
  printf("====== 1-way (full) ======\n");
  bench_1w(&Fp25519._1w_full.arith);
  printf("====== 1-way (red)  ======\n");
  bench_1w(&Fp25519._1w_red.arith);
  printf("====== 2-way (red)  ======\n");
  bench_2w(&Fp25519._2w_red.arith);
  printf("====== 4-way (red)  ======\n");
  bench_4w(&Fp25519._4w_red.arith);
}

void bench_fp448() {
  printf("=== Benchmarking Arith ===\n");
  printf("==== p=2^448-2^224-1 =====\n");
  printf("====== 1-way (full) ======\n");
  bench_1w(&Fp448._1w_full.arith);
  printf("====== 1-way (red)  ======\n");
  bench_1w(&Fp448._1w_red.arith);
  printf("====== 2-way (red)  ======\n");
  bench_2w(&Fp448._2w_red.arith);
  printf("====== 4-way (red)  ======\n");
  bench_4w(&Fp448._4w_red.arith);
}
