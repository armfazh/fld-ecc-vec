#include <eddsa_avx2.h>
#include "vectors.c"

#define LABEL "Test passed? [%s]\n"
#define OK "Ok"
#define ERROR "No"

static void test_vector(const EdDSA_TestData * test)
{
	Ed25519_PublicKey run_public25519;
	Ed25519_Signature run_signature25519;
	Ed448_PublicKey run_public448;
	Ed448_Signature run_signature448;

	argEdDSA_PublicKey run_publickey = NULL;
	argEdDSA_Signature run_signature = NULL;
    int valid = -1;

    printf("Test: %s\n",test->name_test);
	if(test->signature_scheme == Ed25519)
	{
		const SignatureScheme * scheme = &EdDSA.Ed25519;
		run_publickey= run_public25519;
		run_signature = run_signature25519;

		scheme->keygen(run_publickey, test->private_key);
		printf(LABEL, memcmp(run_publickey, test->public_key, scheme->key_size) == 0 ? OK : ERROR);
		scheme->sign(run_signature, test->message, test->message_length,
					 test->public_key,test->private_key);
		printf(LABEL, memcmp(run_signature, test->signature, scheme->signature_size) == 0 ? OK : ERROR);
		valid = scheme->verify(test->message, test->message_length,
							   run_publickey,run_signature);
		printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);
	}
	else
	{
		const SignatureSchemeCtx * schemectx;
		switch(test->signature_scheme)
		{
			case Ed448:
				schemectx = &EdDSA.Ed448;
				run_publickey = run_public448;
				run_signature = run_signature448;
				break;
			case Ed448ph:
				schemectx = &EdDSA.Ed448ph;
				run_publickey = run_public448;
				run_signature = run_signature448;
				break;
			case Ed25519ph:
				schemectx = &EdDSA.Ed25519ph;
				run_publickey = run_public25519;
				run_signature = run_signature25519;
				break;
			case Ed25519ctx:
				schemectx = &EdDSA.Ed25519ctx;
				run_publickey = run_public25519;
				run_signature = run_signature25519;
				break;
			default:
				schemectx = NULL;
				printf("Error in test.\n");
		}
		if(schemectx != NULL)
		{
			schemectx->keygen(run_publickey, test->private_key);
			printf(LABEL, memcmp(run_publickey, test->public_key, schemectx->key_size) == 0 ? OK : ERROR);
			schemectx->sign(run_signature, test->message, test->message_length,
							test->context, test->context_length, test->public_key,
							test->private_key);
			printf(LABEL, memcmp(run_signature, test->signature, schemectx->signature_size) == 0 ? OK : ERROR);
			valid = schemectx->verify(test->message, test->message_length,
									  test->context, test->context_length,
									  run_publickey, run_signature);
			printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);
		}
	}
}

static void test_eddsa()
{
    printf("===== EdDSA testing =====\n");
    int i=0;
    for(i=0; i<NUM_TEST_VECTORS; i++)
    {
        test_vector(vectors+i);
    }
}
