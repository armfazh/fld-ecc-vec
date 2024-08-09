#include <faz_ecdh_avx2.h>
#include "clocks.h"

static void bench_ecdh(const X_ECDH* ecdh) {
  argECDHX_Key secret_key = ecdh->allocKey();
  argECDHX_Key session_key = ecdh->allocKey();
  argECDHX_Key shared_secret = ecdh->allocKey();

  ecdh->randKey(secret_key);

  oper_second(ecdh->randKey(secret_key), ecdh->keygen,
              ecdh->keygen(session_key, secret_key));

  oper_second(ecdh->randKey(secret_key);
              ecdh->randKey(session_key), ecdh->shared,
              ecdh->shared(shared_secret, session_key, secret_key));
  ecdh->freeKey(secret_key);
  ecdh->freeKey(session_key);
  ecdh->freeKey(shared_secret);
}

void bench_x25519(void) {
  printf("===== Benchmarking DH ====\n");
  printf("======  X25519 x64  ======\n");
  bench_ecdh(&X25519_x64);
  printf("======  X25519 AVX2 ======\n");
  bench_ecdh(&X25519);
}

void bench_x448(void) {
  printf("===== Benchmarking DH ====\n");
  printf("======   X448 AVX2  ======\n");
  bench_ecdh(&X448);
}
