#include <iomanip>
#include <faz_eddsa_avx2.h>
#include <gtest/gtest.h>
#include "vectors.h"

using namespace faz::eddsa;

#define TEST_TIMES 50000

static std::ostream &operator<<(std::ostream &os,
                                const Ed25519_PublicKey &key) {
  unsigned int i = 0;
  for (i = 0; i < Ed25519.Pure.key_size; i++) {
    os << std::setbase(16) << std::setfill('0') << std::setw(2)
       << static_cast<int>(key[i]);
  }
  return os << std::endl;
}

static std::ostream &operator<<(std::ostream &os,
                                const Ed25519_Signature &sig) {
  unsigned int i = 0;
  for (i = 0; i < Ed25519.Pure.signature_size; i++) {
    os << std::setbase(16) << std::setfill('0') << std::setw(2)
       << static_cast<int>(sig[i]);
  }
  return os << std::endl;
}

TEST(ED25519, RFC8032) {
  Ed25519_PublicKey run_publickey;
  Ed25519_Signature run_signature;
  EDDSA_FLAGS valid;

  for (int i = 0; i < NUM_TEST_VECTORS_ED25519; i++) {
    const EdDSA_TestData *test = &vectors_ed25519[i];
    //    std::cout << "Test: " << test->name_test << std::endl;
    Ed25519.Pure.keygen(run_publickey, test->private_key);
    EXPECT_EQ(memcmp(run_publickey, test->public_key, Ed25519.Pure.key_size), 0)
        << "got: " << run_publickey << "want: " << test->public_key;

    switch (test->signature_scheme) {
      case EDDSA_Ed25519:
        Ed25519.Pure.sign(run_signature, test->message, test->message_length,
                          test->public_key, test->private_key);
        EXPECT_EQ(
            memcmp(run_signature, test->signature, Ed25519.Pure.signature_size),
            0)
            << "got: " << run_signature << "want: " << test->signature;

        valid = (EDDSA_FLAGS)Ed25519.Pure.verify(
            test->message, test->message_length, run_publickey, run_signature);
        EXPECT_EQ(valid, EDDSA_VERIFICATION_OK)
            << "got: " << valid << " want: " << EDDSA_VERIFICATION_OK;
        break;
      default:
        break;
    }
  }
}
