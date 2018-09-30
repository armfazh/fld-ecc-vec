#ifndef EDDSA_TEST_VECTORS_H
#define EDDSA_TEST_VECTORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


enum EDDSA_SCHEME {
  EDDSA_Ed25519,
  EDDSA_Ed25519ph,
  EDDSA_Ed25519ctx,
  EDDSA_Ed448,
  EDDSA_Ed448ph
};

#define NUM_TEST_VECTORS_ED25519 10
#define NUM_TEST_VECTORS_ED448 11

typedef struct _struct_EdDSA_testdata {
  char *name_test;
  enum EDDSA_SCHEME signature_scheme;
  uint8_t private_key[57];
  uint8_t public_key[57];
  uint8_t signature[114];
  uint8_t message[1024];
  unsigned long long message_length;
  uint8_t context[100];
  unsigned long long context_length;
} EdDSA_TestData;

extern const EdDSA_TestData vectors_ed25519[NUM_TEST_VECTORS_ED25519];
extern const EdDSA_TestData vectors_ed448[NUM_TEST_VECTORS_ED448];

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* EDDSA_TEST_VECTORS_H */
