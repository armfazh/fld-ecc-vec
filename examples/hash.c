#include <faz_hash_avx2.h>
// #include <stdio.h>
#include <string.h>

int main(void) {
  uint8_t *message = (uint8_t *)"Keep Calm and Carry On";
  const size_t size_msg = strlen((const char *)message);
  Point255 P;

  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);
  Fp25519._1w_full.arith.misc.zero(P.T);
  Fp25519._1w_full.arith.misc.zero(P.Z);
  printf("m: %s\n", message);

  h2c25519(&P, (uint8_t *)message, size_msg);

  printf("H(m):\n");
  print_point(stdout, &P);

  return 0;
}
