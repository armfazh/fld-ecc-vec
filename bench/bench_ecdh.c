#include <faz_ecdh_avx2.h>
#include "clocks.h"

static void bench_ecdh(const X_ECDH* ecdh)
{
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

static void bench_x25519_avx512()
{
    struct X25519_KEY_x2 ss, sk, pk;
    X25519_AVX512.keygen(&sk, &pk);
    X25519_AVX512.shared(&ss, &sk, &pk);

    oper_second(
        X25519_AVX512.randKey(sk.k0);
        X25519_AVX512.randKey(sk.k1);
        , keygen, X25519_AVX512.keygen(&sk, &pk));
    oper_second(
        X25519_AVX512.randKey(sk.k0);
        X25519_AVX512.randKey(sk.k1);
        X25519_AVX512.randKey(pk.k0);
        X25519_AVX512.randKey(pk.k1);
        , shared, X25519_AVX512.shared(&ss, &sk, &pk));
}

void bench_x25519(void)
{
    printf("===== Benchmarking DH ====\n");
    printf("======  X25519 x64  ======\n");
    bench_ecdh(&X25519_x64);
    printf("======  X25519 AVX2 ======\n");
    bench_ecdh(&X25519_AVX2);
    printf("====== X25519 AVX512 ====\n");
    bench_x25519_avx512();
}

void bench_x448(void)
{
    printf("===== Benchmarking DH ====\n");
    printf("======   X448 AVX2  ======\n");
    bench_ecdh(&X448_AVX2);
}
