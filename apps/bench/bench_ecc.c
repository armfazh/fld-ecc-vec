#include <ecc_ed448.h>
#include <ecc_ed25519.h>
#include <sha3.h>
#include <sha512.h>
#include <faz_eddsa_avx2.h>

static void bench_ecc25519()
{
	const int BENCH = 350;

	Ed25519_PrivateKey key;
	ALIGN uint64_t S[112];
	ALIGN uint64_t K[112];
	ALIGN int8_t L[512];
	PointXYZT_4way_Fp25519 Q;
	Point_precmp_4way_Fp25519 P;
	PointXYZT_2w_H0H5 PP,QQ;
	Ed25519_Digest digest;
	sph_sha512_context context;
	uint8_t message[ED25519_HASH_BYTES_PARAM];

	printf("======       ECC    ======\n");

	CLOCKS(PointAddition,_1way_fulladd_2w_H0H5(&PP, &QQ));
	CLOCKS(PointDoubling,_1way_doubling_2w_H0H5(&PP));
	CLOCKS(4-way_Addition,_4way_mixadd_ed25519(&Q, &P));
//	CLOCKS_RANDOM(int t=0;, 4-way_Query,query_table_ed25519(&P, ((uint8_t*)Tab)+SIZE_ONE_LUT_ED25519*t,S, K));

	printf("======      MISC    ======\n");
	CLOCKS_RANDOM(random_bytes(key,ED25519_KEY_SIZE_BYTES_PARAM),wnaf,wnaf_256bits(L, key, 5));
	CLOCKS_RANDOM(random_bytes(key,ED25519_KEY_SIZE_BYTES_PARAM),Recoding,recoding_signed_scalar_ed25519(S,K,key));
	CLOCKS_RANDOM(
			random_bytes(message,ED25519_HASH_BYTES_PARAM),
			SHA512,
			sph_sha512_init(&context);
			sph_sha512(&context,message,ED25519_HASH_BYTES_PARAM);
			sph_sha512_close(&context,digest)
	);
}

static void bench_ecc448()
{
	const int BENCH = 350;

	Ed448_PrivateKey key;
	ALIGN uint64_t S[112];
	ALIGN uint64_t K[112];
	ALIGN int8_t L[512];
	PointXYZT_4way_Fp448 Q;
	Point_precmp_4way_Fp448 P;
	PointXYZT_2w_H0H8 PP,QQ;
	Ed448_Digest digest;
	uint8_t message[ED448_HASH_BYTES_PARAM];

	printf("======       ECC    ======\n");

	CLOCKS(PointAddition,_1way_fulladd_2w_H0H8(&PP, &QQ));
	CLOCKS(PointDoubling,_1way_doubling_2w_H0H8(&PP));
	CLOCKS(4-way_Addition,_4way_mixadd_ed448(&Q, &P));
//	CLOCKS_RANDOM(int t=0;, 4-way_Query,query_table_ed448(&P, ((uint8_t*)TableSign_w4_3675k)+SIZE_ONE_LUT_ED448*t,S, K));

	printf("======      MISC    ======\n");
	CLOCKS_RANDOM(random_bytes(key,ED448_KEY_SIZE_BYTES_PARAM),wnaf,wnaf_448bits(L, key, 5));
	CLOCKS_RANDOM(random_bytes(key,ED448_KEY_SIZE_BYTES_PARAM),Recoding,recoding_signed_scalar_fold4w4_448(S,K,key));
	CLOCKS_RANDOM(
			random_bytes(message,ED448_HASH_BYTES_PARAM),
			SHAKE256,SHAKE256(message,ED448_HASH_BYTES_PARAM,digest,ED448_HASH_BYTES_PARAM)
	);
}

