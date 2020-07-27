#include <faz_hash_avx2.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  uint8_t *message = (uint8_t *)"Keep Calm and Carry On";
  const size_t size_msg = strlen((const char *)message);
  Point P;

  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);
  printf("m(%d): %s\n", (int)size_msg, message);

  h2c25519_x64(&P, (uint8_t *)message, size_msg);
  printf("H(m): in E (%s)\n", isOnCurve(&P) ? "Yes" : "No");
  print_point(stdout, &P);

  h2c25519_avx2(&P, (uint8_t *)message, size_msg);
  printf("H(m): in E (%s)\n", isOnCurve(&P) ? "Yes" : "No");
  print_point(stdout, &P);

  return 0;
}
