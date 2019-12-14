#include <faz_eddsa_avx2.h>
#include "clocks.h"
#include "prng.h"

static void bench_ed255pure(const SignatureSchemePure *eddsa) {
  const int MESSAGE_LENGTH = 64;

  argEdDSA_PublicKey public_key = eddsa->allocKey();
  argEdDSA_PublicKey private_key = eddsa->allocKey();
  argEdDSA_Signature signature = eddsa->allocSignature();
  uint8_t message[MESSAGE_LENGTH];
  unsigned long long messageLength = MESSAGE_LENGTH;

  oper_second(eddsa->randKey(private_key), eddsa->keygen,
              eddsa->keygen(public_key, private_key));

  oper_second(
      eddsa->randKey(private_key), eddsa->sign,
      eddsa->sign(signature, message, messageLength, public_key, private_key));

  oper_second(
      prgn_random_bytes(message, messageLength); eddsa->randKey(private_key);
      eddsa->keygen(public_key, private_key);
      eddsa->sign(signature, message, messageLength, public_key, private_key);
      , eddsa->verify,
      eddsa->verify(message, messageLength, public_key, signature));
  eddsa->freeKey(public_key);
  eddsa->freeKey(private_key);
  eddsa->freeSignature(signature);
}

static void bench_ed448context(const SignatureSchemeCtx *eddsa) {
  const int MESSAGE_LENGTH = 64;

  argEdDSA_PublicKey public_key = eddsa->allocKey();
  argEdDSA_PublicKey private_key = eddsa->allocKey();
  argEdDSA_Signature signature = eddsa->allocSignature();
  uint8_t message[MESSAGE_LENGTH];
  unsigned long long messageLength = MESSAGE_LENGTH;

  oper_second(eddsa->randKey(private_key), eddsa->keygen,
              eddsa->keygen(public_key, private_key));

  oper_second(eddsa->randKey(private_key), eddsa->sign,
              eddsa->sign(signature, message, messageLength, EDDSA_NOCONTEXT, 0,
                          public_key, private_key));

  oper_second(prgn_random_bytes(message, messageLength);
              eddsa->randKey(private_key);
              eddsa->keygen(public_key, private_key);
              eddsa->sign(signature, message, messageLength, EDDSA_NOCONTEXT, 0,
                          public_key, private_key);
              , eddsa->verify,
              eddsa->verify(message, messageLength, EDDSA_NOCONTEXT, 0,
                            public_key, signature));
  eddsa->freeKey(public_key);
  eddsa->freeKey(private_key);
  eddsa->freeSignature(signature);
}

void bench_ed25519() {
  printf("== Benchmarking EdDSA ====\n");
  printf("====== Ed25519 AVX2 ======\n");
  bench_ed255pure(&Ed25519.Pure);
}

void bench_ed448() {
  printf("== Benchmarking EdDSA ====\n");
  printf("======  Ed448 AVX2  ======\n");
  bench_ed448context(&Ed448.PureContext);
}
