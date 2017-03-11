#include <ecc.h>
#include <sha3.h>
#include <table_sign_w4_3675k.h>
#include <eddsa_avx2.h>

static void bench_ecc()
{
	const int BENCH = 350;

	Ed448_PrivateKey key;
	ALIGN uint64_t S[112];
	ALIGN uint64_t K[112];
	ALIGN int8_t L[512];
	PointXYZT_4way Q;
	Point_precmp_4way P;
	PointXYZT_2w_H0H8 PP,QQ;
	Ed448_Digest digest;
	uint8_t message[ED448_HASH_BYTES_PARAM];

	printf("======       ECC    ======\n");

	CLOCKS(PointAddition,_1way_fulladd_2w_H0H8(&PP, &QQ));
	CLOCKS(PointDoubling,_1way_doubling_2w_H0H8(&PP));
	CLOCKS(4-way_Addition,_4way_mixadd_448(&Q, &P));
	CLOCKS_RANDOM(int t=0;, 4-way_Query,query_table_448(&P, ((uint8_t*)TableSign_w4_3675k)+SIZE_ONE_LUT*t,S, K));

	printf("======      MISC    ======\n");
	CLOCKS_RANDOM(random_bytes(key,ED448_KEY_SIZE_BYTES_PARAM),wnaf,wnaf_448bits(L, key, 5));
	CLOCKS_RANDOM(random_bytes(key,ED448_KEY_SIZE_BYTES_PARAM),Recoding,recoding_signed_scalar_fold4w4_448(S,K,key));
	CLOCKS_RANDOM(
			random_bytes(message,ED448_HASH_BYTES_PARAM),
			SHAKE256,SHAKE256(message,ED448_HASH_BYTES_PARAM,digest,ED448_HASH_BYTES_PARAM)
	);
}


