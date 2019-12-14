#ifndef ALL_TESTS_H
#define ALL_TESTS_H

#define TEST_TIMES 1000
#define TEST(X) printf("Test passed? [%s]\n", (X) ? "Ok" : "error")

void test_fp25519();
void test_fp448();
void test_x25519();
void test_ed25519();
void test_ed448();
void test_x448();

#endif /* ALL_TESTS_H */
