#include <faz_eddsa_avx2.h>
#include "vectors.h"
#include <stdio.h>
#include <string.h>

#define TEST_TIMES 1000
#define LABEL "Test passed? [%s]\n"
#define OK "Ok"
#define ERROR "error"

static void test_vector(const EdDSA_TestData *test) {
  int valid = EDDSA_INVALID_SIGNATURE;

  printf("Test: %s\n", test->name_test);
  if (test->signature_scheme == EDDSA_Ed25519) {
    const SignatureSchemePure *scheme = &Ed25519.Pure;
    argEdDSA_PublicKey run_publickey = scheme->allocKey();
    argEdDSA_Signature run_signature = scheme->allocSignature();

    scheme->keygen(run_publickey, test->private_key);
    printf(LABEL, memcmp(run_publickey, test->public_key, scheme->key_size) == 0 ? OK : ERROR);
    scheme->sign(run_signature, test->message, test->message_length,
                 test->public_key, test->private_key);
    printf(LABEL, memcmp(run_signature, test->signature, scheme->signature_size) == 0 ? OK : ERROR);
    valid = scheme->verify(test->message, test->message_length,
                           run_publickey, run_signature);
    printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);

    scheme->freeKey(run_publickey);
    scheme->freeSignature(run_signature);
  } else {
    const SignatureSchemeCtx *schemectx;
    switch (test->signature_scheme) {
      case EDDSA_Ed25519ph: schemectx = &Ed25519.PreHash;
        break;
      case EDDSA_Ed25519ctx: schemectx = &Ed25519.Context;
        break;
      case EDDSA_Ed448: schemectx = &Ed448.PureContext;
        break;
      case EDDSA_Ed448ph: schemectx = &Ed448.PreHash;
        break;
      default: schemectx = NULL;
        printf("Error in test.\n");
    }
    if (schemectx != NULL) {
      argEdDSA_PublicKey run_publickey = schemectx->allocKey();
      argEdDSA_Signature run_signature = schemectx->allocSignature();

      schemectx->keygen(run_publickey, test->private_key);
      printf(LABEL, memcmp(run_publickey, test->public_key, schemectx->key_size) == 0 ? OK : ERROR);
      schemectx->sign(run_signature, test->message, test->message_length,
                      test->context, test->context_length, test->public_key,
                      test->private_key);
      printf(LABEL, memcmp(run_signature, test->signature, schemectx->signature_size) == 0 ? OK : ERROR);
      valid = schemectx->verify(test->message, test->message_length,
                                test->context, test->context_length,
                                run_publickey, run_signature);
      printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);

      schemectx->freeKey(run_publickey);
      schemectx->freeSignature(run_signature);
    }
  }
}

void test_ed25519() {
  printf("==== RFC8032 Test ====\n");
  int i = 0;
  for (i = 0; i < NUM_TEST_VECTORS_ED25519; i++) {
    test_vector(vectors_ed25519 + i);
  }
}

void test_ed448() {
  printf("==== RFC8032 Test ====\n");
  int i = 0;
  for (i = 0; i < NUM_TEST_VECTORS_ED448; i++) {
    test_vector(vectors_ed448 + i);
  }
}
