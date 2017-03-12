static void bench_ed448()
{
	const int BENCH = 250;
	const int MESSAGE_LENGTH = 64;

	Ed448_PublicKey public_key;
	Ed448_PrivateKey secret_key;
	Ed448_Signature signature;
	uint8_t message[MESSAGE_LENGTH];
	unsigned long long messageLength=MESSAGE_LENGTH;

	printf("======  Ed448 AVX2  ======\n");

	CLOCKS_RANDOM(
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM),
			Ed448.keygen,
			EdDSA.Ed448.keygen(public_key,secret_key));

	oper_second(
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM),
			Ed448.keygen,
			EdDSA.Ed448.keygen(public_key,secret_key));

	CLOCKS_RANDOM(
			random_bytes(message,messageLength);
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM);,
			Ed448.sign,
			EdDSA.Ed448.sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,secret_key));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM);,
			Ed448.sign,
			EdDSA.Ed448.sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,secret_key));

	CLOCKS_RANDOM(
			random_bytes(message,messageLength);
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM);
			EdDSA.Ed448.keygen(public_key,secret_key);
			EdDSA.Ed448.sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,secret_key);,
			Ed448.verify,
			EdDSA.Ed448.verify(message,messageLength,EDDSA_NOCONTEXT,0,public_key,signature));

	oper_second(
			random_bytes(message,messageLength);
			random_bytes(secret_key,ED448_KEY_SIZE_BYTES_PARAM);
			EdDSA.Ed448.keygen(public_key,secret_key);
			EdDSA.Ed448.sign(signature,message,messageLength,EDDSA_NOCONTEXT,0,public_key,secret_key);,
			Ed448.verify,
			EdDSA.Ed448.verify(message,messageLength,EDDSA_NOCONTEXT,0,public_key,signature));
}
