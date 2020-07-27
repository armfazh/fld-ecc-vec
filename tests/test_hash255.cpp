#include <faz_hash_avx2.h>
#include <gtest/gtest.h>
#include <prng.h>

using namespace faz::hash;
using namespace prng;

#define TEST_TIMES 10000

namespace faz {
namespace hash {
static std::ostream &operator<<(std::ostream &os, const Point &P) {
  int i = 0;
  os << "0x";
  for (i = NUM_DIGITS_FP25519_X64 - 1; i >= 0; i--) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << P.X[i];
  }
  os << std::endl;
  os << "0x";
  for (i = NUM_DIGITS_FP25519_X64 - 1; i >= 0; i--) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << P.Y[i];
  }
  return os;
}
}  // namespace hash
}  // namespace faz

TEST(HASH, H2C_25519_x64) {
  int64_t i = 0, TIMES = TEST_TIMES;
  int cnt = 0;
  const size_t MLEN = 256;
  uint8_t message[MLEN];
  Point P;

  for (i = 0; i < TIMES; i++) {
    prgn_random_bytes(message, MLEN);
    h2c25519_x64(&P, message, MLEN);
    EXPECT_EQ(isOnCurve(&P), 1) << "invalid point: " << P << std::endl;
    cnt++;
  }
  EXPECT_EQ(cnt, TIMES) << "passed: " << cnt << "/" << TIMES << std::endl;
}

TEST(HASH, H2C_25519_AVX2) {
  int64_t i = 0, TIMES = TEST_TIMES;
  int cnt = 0;
  const size_t MLEN = 256;
  uint8_t message[MLEN];
  Point P;

  for (i = 0; i < TIMES; i++) {
    prgn_random_bytes(message, MLEN);
    h2c25519_avx2(&P, message, MLEN);
    EXPECT_EQ(isOnCurve(&P), 1) << "invalid point: " << P << std::endl;
    cnt++;
  }
  EXPECT_EQ(cnt, TIMES) << "passed: " << cnt << "/" << TIMES << std::endl;
}

TEST(HASH, X64_AVX2) {
  int64_t i = 0, TIMES = TEST_TIMES;
  int cnt = 0;
  const size_t MLEN = 256;
  uint8_t message[MLEN];
  Point P0, P1;

  for (i = 0; i < TIMES; i++) {
    prgn_random_bytes(message, MLEN);
    h2c25519_x64(&P0, message, MLEN);
    h2c25519_avx2(&P1, message, MLEN);
    EXPECT_EQ(areEqual(&P0, &P1), 1)
        << "got: " << P0 << "want: " << P1 << std::endl;
    cnt++;
  }
  EXPECT_EQ(cnt, TIMES) << "passed: " << cnt << "/" << TIMES << std::endl;
}
