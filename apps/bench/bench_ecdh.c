#include <faz_ecdh_avx2.h>

static void bench_ecdh(const DiffieHellmanXFunction * ecdh)
{
	argECDHX_Key secret_key = ecdh->initKey();
	argECDHX_Key session_key = ecdh->initKey();
	argECDHX_Key shared_secret = ecdh->initKey();

	random_bytes(secret_key, ecdh->key_size);

	oper_second(
		random_bytes(secret_key, ecdh->key_size),
		ecdh->keygen,
		ecdh->keygen(session_key, secret_key)
	);

	oper_second(
		random_bytes(secret_key, ecdh->key_size);
		random_bytes(session_key, ecdh->key_size),
		ecdh->shared,
		ecdh->shared(shared_secret, session_key, secret_key)
	);
	ecdh->clearKey(secret_key);
	ecdh->clearKey(session_key);
	ecdh->clearKey(shared_secret);
}

static void bench_x25519()
{
	printf("===== Benchmarking DH ====\n");
	printf("======  X25519 x64  ======\n");
	bench_ecdh(&ECDHX.X25519_x64);
	printf("======  X25519 AVX2 ======\n");
	bench_ecdh(&ECDHX.X25519);
}
static void bench_x448()
{
	printf("===== Benchmarking DH ====\n");
	printf("======   X448 AVX2  ======\n");
	bench_ecdh(&ECDHX.X448);
}
