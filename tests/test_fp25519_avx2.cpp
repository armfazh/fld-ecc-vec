#include <faz_fp_avx2.h>
#include <gmp.h>
#include <gtest/gtest.h>

using namespace faz::fp;

#define TEST_TIMES 50000

static std::ostream &operator<<(std::ostream &os,
                                const EltFp25519_1w_redradix &number) {
  os << "0x";
  for (int i = (NUM_DIGITS_FP25519)-1; i > 0; i -= 2) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << number[i];
  }
  for (int i = (NUM_DIGITS_FP25519)-2; i >= 0; i -= 2) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << number[i];
  }
  return os << std::endl;
}

static void vec_to_gmp(mpz_t number, argElement_1w vect) {
  ALIGN uint8_t a_bytes[SIZE_FP25519];
  Fp25519._1w_red.arith.misc.ser(a_bytes, vect);
  mpz_import(number, SIZE_FP25519, -1, sizeof(uint8_t), 0, 0, a_bytes);
}

/* Verifies that (a^2-b^2) == (a+b)(a-b) */
TEST(FP25519_1w_AVX2, MUL_VS_SQR) {
  int64_t i;
  int64_t cnt = 0;
  EltFp25519_1w_redradix a, b, c, d, e;
  const Arith_1w *arith = &Fp25519._1w_red.arith;
  const Arith_1w_redradix *arithex = &Fp25519._1w_red.arithex;

  for (i = 0; i < TEST_TIMES; i++) {
    arith->misc.rand(a);
    arith->misc.rand(b);

    arith->add(e, a, b);
    arith->sub(d, a, b);
    arithex->compress(d);
    arith->mul(e, e, d);

    arith->sqr(a);
    arith->sqr(b);
    arith->sub(c, a, b);
    arithex->compress(c);

    ASSERT_EQ(arith->cmp(c, e), 0) << "got:  " << c << "want: " << e;
    cnt++;
  }
  EXPECT_EQ(cnt, TEST_TIMES)
      << "passed: " << cnt << "/" << TEST_TIMES << std::endl;
}

/* Verifies that (a*b*a^1) == (b) */
TEST(FP25519_1w_AVX2, MUL_VS_INV) {
  int64_t i;
  int64_t cnt = 0;
  EltFp25519_1w_redradix a, b, c;
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  for (i = 0; i < TEST_TIMES; i++) {
    arith->misc.rand(a);
    arith->misc.rand(b);

    arith->inv(c, a);
    arith->mul(a, a, b);
    arith->mul(a, a, c);

    ASSERT_EQ(arith->cmp(a, b), 0) << "got:  " << a << "want: " << b;
    cnt++;
  }
  EXPECT_EQ(cnt, TEST_TIMES)
      << "passed: " << cnt << "/" << TEST_TIMES << std::endl;
}

/* Verifies that 0 <= c=a+b < 2^256 and that c be congruent to a+b mod p */
TEST(FP25519_1w_AVX2, ADDITION) {
  int count = 0;
  EltFp25519_1w_redradix a, b, c;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  mpz_t gmp_a, gmp_b, gmp_c, prime;
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);
    arith->misc.rand(b);
    vec_to_gmp(gmp_a, a);
    vec_to_gmp(gmp_b, b);

    arith->add(c, a, b);
    arith->misc.ser(get_c, c);

    mpz_add(gmp_c, gmp_a, gmp_b);
    mpz_mod(gmp_c, gmp_c, prime);

    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, SIZE_FP25519), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
  mpz_clear(prime);
}

/* Verifies that 0 <= c=a-b < 2^256 and that c be congruent to a-b mod p */
TEST(FP25519_1w_AVX2, SUBTRACTION) {
  int count = 0;
  EltFp25519_1w_redradix a, b, c;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  mpz_t gmp_a, gmp_b, gmp_c, prime;
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);
    arith->misc.rand(b);
    vec_to_gmp(gmp_a, a);
    vec_to_gmp(gmp_b, b);

    arith->sub(c, a, b);
    arith->misc.ser(get_c, c);

    mpz_sub(gmp_c, gmp_a, gmp_b);
    mpz_mod(gmp_c, gmp_c, prime);

    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, SIZE_FP25519), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;

  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
  mpz_clear(prime);
}

/* Verifies that 0 <= c=a*b < p */
TEST(FP25519_1w_AVX2, MULTIPLICATION) {
  int count = 0;
  EltFp25519_1w_redradix a, b, c;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  mpz_t gmp_a, gmp_b, gmp_c, prime;
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_b);
  mpz_init(gmp_c);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);
    arith->misc.rand(b);
    vec_to_gmp(gmp_a, a);
    vec_to_gmp(gmp_b, b);

    arith->mul(c, a, b);
    arith->misc.ser(get_c, c);

    mpz_mul(gmp_c, gmp_a, gmp_b);
    mpz_mod(gmp_c, gmp_c, prime);
    mpz_export(want_c, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

    ASSERT_EQ(memcmp(get_c, want_c, SIZE_FP25519), 0)
        << "a: " << a << "b: " << b << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES)
      << "passed: " << count << "/" << TEST_TIMES << std::endl;
  mpz_clear(gmp_a);
  mpz_clear(gmp_b);
  mpz_clear(gmp_c);
  mpz_clear(prime);
}

/* Verifies that 0 <= c=a*a < p */
TEST(FP25519_1w_AVX2, SQUARING) {
  int count = 0;
  EltFp25519_1w_redradix a;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  mpz_t gmp_a, gmp_c, prime;
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  mpz_init(gmp_a);
  mpz_init(gmp_c);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  for (int i = 0; i < TEST_TIMES; i++) {
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);
    vec_to_gmp(gmp_a, a);

    arith->sqr(a);
    arith->misc.ser(get_c, a);

    mpz_mul(gmp_c, gmp_a, gmp_a);
    mpz_mod(gmp_c, gmp_c, prime);
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

/* Verifies that 0 <= c=a^(-1) < p */
TEST(FP25519_1w_AVX2, INVERSION) {
  int count = 0;
  EltFp25519_1w_redradix a, c;
  ALIGN uint8_t get_c[SIZE_FP25519];
  ALIGN uint8_t want_c[SIZE_FP25519];
  const Arith_1w *arith = &Fp25519._1w_red.arith;

  mpz_t gmp_a, gmp_c, prime, prime_minus_two;
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
    memset(get_c, 0, SIZE_FP25519);
    memset(want_c, 0, SIZE_FP25519);

    arith->misc.rand(a);
    vec_to_gmp(gmp_a, a);

    arith->inv(c, a);
    arith->misc.ser(get_c, c);

    mpz_powm(gmp_c, gmp_a, prime_minus_two, prime);
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
  mpz_clear(prime_minus_two);
}

/* TODO */
#if 0
static std::ostream &operator<<(std::ostream &os,
                                const EltFp25519_1w_fullradix &number) {
  os << "0x";
  for (int i = NUM_DIGITS_FP25519_X64 - 1; i >= 0; i--) {
    os << std::setbase(16) << std::setfill('0') << std::setw(16) << number[i];
  }
  return os << std::endl;
}

static int gmp_scalar_to_vect(argElement_1w vect, argElement_1w scalar) {
  mpz_t gmp_a, gmp_remainder;

  mpz_init(gmp_a);
  mpz_init(gmp_remainder);
  const Arith_1way *arith = &Fp25519._1w_red;

  // Set gmp with scalar
  arith->zero(vect);
  mpz_import(gmp_a, NUM_DIGITS_FP25519_X64, -1, sizeof(scalar[0]), 0, 0,
             scalar);

  assert(mpz_sizeinbase(gmp_a, 2) < 256);
  int i = 0;
  while (mpz_cmp_ui(gmp_a, 0) > 0) {
    mpz_mod_2exp(gmp_remainder, gmp_a, 26 - (i % 2));
    mpz_div_2exp(gmp_a, gmp_a, 26 - (i % 2));
    mpz_export(vect + i, NULL, -1, sizeof(vect[0]), 0, 0, gmp_remainder);
    i++;
  }
  to_1w_h0h5(vect);
  mpz_clear(gmp_a);
  mpz_clear(gmp_remainder);
  return i;
}

/* Verifies the conversion from full-radix to reduced-radix */
TEST(FP25519_1w_AVX2, SCALAR2VECT) {
  int count = 0;
  EltFp25519_1w_fullradix a;
  EltFp25519_1w_redradix get_c, want_c;
  const Arith_1way *arith = &Fp25519._1w_red;

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->zero(get_c);
    arith->zero(want_c);

    arith->rand(get_c);
    int num_words = gmp_scalar_to_vect(want_c, get_c);

    ASSERT_EQ(num_words, NUM_DIGITS_FP25519);

    for (int j = 0; j < NUM_DIGITS_FP25519; j++) {
      ASSERT_EQ(get_c[j], want_c[j]) << "a: " << a << "got:  " << get_c
                                     << "want: " << want_c;
    }
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES) << "passed: " << count << "/" << TEST_TIMES
                               << std::endl;
}

static void gmp_vect_to_scalar(argElement_1w scalar, argElement_1w vect) {
  mpz_t gmp_a, gmp_c, prime;

  mpz_init(gmp_c);
  mpz_init_set_ui(gmp_a, 0);

  // prime = 2^255-19
  mpz_init_set_ui(prime, 1);
  mpz_mul_2exp(prime, prime, 255);
  mpz_sub_ui(prime, prime, 19);

  from_1w_h0h5(vect);
  for (int i = NUM_DIGITS_FP25519 - 1; i >= 0; i--) {
    mpz_mul_2exp(gmp_a, gmp_a, 26 - (i % 2));
    mpz_add_ui(gmp_a, gmp_a, vect[i]);
  }
  mpz_mod(gmp_c, gmp_a, prime);
  to_1w_h0h5(vect);

  mpz_export(scalar, NULL, -1, SIZE_FP25519, 0, 0, gmp_c);

  mpz_clear(gmp_a);
  mpz_clear(gmp_c);
  mpz_clear(prime);
}

/* Verifies the conversion from reduced-radix to full-radix */
TEST(FP25519_1w_AVX2, VECT2SCALAR) {
  int count = 0;
  EltFp25519_1w_redradix a;
  EltFp25519_1w_fullradix get_c, want_c;
  const Arith_1way *arith = &Fp25519._1w_red;

  for (int i = 0; i < TEST_TIMES; i++) {
    arith->zero(get_c);
    arith->zero(want_c);
    arith->rand(a);

    arith->ser(get_c,a);
    gmp_vect_to_scalar(want_c, a);

    ASSERT_EQ(memcmp(get_c, want_c, SIZE_FP25519), 0)
                  << "a: " << a << "got:  " << get_c << "want: " << want_c;
    count++;
  }
  EXPECT_EQ(count, TEST_TIMES) << "passed: " << count << "/" << TEST_TIMES
                               << std::endl;
}
#endif
