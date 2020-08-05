#include <multi_hash.h>
#include <prng.h>
#include <sha3.h>
#include <sha512.h>
#include <stdio.h>

#include "clocks.h"

static void bench_sha512_2w() {
  const int BENCH = 500;
  const int NUM = 2;
  const int MSG_LEN = 64;
  int i_multi = 0;
  uint8_t *message[NUM];
  uint8_t *digest[NUM];
  for (i_multi = 0; i_multi < NUM; i_multi++) {
    message[i_multi] = (uint8_t *)_mm_malloc((MSG_LEN) + 1, ALIGN_BYTES);
    digest[i_multi] = (uint8_t *)_mm_malloc(64, ALIGN_BYTES);
    prgn_random_bytes(message[i_multi], MSG_LEN);
  }
  CLOCKS(SHA512_avx, sha512_2w_avx(message, MSG_LEN, digest));
  for (i_multi = 0; i_multi < NUM; i_multi++) {
    _mm_free(message[i_multi]);
    _mm_free(digest[i_multi]);
  }
}

void bench_third_party() {
  const int BENCH = 500;
  const int message_length = 64;
  uint8_t message[message_length];
  uint8_t digest_sha512[SPH_SIZE_sha512];
  uint8_t digest_sha3[128];

  printf("===== Hash Functions =====\n");
  printf("====== SHA512   ==========\n");
  sph_sha512_context mc;

  prgn_random_bytes(message, message_length);
  CLOCKS(sha512, {
    sph_sha512_init(&mc);
    sph_sha512(&mc, message, message_length);
    sph_sha512_close(&mc, digest_sha512);
  });
  CLOCKS(sha512_x64, {
    sph_sha512_init(&mc);
    sph_sha512(&mc, message, message_length);
    sph_sha512_close(&mc, digest_sha512);
    sph_sha512_init(&mc);
    sph_sha512(&mc, message, message_length);
    sph_sha512_close(&mc, digest_sha512);
  });

  bench_sha512_2w();
  printf("====== SHAKE256 ==========\n");
  CLOCKS(sha3, SHAKE256(message, message_length, digest_sha3, 128));
}
