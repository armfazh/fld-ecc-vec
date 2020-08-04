#include <faz_hash_avx2.h>
#include <prng.h>
#include <string.h>

#include "clocks.h"

void bench_hash25519() {
  printf("=== Benchmarking Hash ====\n");
  printf("===== Hash25519 AVX2 =====\n");

  long int BENCH = 300;
  const int size_msg = 64;
  uint8_t message[size_msg];
  prgn_random_bytes(message, size_msg);

  Point P;
  Fp25519._1w_full.arith.misc.zero(P.X);
  Fp25519._1w_full.arith.misc.zero(P.Y);

  EltFp25519_1w_fullradix u0, u1;
  PointXYZT_1way_full Q, Q0, Q1;
  PointXYZT_2way Q0Q1;
  EltFp25519_2w_redradix u0u1;

  CLOCKS_RANDOM(, addPoint, _1way_fulladd_1w_full(&Q, &Q0, &Q1));

CLOCKS_RANDOM(,h_x64,
    hash_to_field(u0, 0, message, size_msg);
    hash_to_field(u1, 1, message, size_msg));

EltFp25519_1w_fullradix uu0, uu1;
CLOCKS_RANDOM(,h_avx2,hash_to_field_2w(uu0, uu1, message, size_msg));

  CLOCKS_RANDOM(Fp25519._1w_full.arith.misc.rand(u0);
                Fp25519._1w_full.arith.misc.rand(u1), map_x64,
                map_to_curve(&Q0, u0);
                map_to_curve(&Q1, u1));
  CLOCKS_RANDOM(Fp25519._2w_red.arith.misc.rand(u0u1), map_avx2,
                map_to_curve_2w(&Q0Q1, u0u1));
  CLOCKS_RANDOM(prgn_random_bytes(message, size_msg), hash_x64,
                h2c25519_x64(&P, message, size_msg));
  CLOCKS_RANDOM(prgn_random_bytes(message, size_msg), hash_avx2,
                h2c25519_avx2(&P, message, size_msg));

  uint8_t *msg = NULL;
  int m, mlen;
  for (m = 0; m <= 21; m++) {
    BENCH = 300 - 12 * m;
    mlen = 1 << m;
    msg = (uint8_t *)malloc(mlen);
    printf("mlen: %-6d ", mlen);
    CLOCKS_RANDOM(prgn_random_bytes(msg, mlen), hash_x64,
                  h2c25519_x64(&P, msg, mlen));
    free(msg);
  }
  for (m = 0; m <= 21; m++) {
    BENCH = 300 - 12 * m;
    mlen = 1 << m;
    msg = (uint8_t *)malloc(mlen);
    printf("mlen: %-6d ", mlen);
    CLOCKS_RANDOM(prgn_random_bytes(msg, mlen), hash_avx2,
                  h2c25519_avx2(&P, msg, mlen));
    free(msg);
  }
}
