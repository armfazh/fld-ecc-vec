#include <stdio.h>
#include <sha512.h>
#include <sha3.h>
#include "clocks.h"

void bench_hash() {
  const int BENCH = 1000;
  const int message_length = 64;
  uint8_t message[message_length];
  uint8_t digest_sha512[SPH_SIZE_sha512];
  uint8_t digest_sha3[128];

  printf("===== Hash Functions =====\n");
  printf("====== SHA512   ==========\n");
  sph_sha512_context mc;
  CLOCKS(sha512,
         sph_sha512_init(&mc);
         sph_sha512(&mc, message, message_length);
         sph_sha512_close(&mc, digest_sha512)
  );
  printf("====== SHAKE256 ==========\n");
  CLOCKS(sha3,
      SHAKE256(message,message_length,digest_sha3,128)
  );
}
