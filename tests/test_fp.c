#include <stdio.h>
#include <faz_fp_avx2.h>

#define TEST_TIMES 50000

static void test_1w(const Arith_1w *fp) {
  int64_t i;
  int64_t cnt = 0, match = 0;
  argElement_1w a = fp->misc.alloc();
  argElement_1w b = fp->misc.alloc();
  argElement_1w c = fp->misc.alloc();
  argElement_1w d = fp->misc.alloc();
  argElement_1w e = fp->misc.alloc();
  argElement_1w f = fp->misc.alloc();

  /**
  * Test identity:
  *    (a^2-b^2) == (a+b)(a-b)
  */
  printf("Test mul/sqr:");
  cnt = 0;
  for (i = 0; i < TEST_TIMES; i++) {
    fp->misc.rand(a);
    fp->misc.rand(b);

    fp->add(e, a, b);
    fp->sub(f, a, b);
    fp->mul(e, e, f);

    fp->sqr(a);
    fp->sqr(b);
    fp->sub(c, a, b);

    match = fp->misc.cmp(c, e) == 0;
    if (!match) {
      fprintf(stderr,"A:\n");
      fp->misc.print(stderr, a);
      fprintf(stderr,"B:\n");
      fp->misc.print(stderr, b);
      break;
    }
    cnt += match;
  }
  printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");

  /**
  * Test identity:
  *    (a*b*a^1) == b
  */
  printf("Test mul/inv:");
  cnt = 0;
  for (i = 0; i < TEST_TIMES; i++) {
    fp->misc.rand(a);
    fp->misc.rand(b);

    fp->inv(d, a);
    fp->mul(a, a, b);
    fp->mul(a, a, d);

    match = fp->misc.cmp(a, b) == 0;
    if (!match) {
      fprintf(stderr,"A:\n");
      fp->misc.print(stderr, a);
      fprintf(stderr,"B:\n");
      fp->misc.print(stderr, b);
      break;
    }
    cnt += match;
  }
  printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");

  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
  fp->misc.free(d);
  fp->misc.free(e);
  fp->misc.free(f);
}

static void test_2w(const Arith_2w *fp) {
  int64_t i;
  int64_t cnt = 0, match = 0;

  argElement_2w a = fp->misc.alloc();
  argElement_2w b = fp->misc.alloc();
  argElement_2w c = fp->misc.alloc();
  argElement_2w d = fp->misc.alloc();
  argElement_2w e = fp->misc.alloc();

  /**
  * Test identity:
  *    (a^2-b^2) == (a+b)(a-b)
  */
  printf("Test mul/sqr:");
  cnt = 0;
  for (i = 0; i < TEST_TIMES; i++) {
    fp->misc.rand(a);
    fp->misc.rand(b);

    fp->add(e, a, b);
    fp->sub(d, a, b);
    fp->mul(e, e, d);

    fp->sqr(a);
    fp->sqr(b);
    fp->sub(c, a, b);

    match = fp->misc.cmp(c, e) == 0;
    if (!match) {
      printf("A:\n");
      fp->misc.print(stderr, a);
      printf("B:\n");
      fp->misc.print(stderr, b);
      break;
    }
    cnt += match;
  }
  printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");
  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
  fp->misc.free(d);
  fp->misc.free(e);
}

static void test_4w(const Arith_4w *fp) {
  int64_t i;
  int64_t cnt = 0, match = 0;

  argElement_2w a = fp->misc.alloc();
  argElement_2w b = fp->misc.alloc();
  argElement_2w c = fp->misc.alloc();
  argElement_2w d = fp->misc.alloc();
  argElement_2w e = fp->misc.alloc();

  /**
  * Test identity:
  *    (a^2-b^2) == (a+b)(a-b)
  */
  printf("Test mul/sqr:");
  cnt = 0;
  for (i = 0; i < TEST_TIMES; i++) {
    fp->misc.rand(a);
    fp->misc.rand(b);

    fp->add(e, a, b);
    fp->sub(d, a, b);
    fp->mul(e, e, d);

    fp->sqr(a);
    fp->sqr(b);
    fp->sub(c, a, b);

    match = fp->misc.cmp(c, e) == 0;
    if (!match) {
      printf("A:\n");
      fp->misc.print(stderr, a);
      printf("B:\n");
      fp->misc.print(stderr, b);
      break;
    }
    cnt += match;
  }
  printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");
  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(c);
  fp->misc.free(d);
  fp->misc.free(e);
}

void test_fp25519() {
  printf("=== Testing Arith ====\n");
  printf("===   p=2^255-19  ====\n");
  printf("=== 1-way (full)  ====\n");
  test_1w(&Fp25519._1w_full.arith);
  printf("=== 1-way (red)   ====\n");
  test_1w(&Fp25519._1w_red.arith);
  printf("=== 2-way (red)   ====\n");
  test_2w(&Fp25519._2w_red.arith);
  printf("=== 4-way (red)   ====\n");
  test_4w(&Fp25519._4w_red.arith);
}

void test_fp448() {
  printf("=== Testing Arith  ===\n");
  printf("=== p=2^448-2^224-1 ==\n");
  printf("=== 1-way (full)  ====\n");
  test_1w(&Fp448._1w_full.arith);
  printf("=== 1-way (red)   ====\n");
  test_1w(&Fp448._1w_red.arith);
  printf("=== 2-way (red)   ====\n");
  test_2w(&Fp448._2w_red.arith);
  printf("=== 4-way (red)   ====\n");
  test_4w(&Fp448._4w_red.arith);
}
