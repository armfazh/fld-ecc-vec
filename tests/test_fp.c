#include <faz_fp_avx2.h>
#include <stdio.h>
#include "tests.h"

static void test_1w(const Arith_1w *fp)
{
    int64_t i;
    int64_t cnt = 0, match = 0;
    argElement_1w a = fp->misc.alloc();
    argElement_1w b = fp->misc.alloc();
    argElement_1w c = fp->misc.alloc();
    argElement_1w d = fp->misc.alloc();
    argElement_1w e = fp->misc.alloc();
    argElement_1w f = fp->misc.alloc();

    /**
     * Test identity:
     *    (a^2-b^2) == (a+b)(a-b)
     */
    printf("Test mul/sqr:");
    cnt = 0;
    for (i = 0; i < TEST_TIMES; i++) {
        fp->misc.rand(a);
        fp->misc.rand(b);

        fp->add(e, a, b);
        fp->sub(f, a, b);
        fp->mul(e, e, f);

        fp->sqr(a);
        fp->sqr(b);
        fp->sub(c, a, b);

        match = fp->cmp(c, e) == 0;
        if (!match) {
            fprintf(stderr, "A:\n");
            fp->misc.print(stderr, a);
            fprintf(stderr, "B:\n");
            fp->misc.print(stderr, b);
            break;
        }
        cnt += match;
    }
    printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");

    /**
     * Test identity:
     *    (a*b*a^1) == b
     */
    printf("Test mul/inv:");
    cnt = 0;
    for (i = 0; i < TEST_TIMES; i++) {
        fp->misc.rand(a);
        fp->misc.rand(b);

        fp->inv(d, a);
        fp->mul(a, a, b);
        fp->mul(a, a, d);

        match = fp->cmp(a, b) == 0;
        if (!match) {
            fprintf(stderr, "A:\n");
            fp->misc.print(stderr, a);
            fprintf(stderr, "B:\n");
            fp->misc.print(stderr, b);
            break;
        }
        cnt += match;
    }
    printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");

    fp->misc.free(a);
    fp->misc.free(b);
    fp->misc.free(c);
    fp->misc.free(d);
    fp->misc.free(e);
    fp->misc.free(f);
}

static void test_2w(const Arith_2w *fp)
{
    const __m256i ONES = _mm256_set1_epi8(-1);
    int64_t i;
    int64_t cnt = 0, match = 0;

    argElement_2w a = fp->misc.alloc();
    argElement_2w b = fp->misc.alloc();
    argElement_2w c = fp->misc.alloc();
    argElement_2w d = fp->misc.alloc();
    argElement_2w e = fp->misc.alloc();

    /**
     * Test identity:
     *    (a^2-b^2) == (a+b)(a-b)
     */
    printf("Test mul/sqr:");
    cnt = 0;
    for (i = 0; i < TEST_TIMES; i++) {
        fp->misc.rand(a);
        fp->misc.rand(b);

        fp->add(e, a, b);
        fp->sub(d, a, b);
        fp->mul(e, e, d);

        fp->sqr(a);
        fp->sqr(b);
        fp->sub(c, a, b);

        match = _mm256_testz_si256(fp->cmp(c, e), ONES);
        if (!match) {
            printf("A:\n");
            fp->misc.print(stderr, a);
            printf("B:\n");
            fp->misc.print(stderr, b);
            printf("C:\n");
            fp->misc.print(stderr, c);
            printf("E:\n");
            fp->misc.print(stderr, e);
            break;
        }
        cnt += match;
    }
    printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");
    fp->misc.free(a);
    fp->misc.free(b);
    fp->misc.free(c);
    fp->misc.free(d);
    fp->misc.free(e);
}

static void test_4w(const Arith_4w *fp)
{
    const __m256i ONES = _mm256_set1_epi8(-1);
    int64_t i;
    int64_t cnt = 0, match = 0;

    argElement_2w a = fp->misc.alloc();
    argElement_2w b = fp->misc.alloc();
    argElement_2w c = fp->misc.alloc();
    argElement_2w d = fp->misc.alloc();
    argElement_2w e = fp->misc.alloc();

    /**
     * Test identity:
     *    (a^2-b^2) == (a+b)(a-b)
     */
    printf("Test mul/sqr:");
    cnt = 0;
    for (i = 0; i < TEST_TIMES; i++) {
        fp->misc.rand(a);
        fp->misc.rand(b);

        fp->add(e, a, b);
        fp->sub(d, a, b);
        fp->mul(e, e, d);

        fp->sqr(a);
        fp->sqr(b);
        fp->sub(c, a, b);

        match = _mm256_testz_si256(fp->cmp(c, e), ONES);
        if (!match) {
            printf("A:\n");
            fp->misc.print(stderr, a);
            printf("B:\n");
            fp->misc.print(stderr, b);
            printf("C:\n");
            fp->misc.print(stderr, c);
            printf("E:\n");
            fp->misc.print(stderr, e);
            break;
        }
        cnt += match;
    }
    printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");
    fp->misc.free(a);
    fp->misc.free(b);
    fp->misc.free(c);
    fp->misc.free(d);
    fp->misc.free(e);
}

static void test_2w_x2(const Arith_2w_x2 *fp)
{
    const __m256i ONES = _mm256_set1_epi8(-1);
    int64_t i;
    int64_t cnt = 0, match0 = 0, match1 = 0;

    argElement_2w_x2 a = fp->misc.alloc();
    argElement_2w_x2 b = fp->misc.alloc();
    argElement_2w_x2 c = fp->misc.alloc();
    argElement_2w_x2 d = fp->misc.alloc();
    argElement_2w_x2 e = fp->misc.alloc();

    /**
     * Test identity:
     *    (a^2-b^2) == (a+b)(a-b)
     */
    printf("Test mul/sqr:");
    cnt = 0;
    for (i = 0; i < TEST_TIMES; i++) {
        fp->misc.rand(a);
        fp->misc.rand(b);

        fp->add(e, a, b);
        fp->sub(d, a, b);
        fp->mul(e, e, d);

        fp->sqr(a);
        fp->sqr(b);
        fp->sub(c, a, b);

        __m512i cmp = fp->cmp(c, e);
        match0 = _mm256_testz_si256(_mm512_extracti64x4_epi64(cmp,0),ONES);
        match1 = _mm256_testz_si256(_mm512_extracti64x4_epi64(cmp,1),ONES);
        if (!match0 || !match1) {
            printf("A:\n");
            fp->misc.print(stderr, a);
            printf("B:\n");
            fp->misc.print(stderr, b);
            printf("C:\n");
            fp->misc.print(stderr, c);
            printf("E:\n");
            fp->misc.print(stderr, e);
            break;
        }
        cnt += match0||match1;
    }
    printf(" %ld %s\n", cnt, cnt == TEST_TIMES ? "OK" : "FAIL");
    fp->misc.free(a);
    fp->misc.free(b);
    fp->misc.free(c);
    fp->misc.free(d);
    fp->misc.free(e);
}

void test_fp25519()
{
    printf("=== Testing Arith ====\n");
    printf("===   p=2^255-19  ====\n");
    printf("=== 1-way (full)  ====\n");
    test_1w(&Fp25519._1w_full.arith);
    printf("=== 1-way (red)   ====\n");
    test_1w(&Fp25519._1w_red.arith);
    printf("=== 2-way (red)   ====\n");
    test_2w(&Fp25519._2w_red.arith);
    printf("=== 4-way (red)   ====\n");
    test_4w(&Fp25519._4w_red.arith);
    printf("=== 2-way x2 (red)   ====\n");
    test_2w_x2(&Fp25519._2w_red_x2.arith);
}

void test_fp448()
{
    printf("=== Testing Arith  ===\n");
    printf("=== p=2^448-2^224-1 ==\n");
    printf("=== 1-way (full)  ====\n");
    test_1w(&Fp448._1w_full.arith);
    printf("=== 1-way (red)   ====\n");
    test_1w(&Fp448._1w_red.arith);
    printf("=== 2-way (red)   ====\n");
    test_2w(&Fp448._2w_red.arith);
    printf("=== 4-way (red)   ====\n");
    test_4w(&Fp448._4w_red.arith);
}
