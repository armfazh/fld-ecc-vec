#include <faz_hash_avx2.h>
#include <string.h>

#include "clocks.h"

void bench_h2c25519() {
  printf("=== Benchmarking Hash ====\n");
  printf("===== Hash25519 AVX2 =====\n");

  const long int BENCH = 300;
  uint8_t *message = (uint8_t *)"Keep Calm and Carry On";
  const size_t size_msg = strlen((const char *)message);
  PointXYZT_1way_full P;

  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);
  Fp25519._1w_full.arith.misc.zero(P.T);
  Fp25519._1w_full.arith.misc.zero(P.Z);

  EltFp25519_1w_fullradix u0, u1;
  PointXYZT_1way_full Q0, Q1;
  PointXYZT_2way Q0Q1;
  EltFp25519_2w_redradix u0u1;
  Fp25519._1w_full.arith.misc.rand(u0);
  Fp25519._1w_full.arith.misc.rand(u1);
  Fp25519._2w_red.arith.misc.rand(u0u1);

  CLOCKS(map_x64, map_to_curve(&Q0, u0); map_to_curve(&Q1, u1););
  CLOCKS(map_avx2, map_to_curve_2w(&Q0Q1, u0u1););
  CLOCKS(hash_x64, h2c25519_x64(&P, (uint8_t *)message, size_msg));
  CLOCKS(hash_avx2, h2c25519_avx2(&P, (uint8_t *)message, size_msg));
}
