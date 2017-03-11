#include <eddsa_avx2.h>

static void bench_x448()
{
	const int BENCH = 250;
	ECDH_X448_KEY secret_key;
	ECDH_X448_KEY public_key;
	ECDH_X448_KEY shared_secret;
	printf("======   X448 AVX2  ======\n");

	random_bytes(secret_key, ECDH448_KEY_SIZE_BYTES);

	CLOCKS_RANDOM(
		random_bytes(secret_key, ECDH448_KEY_SIZE_BYTES),
		X448_KeyGen,
		ECDHX.X448.keygen(public_key, secret_key)
	);

	oper_second(
		random_bytes(secret_key, ECDH448_KEY_SIZE_BYTES),
		X448_KeyGen,
		ECDHX.X448.keygen(public_key, secret_key)
	);

	CLOCKS_RANDOM(
		random_bytes(secret_key, ECDH448_KEY_SIZE_BYTES);
		random_bytes(public_key, ECDH448_KEY_SIZE_BYTES),
		X448_Shared,
		ECDHX.X448.shared(shared_secret, public_key, secret_key)
	);

	oper_second(
		random_bytes(secret_key, ECDH448_KEY_SIZE_BYTES);
		random_bytes(public_key, ECDH448_KEY_SIZE_BYTES),
		X448_Shared,
		ECDHX.X448.shared(shared_secret, public_key, secret_key)
	);
}
