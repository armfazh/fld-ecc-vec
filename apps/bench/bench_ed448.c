#include <faz_eddsa_avx2.h>

static void bench_eddsa(const SignatureSchemeCtx * eddsa)
{
	const int BENCH = 250;
	const int MESSAGE_LENGTH = 64;

	argEdDSA_PublicKey public_key = eddsa->newKey();
	argEdDSA_PublicKey private_key = eddsa->newKey();
	argEdDSA_Signature signature = eddsa->newSignature();
	uint8_t message[MESSAGE_LENGTH];
	unsigned long long messageLength=MESSAGE_LENGTH;

	CLOCKS_RANDOM(
			random_bytes(private_key,eddsa->key_size),
			eddsa->keygen,
			eddsa->keygen(public_key,private_key));

	oper_second(
			random_bytes(private_key,eddsa->key_size),
			eddsa->keygen,
			eddsa->keygen(public_key,private_key));

	CLOCKS_RANDOM(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);,
	        eddsa->sign,
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);,
	        eddsa->sign,
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key));

	CLOCKS_RANDOM(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);
			eddsa->keygen(public_key,private_key);
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key);,
	        eddsa->verify,
			eddsa->verify(message,messageLength,EDDSA_NOCONTEXT,0,public_key,signature));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(private_key,eddsa->key_size);
			eddsa->keygen(public_key,private_key);
			eddsa->sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,private_key);,
			eddsa->verify,
			eddsa->verify(message,messageLength,EDDSA_NOCONTEXT,0,public_key,signature));
	eddsa->cleanKey(public_key);
	eddsa->cleanKey(private_key);
	eddsa->cleanSignature(signature);
}
static void bench_ed25519()
{
	bench_eddsa(&EdDSA.Ed25519ctx);
}
static void bench_ed448()
{
	bench_eddsa(&EdDSA.Ed448);
}
