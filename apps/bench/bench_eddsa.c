#include <faz_eddsa_avx2.h>

static void bench_eddsa(const SignatureSchemeCtx * eddsa)
{
	const int MESSAGE_LENGTH = 64;

	argEdDSA_PublicKey public_key = eddsa->initKey();
	argEdDSA_PublicKey private_key = eddsa->initKey();
	argEdDSA_Signature signature = eddsa->initSignature();
	uint8_t message[MESSAGE_LENGTH];
	unsigned long long messageLength=MESSAGE_LENGTH;

	oper_second(
			random_bytes(private_key,eddsa->key_size),
			eddsa->keygen,
			eddsa->keygen(public_key,private_key));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);,
	        eddsa->sign,
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);
			eddsa->keygen(public_key,private_key);
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key);,
			eddsa->verify,
			eddsa->verify(message,messageLength,EDDSA_NOCONTEXT,0,public_key,signature));
	eddsa->clearKey(public_key);
	eddsa->clearKey(private_key);
	eddsa->clearSignature(signature);
}
static void bench_ed25519()
{
	printf("== Benchmarking EdDSA ====\n");
	printf("====== Ed25519 AVX2 ======\n");
	bench_eddsa(&EdDSA.Ed25519ctx);
}
static void bench_ed448()
{
	printf("== Benchmarking EdDSA ====\n");
	printf("======  Ed448 AVX2  ======\n");
	bench_eddsa(&EdDSA.Ed448);
}
