#ifndef _ED448_AVX2_VECTORS_H_
#define _ED448_AVX2_VECTORS_H_


enum EDDSA_SCHEME{
    Ed25519,
    Ed25519ph,
    Ed25519ctx,
    Ed448,
    Ed448ph
};

#define NUM_TEST_VECTORS 11
typedef struct _struct_EdDSA_testdata {
	char *name_test;
	enum EDDSA_SCHEME signature_scheme;
	uint8_t private_key[57];
	uint8_t public_key[57];
	uint8_t signature[114];
	uint8_t message[1024];
	unsigned long long message_length;
	uint8_t context[100];
	unsigned long long context_length;
} EdDSA_TestData;


#endif /* _ED448_AVX2_VECTORS_H_ */
