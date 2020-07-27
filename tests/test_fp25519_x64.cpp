#include <faz_fp_avx2.h>
#include <gmp.h>
#include <gtest/gtest.h>

using namespace faz::fp;

#define TEST_TIMES 50000

static std::ostream &operator<<(std::ostream &os,
                                const EltFp25519_1w_fullradix &number) {
  os << "0x";
  for (int i = NUM_DIGITS_FP25519_X64 - 1; i >= 0; i--) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << number[i];
  }
  return os << std::endl;
}

static std::ostream &operator<<(std::ostream &os,
                                const EltFp25519_1w_fullradix_buffer &number) {
  os << "0x";
  for (int i = 2 * NUM_DIGITS_FP25519_X64 - 1; i >= 0; i--) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << number[i];
  }
  return os << std::endl;
}

/* Verifies that (a^2-b^2) == (a+b)(a-b) */
TEST(FP25519_1w_x64, MUL_VS_SQR) {
  int64_t i;
  int64_t cnt = 0;
  EltFp25519_1w_fullradix a, b, c, e, f;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  for (i = 0; i < TEST_TIMES; i++) {
    arith->misc.rand(a);
    arith->misc.rand(b);

    arith->add(e, a, b);
    arith->sub(f, a, b);
    arith->mul(e, e, f);

    arith->sqr(a);
    arith->sqr(b);
    arith->sub(c, a, b);

    ASSERT_EQ(arith->cmp(c, e), 0) << "got:  " << c << "want: " << e;
    cnt++;
  }
  EXPECT_EQ(cnt, TEST_TIMES)
      << "passed: " << cnt << "/" << TEST_TIMES << std::endl;
}

/* Verifies that (a*b*a^1) == (b) */
TEST(FP25519_1w_x64, MUL_VS_INV) {
  int64_t i;
  int64_t cnt = 0;
  EltFp25519_1w_fullradix a, b, d;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  for (i = 0; i < TEST_TIMES; i++) {
    arith->misc.rand(a);
    arith->misc.rand(b);
    arith->inv(d, a);
    arith->mul(a, a, b);
    arith->mul(a, a, d);

    ASSERT_EQ(arith->cmp(a, b), 0) << "got:  " << a << "want: " << b;
    cnt++;
  }
  EXPECT_EQ(cnt, TEST_TIMES)
      << "passed: " << cnt << "/" << TEST_TIMES << std::endl;
}

/* Verifies that 0 <= c=a+b < 2^256 and that c be congruent to a+b mod p */
TEST(FP25519_1w_x64, ADDITION) {
  int count = 0;
  EltFp25519_1w_fullradix a, b, get_c, want_c;
  mpz_t gmp_a, gmp_b, gmp_c, two_prime, two_to_256;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  // two_prime = 2^256-38
  mpz_init_set_ui(two_prime, 1);
  mpz_mul_2exp(two_prime, two_prime, 256);
  mpz_sub_ui(two_prime, two_prime, 38);

  // two_to_256 = 2^256
  mpz_init_set_ui(two_to_256, 1);
  mpz_mul_2exp(two_to_256, two_to_256, 256);

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->misc.zero(get_c);
    arith->misc.zero(want_c);

    arith->misc.rand(a);
    arith->misc.rand(b);
    arith->add(get_c, a, b);

    mpz_import(gmp_a, NUM_DIGITS_FP25519_X64, -1, sizeof(a[0]), 0, 0, a);
    mpz_import(gmp_b, NUM_DIGITS_FP25519_X64, -1, sizeof(b[0]), 0, 0, b);

    mpz_add(gmp_c, gmp_a, gmp_b);
    while (mpz_cmp(gmp_c, two_to_256) >= 0) {
      mpz_sub(gmp_c, gmp_c, two_prime);
    }
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(arith->cmp(get_c, want_c), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
  mpz_clear(two_prime);
  mpz_clear(two_to_256);
}

/* Verifies that 0 <= c=a-b < 2^256 and that c be congruent to a-b mod p */
TEST(FP25519_1w_x64, SUBTRACTION) {
  int count = 0;
  EltFp25519_1w_fullradix a, b, get_c, want_c;
  mpz_t gmp_a, gmp_b, gmp_c, two_prime, zero;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  // two_prime = 2^256-38
  mpz_init_set_ui(two_prime, 1);
  mpz_mul_2exp(two_prime, two_prime, 256);
  mpz_sub_ui(two_prime, two_prime, 38);

  // zero = 0
  mpz_init_set_ui(zero, 0);

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->misc.zero(get_c);
    arith->misc.zero(want_c);

    arith->misc.rand(a);
    arith->misc.rand(b);
    arith->sub(get_c, a, b);

    mpz_import(gmp_a, NUM_DIGITS_FP25519_X64, -1, sizeof(a[0]), 0, 0, a);
    mpz_import(gmp_b, NUM_DIGITS_FP25519_X64, -1, sizeof(b[0]), 0, 0, b);

    mpz_sub(gmp_c, gmp_a, gmp_b);
    while (mpz_cmp(gmp_c, zero) < 0) {
      mpz_add(gmp_c, gmp_c, two_prime);
    }
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(arith->cmp(get_c, want_c), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
  mpz_clear(two_prime);
  mpz_clear(zero);
}

/* Verifies that 0 <= c=a*b < 2^512 */
TEST(FP25519_1w_x64, INT_MULTIPLICATION) {
  int count = 0;
  EltFp25519_1w_fullradix a, b;
  EltFp25519_1w_fullradix_buffer get_c, want_c;
  mpz_t gmp_a, gmp_b, gmp_c;
  const Arith_1w *arith = &Fp25519._1w_full.arith;
  const Arith_1w_fullradix *arithex = &Fp25519._1w_full.arithex;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, 2 * SIZE_FP25519);
    memset(want_c, 0, 2 * SIZE_FP25519);

    arith->misc.rand(a);
    arith->misc.rand(b);
    arithex->intmul(get_c, a, b);

    mpz_import(gmp_a, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);
    mpz_import(gmp_b, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, b);

    mpz_mul(gmp_c, gmp_a, gmp_b);
    mpz_export(want_c, NULL, -1, 2 * SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, 2 * SIZE_FP25519), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
}

/* Verifies that 0 <= c=a*a < 2^512 */
TEST(FP25519_1w_x64, INT_SQUARING) {
  int count = 0;
  EltFp25519_1w_fullradix a;
  EltFp25519_1w_fullradix_buffer get_c, want_c;
  mpz_t gmp_a, gmp_c;
  const Arith_1w *arith = &Fp25519._1w_full.arith;
  const Arith_1w_fullradix *arithex = &Fp25519._1w_full.arithex;

  mpz_init(gmp_a);
  mpz_init(gmp_c);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, 2 * SIZE_FP25519);
    memset(want_c, 0, 2 * SIZE_FP25519);

    arith->misc.rand(a);
    arithex->intsqr(get_c, a);

    mpz_import(gmp_a, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);

    mpz_mul(gmp_c, gmp_a, gmp_a);
    mpz_export(want_c, NULL, -1, 2 * SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, 2 * SIZE_FP25519), 0)
        << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;
  mpz_clear(gmp_a);
  mpz_clear(gmp_c);
}

/* Verifies that 0 <= a < 2^256 and that c be congruent to a^-1 mod p */
TEST(FP25519_1w_x64, INVERSION) {
  int count = 0;
  EltFp25519_1w_fullradix a, get_c, want_c;
  mpz_t gmp_a, gmp_c, prime, prime_minus_two;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_c);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  // expo = p-2 = 2^255-19-2 = 2^255-21
  mpz_init_set_ui(prime_minus_two, 1);
  mpz_mul_2exp(prime_minus_two, prime_minus_two, 255);
  mpz_sub_ui(prime_minus_two, prime_minus_two, 21);

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->misc.zero(get_c);
    arith->misc.zero(want_c);

    arith->misc.rand(a);
    arith->inv(get_c, a);

    mpz_import(gmp_a, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);
    mpz_powm(gmp_c, gmp_a, prime_minus_two, prime);
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(arith->cmp(get_c, want_c), 0)
        << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_c);
  mpz_clear(prime);
  mpz_clear(prime_minus_two);
}

/* Verifies that 0 <= c < 2^256 be congruent (mod p) to 0<= a < 2^512 */
TEST(FP25519_1w_x64, REDUCTION) {
  int count = 0;
  EltFp25519_1w_fullradix get_c, want_c;
  EltFp25519_1w_fullradix_buffer a;
  mpz_t gmp_a, gmp_low, gmp_high, two_to_256;
  const Arith_1w *arith = &Fp25519._1w_full.arith;
  const Arith_1w_fullradix *arithex = &Fp25519._1w_full.arithex;

  mpz_init(gmp_a);
  mpz_init(gmp_low);
  mpz_init(gmp_high);

  // two_to_256 = 2^256
  mpz_init_set_ui(two_to_256, 1);
  mpz_mul_2exp(two_to_256, two_to_256, 256);

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->misc.zero(get_c);
    arith->misc.zero(want_c);

    arith->misc.rand(a);
    arith->misc.rand(a + NUM_DIGITS_FP25519_X64);
    arithex->reduce(get_c, a);

    mpz_import(gmp_a, 2 * SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);

    while (mpz_cmp(gmp_a, two_to_256) >= 0) {
      mpz_mod_2exp(gmp_low, gmp_a, 256);
      mpz_div_2exp(gmp_high, gmp_a, 256);
      mpz_mul_ui(gmp_high, gmp_high, 38);
      mpz_add(gmp_a, gmp_low, gmp_high);
    }
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_a);

    ASSERT_EQ(arith->cmp(get_c, want_c), 0)
        << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_low);
  mpz_clear(gmp_high);
  mpz_clear(two_to_256);
}

/* Verifies that 0<= c=a24*a < 2^256 and that c be congruent to a24*a mod p */
TEST(FP25519_1w_x64, MULA24) {
  int count = 0;
  const uint64_t a24 = 121666;
  EltFp25519_1w_fullradix a, get_c, want_c;
  mpz_t gmp_a, gmp_c, gmp_low, gmp_high, two_to_256;
  const Arith_1w *arith = &Fp25519._1w_full.arith;
  const Arith_1w_fullradix *arithex = &Fp25519._1w_full.arithex;

  mpz_init(gmp_a);
  mpz_init(gmp_c);
  mpz_init(gmp_low);
  mpz_init(gmp_high);

  // two_to_256 = 2^256
  mpz_init_set_ui(two_to_256, 1);
  mpz_mul_2exp(two_to_256, two_to_256, 256);

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->misc.zero(get_c);
    arith->misc.zero(want_c);

    arith->misc.rand(a);
    arithex->mula24(get_c, a);

    mpz_import(gmp_a, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);
    mpz_mul_ui(gmp_c, gmp_a, a24);

    while (mpz_cmp(gmp_c, two_to_256) >= 0) {
      mpz_mod_2exp(gmp_low, gmp_c, 256);
      mpz_div_2exp(gmp_high, gmp_c, 256);
      mpz_mul_ui(gmp_high, gmp_high, 38);
      mpz_add(gmp_c, gmp_low, gmp_high);
    }
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(arith->cmp(get_c, want_c), 0)
        << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_c);
  mpz_clear(gmp_low);
  mpz_clear(gmp_high);
  mpz_clear(two_to_256);
}

/* Verifies that 0 <= c=a mod p < 2^255-19 for a number 0 <= a < 2^256 */
TEST(FP25519_1w_x64, FREDUCTION) {
  int count = 0;
  EltFp25519_1w_fullradix a;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  mpz_t gmp_a, gmp_c, prime;
  const Arith_1w *arith = &Fp25519._1w_full.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_c);

  // prime = p-2 = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);

    mpz_import(gmp_a, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a);

    arith->misc.ser(get_c, a);
    mpz_mod(gmp_c, gmp_a, prime);

    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, SIZE_FP25519), 0)
        << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;
  mpz_clear(gmp_a);
  mpz_clear(gmp_c);
  mpz_clear(prime);
}
