#include <faz_fp_avx2.h>
#include <stdio.h>

static void identity(const Arith_1w* fp) {
  argElement_1w a = fp->misc.alloc();
  argElement_1w b = fp->misc.alloc();
  argElement_1w add_ab = fp->misc.alloc();
  argElement_1w sub_ab = fp->misc.alloc();
  argElement_1w left = fp->misc.alloc();
  argElement_1w right = fp->misc.alloc();
  argElement_1w prime = fp->misc.alloc();

  printf("Testing the identity: (a+b)*(a-b) = (a^2-b^2)\n");
  fp->misc.rand(a);
  fp->misc.rand(b);

  printf("a: ");
  fp->misc.print(stdout, a);
  printf("b: ");
  fp->misc.print(stdout, b);

  fp->add(add_ab, a, b);
  fp->sub(sub_ab, a, b);
  fp->mul(left, add_ab, sub_ab);
  printf("left: ");
  fp->misc.print(stdout, left);

  fp->sqr(a);
  fp->sqr(b);
  fp->sub(right, a, b);
  printf("right: ");
  fp->misc.print(stdout, right);

  if (fp->cmp(left, right) == 0) {
    printf("Test passed.\n\n");
  } else {
    printf("Test failed.\n\n");
  }

  fp->misc.free(a);
  fp->misc.free(b);
  fp->misc.free(add_ab);
  fp->misc.free(sub_ab);
  fp->misc.free(left);
  fp->misc.free(right);
  fp->misc.free(prime);
}

int main(void) {
  identity(&Fp25519._1w_full.arith);
  identity(&Fp25519._1w_red.arith);
  return 0;
}
