#include <faz_hash_avx2.h>
#include <prng.h>
#include <string.h>

#include "clocks.h"

void bench_hash25519() {
  printf("=== Benchmarking Hash ====\n");
  printf("===== Hash25519 AVX2 =====\n");

  const long int BENCH = 300;
  uint8_t message[22];
  memcpy(message, (uint8_t *)"Keep Calm and Carry On", 22);
  const size_t size_msg = strlen((const char *)message);
  Point P;

  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);

  EltFp25519_1w_fullradix u0, u1;
  PointXYZT_1way_full Q0, Q1;
  PointXYZT_2way Q0Q1;
  EltFp25519_2w_redradix u0u1;

  CLOCKS_RANDOM(Fp25519._1w_full.arith.misc.rand(u0);
                Fp25519._1w_full.arith.misc.rand(u1), map_x64,
                map_to_curve(&Q0, u0);
                map_to_curve(&Q1, u1););
  CLOCKS_RANDOM(Fp25519._2w_red.arith.misc.rand(u0u1), map_avx2,
                map_to_curve_2w(&Q0Q1, u0u1););
  CLOCKS_RANDOM(prgn_random_bytes(message, size_msg), hash_x64,
                h2c25519_x64(&P, message, size_msg));
  CLOCKS_RANDOM(prgn_random_bytes(message, size_msg), hash_avx2,
                h2c25519_avx2(&P, message, size_msg));
}
