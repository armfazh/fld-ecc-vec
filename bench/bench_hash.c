#include <faz_hash_avx2.h>
#include <string.h>

#include "clocks.h"

void bench_h2c25519() {
  printf("=== Benchmarking Hash ====\n");
  printf("===== Hash25519 AVX2 =====\n");

  uint8_t *message = (uint8_t *)"Keep Calm and Carry On";
  const size_t size_msg = strlen((const char *)message);
  PointXYZT_1way_full P;

  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);
  Fp25519._1w_full.arith.misc.zero(P.T);
  Fp25519._1w_full.arith.misc.zero(P.Z);

  long int BENCH = 300;
  CLOCKS(hash, h2c25519_x64(&P, (uint8_t *)message, size_msg));
  CLOCKS(hash, h2c25519_avx2(&P, (uint8_t *)message, size_msg));
}
